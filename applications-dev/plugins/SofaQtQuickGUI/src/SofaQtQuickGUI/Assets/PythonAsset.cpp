#include "PythonAsset.h"

#include <SofaPython/SceneLoaderPY.h>
#include <SofaPython/PythonEnvironment.h>

#include <memory>
#include <experimental/filesystem>
#include <SofaPython/PythonEnvironment.h>
#include <SofaPython/PythonFactory.h>
#include <QMessageBox>

#include "AssetFactory.h"
#include "AssetFactory.inl"

using sofa::simulation::PythonEnvironment;

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

    PythonEnvironment::gil lock(__func__);
    PyObject* mpath = PyString_FromString(path.c_str());
    PyObject* mname = PyString_FromString(stem.c_str());
    PyObject* pname = PyString_FromString(assetName.toStdString().c_str());
    PyObject* assetnode = sofa::PythonFactory::toPython(root.get());

    PyObject* args = PyTuple_Pack(4, mpath, mname, pname, assetnode);
    PythonEnvironment::callObject("loadPythonAsset", "SofaPython", args);
    sofa::simulation::graph::DAGNode::SPtr node = sofa::simulation::graph::DAGNode::SPtr(
                dynamic_cast<sofa::simulation::graph::DAGNode*>(root.get()));
    node->init(sofa::core::ExecParams::defaultInstance());
    return new sofaqtquick::bindings::SofaNode(node, dynamic_cast<QObject*>(this));
}

PythonAssetModel::PythonAssetModel(std::string name, std::string type, std::string docstring)
    : m_name(name),
      m_type(type),
      m_docstring(docstring)
{}

QString PythonAssetModel::getName() const { return QString(m_name.c_str()); }
QString PythonAssetModel::getType() const { return QString(m_type.c_str()); }
QString PythonAssetModel::getDocstring() const { return QString(m_docstring.c_str()); }

void PythonAsset::getDetails()
{
    if (m_detailsLoaded) return;
    QString docstring(PythonEnvironment::getPythonModuleDocstring(m_path).c_str());
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

    fs::path obj(m_path);

    std::string stem = obj.stem();
    std::string path = obj.parent_path().string();

    std::map<std::string, std::map<std::string, std::string>> map;
    map = PythonEnvironment::getPythonModuleContent(path, stem);

    for (auto item : m_scriptContent)
        delete item;
    m_scriptContent.clear();
    for (auto pair : map)
        m_scriptContent.append(new PythonAssetModel(
                                   pair.first,
                                   pair.second["type"],
                               pair.second["docstring"]));
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
        if (item->getName() == "createScene")
            return true;
    }
    return false;
}

} // namespace sofa::qtquick
