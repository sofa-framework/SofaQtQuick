#include "PythonAsset.h"

#include "SofaQtQuickGUI/SofaQtQuick_PythonEnvironment.h"
using sofaqtquick::PythonEnvironment;
#include <SofaPython3/PythonEnvironment.h>
#include <SofaPython3/PythonFactory.h>
namespace py = pybind11;

#include <memory>
#include <experimental/filesystem>
#include <QMessageBox>
#include <QProcess>

#include "AssetFactory.h"
#include "AssetFactory.inl"


namespace fs = std::experimental::filesystem;

namespace sofa::qtquick
{

/// Register all python assets extensions to the factory
static bool __pyscn = AssetFactory::registerAsset("pyscn", new AssetCreator<PythonAsset>());
static bool __py = AssetFactory::registerAsset("py", new AssetCreator<PythonAsset>());
static bool __prefab = AssetFactory::registerAsset("prefab", new AssetCreator<PythonAsset>());
static bool __pyctl = AssetFactory::registerAsset("pyctl", new AssetCreator<PythonAsset>());
static bool __pyeng = AssetFactory::registerAsset("pyeng", new AssetCreator<PythonAsset>());

const PythonAsset::LoaderMap PythonAsset::_loaders = PythonAsset::createLoaders();

std::map<std::string, BaseAssetLoader*> PythonAsset::createLoaders()
{
    PythonAsset::LoaderMap m;
    m["pyscn"] = new PythonAssetLoader<BaseObject>();
    m["prefab"] = new PythonAssetLoader<BaseObject>();
    m["pyctl"] = new PythonAssetLoader<BaseObject>();
    m["pyeng"] = new PythonAssetLoader<BaseObject>();
    m["py"] = new PythonAssetLoader<BaseObject>();
    return m;
}

PythonAsset::PythonAsset(std::string path, std::string extension)
    : Asset(path, extension)
{
}

sofaqtquick::bindings::SofaNode* PythonAsset::create(const QString& assetName)
{
    if (_loaders.find(m_extension) == _loaders.end() ||
            _loaders.find(m_extension)->second == nullptr)
    {
        std::cout << "Unknown file format." << std::endl;
        return new sofaqtquick::bindings::SofaNode(nullptr);
    }

    fs::path obj(m_path);

    std::string stem = obj.stem();
    std::string path = obj.parent_path().string();

    sofa::simulation::Node::SPtr root = sofa::core::objectmodel::New<sofa::simulation::graph::DAGNode>();
    root->setName("NEWNAME");
    py::module::import("Sofa.Core");
    py::object assetNode = sofapython3::PythonFactory::toPython(root->toBaseNode());
    py::module::import("SofaQtQuick").attr(
                "loadPythonAsset")(path, stem, assetName.toStdString(), assetNode);

    sofa::simulation::graph::DAGNode::SPtr node = sofa::simulation::graph::DAGNode::SPtr(
                dynamic_cast<sofa::simulation::graph::DAGNode*>(root.get()));
    node->init(sofa::core::ExecParams::defaultInstance());
    return new sofaqtquick::bindings::SofaNode(node, dynamic_cast<QObject*>(this));
}

PythonAssetModel::PythonAssetModel(std::string name, std::string type, std::string docstring, int lineno, std::vector<std::string> params, std::string sourcecode)
    : m_name(name.c_str()),
      m_type(type.c_str()),
      m_docstring(docstring.c_str()),
      m_sourcecode(sourcecode.c_str()),
      m_lineno(lineno)
{
    for (const auto& p : params)
        m_params.push_back(p.c_str());
}

const QString& PythonAssetModel::getName() const { return m_name; }
const QString& PythonAssetModel::getType() const { return m_type; }
const QString& PythonAssetModel::getDocstring() const { return m_docstring; }
int PythonAssetModel::getLineno() const { return m_lineno; }
const QStringList& PythonAssetModel::getParams() const { return m_params; }
const QString& PythonAssetModel::getSourceCode() const { return m_sourcecode; }

void PythonAssetModel::setName(const QString& name) { m_name = name; }
void PythonAssetModel::setType(const QString& type) { m_type = type; }
void PythonAssetModel::setDocstring(const QString& docstring) { m_docstring = docstring; }
void PythonAssetModel::setLineno(int lineno) { m_lineno = lineno; }
void PythonAssetModel::setParams(const QStringList& params) { m_params = params; }
void PythonAssetModel::setSourceCode(const QString& sourcecode) { m_sourcecode = sourcecode; }

void PythonAsset::getDetails()
{
    if (m_detailsLoaded) return;

    fs::path obj(m_path);

    std::string stem = obj.stem();
    std::string ext = obj.extension();
    std::string path = obj.parent_path().string();
    if (ext == ".pyscn")
    {
        QProcess process;
        process.start("/bin/mkdir", QStringList() << "-p" << "/tmp/runSofa2");
        process.waitForFinished(-1);
        process.start("/bin/cp", QStringList() << obj.string().c_str() << QString("/tmp/runSofa2/") + stem.c_str() + ".py");
        process.waitForFinished(-1);
        path = "/tmp/runSofa2";
    }

    QString docstring(PythonEnvironment::getPythonScriptDocstring(path, stem).c_str());
    if (!docstring.contains("type: SofaContent"))
    {
        QMessageBox mbox;
        mbox.setText("This python module does not contain the safe-guard "
                     "module docstring");
        mbox.setInformativeText("To be able to load this asset in runSofa2, "
                                "append these lines at the top of the python "
                                "script:\n \"\"\"type: SofaContent\"\"\"");
        mbox.setIcon(QMessageBox::Critical);
        mbox.setDefaultButton(QMessageBox::Ok);
        mbox.exec();
        return;
    }


    auto dict = PythonEnvironment::getPythonScriptContent(path, stem);

    for (auto item : m_scriptContent)
        delete item;
    m_scriptContent.clear();



    for (auto pair : dict)
    {
        if (!py::isinstance<py::dict>(pair.second))
            return;
        py::dict data = py::cast<py::dict>(pair.second);
        std::vector<std::string> params;
        for (auto p : py::cast<py::list>(data["params"]))
            params.push_back(py::cast<std::string>(p));
        m_scriptContent.append(new PythonAssetModel(
                py::cast<std::string>(pair.first),
                py::cast<std::string>(data["type"]),
                py::cast<std::string>(data["docstring"]),
                py::cast<int>(data["lineno"]),
                params,
                py::cast<std::string>(data["sourcecode"])));
    }
    m_detailsLoaded = true;
}

QUrl PythonAsset::getAssetInspectorWidget() {
    return QUrl("qrc:/SofaWidgets/PythonAssetInspector.qml");
}


QVariantList PythonAsset::scriptContent()
{
    getDetails();
    QVariantList list;
    for (const auto& item : m_scriptContent)
    {
        list.append(QVariant::fromValue(item));
    }
    return list;
}

bool PythonAsset::isScene()
{
    getDetails();
    for (const auto& item : m_scriptContent){
        if (item->getType() == "SofaScene")
            return true;
    }
    return false;
}

} // namespace sofa::qtquick
