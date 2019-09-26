#include "PythonAsset.h"

#include "SofaQtQuickGUI/SofaQtQuick_PythonEnvironment.h"
using sofaqtquick::PythonEnvironment;
#include <SofaPython3/PythonEnvironment.h>
#include <SofaPython3/PythonFactory.h>
namespace py = pybind11;

#include <memory>
#include <experimental/filesystem>
#include <QMessageBox>
#include <QWizard>
#include <QProcess>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

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

QWizardPage* createArgsPage(py::list args, py::tuple defaults, py::dict annotations, std::list<QLineEdit*>& values, sofa::simulation::Node::SPtr parent)
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Positional Arguments");

    QLabel *label = new QLabel("This Prefab requires " + QString(std::to_string(args.size() - 1).c_str()) + " positional arguments.\nPlease fill in the required fields:");
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    page->setLayout(layout);
    QVBoxLayout* form = new QVBoxLayout;

    size_t diff = args.size() - defaults.size();
    for (size_t i = 0 ; i < diff ; ++i)
    {
        py::dict locals = py::dict();
        locals["defaults"] = defaults;
        defaults = py::eval("('',) + defaults", py::dict(), locals);
    }
    for (size_t i = 0 ; i < args.size() ; i++)
    {
        QString argname = QString::fromStdString(py::cast<std::string>(args[i]));
        QString defaultValue = QString::fromStdString((defaults.size() > i) ? py::cast<std::string>(py::str(defaults[i])) : "");
        QString annotation = (annotations.contains(args[i]) ? QString::fromStdString(py::cast<std::string>(annotations[args[i]])) : "");

        if (argname == "self")
            continue;

        QHBoxLayout* entry = new QHBoxLayout;
        QLabel* lbl = new QLabel(argname);
        QLineEdit* value = new QLineEdit(defaultValue);
        if (argname == "parent") {
            value->setText(QString("@") + parent->getPathName().c_str());
            value->setReadOnly(true);
            value->setEnabled(false);
        }
        if (annotation.size()) {
            value->setToolTip(annotation);
            value->setToolTipDuration(1000);
        }
        values.push_back(value);
        entry->addWidget(lbl);
        entry->addWidget(value);
        form->addLayout(entry);
    }
    layout->addLayout(form);
    return page;
}


sofaqtquick::bindings::SofaNode* PythonAsset::create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName)
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

    sofa::simulation::Node::SPtr root = parent->self();

    py::module::import("Sofa.Core");
    py::object assetNode = sofapython3::PythonFactory::toPython(root->toBaseNode());
    py::module inspect = py::module::import("inspect");
    py::module sys = py::module::import("sys");
    sys.attr("path").attr("append")(path);

    auto local = py::dict();
    local["math"] = py::module::import("math");
    local["numpy"] = py::module::import("numpy");
    local["Sofa"] = py::module::import("Sofa");
    local["Sofa"] = py::dict();
    local["Sofa"]["Core"] = py::module::import("Sofa.Core");

    py::object callable = py::module::import(stem.c_str()).attr(assetName.toStdString().c_str());
    py::object fullargspec = inspect.attr("getfullargspec")(callable.attr("__wrapped__"));
    py::list args = fullargspec.attr("args");
//    py::str varargs = fullargspec.attr("varargs");
//    py::str varkw = fullargspec.attr("varkw");
    py::tuple defaults = fullargspec.attr("defaults");
//    py::list kwonlyargs = fullargspec.attr("kwonlyargs");
//    py::dict kwonlydefaults = fullargspec.attr("kwonlydefaults");
    py::dict annotations = fullargspec.attr("annotations");

    QWizard wizard;
    std::list<QLineEdit*> values;
    py::list expressions;
    QWizardPage* page = createArgsPage(args, defaults, annotations, values, root);
    if (page != nullptr)
        wizard.addPage(page);
    wizard.setWindowTitle("Prefab instantiation wizard");
    wizard.exec();

    py::module ctypes = py::module::import("ctypes");
    for (auto entry : values)
    {
        if (entry->isReadOnly()) // The only field read only is the parent, which is imposed during drag n drop
            expressions.append(sofapython3::PythonFactory::toPython(root->toBaseNode()));
        else
            expressions.append(py::eval(entry->text().toStdString(), py::dict(), local));
    }
    std::cout << py::cast<std::string>(py::str(expressions)) << std::endl;
    py::object prefab = callable(*expressions);

    sofa::simulation::graph::DAGNode::SPtr node = sofa::simulation::graph::DAGNode::SPtr(
                dynamic_cast<sofa::simulation::graph::DAGNode*>(root.get()));
    node->init(sofa::core::ExecParams::defaultInstance());
    return new sofaqtquick::bindings::SofaNode(node, dynamic_cast<QObject*>(this));
}

PythonAssetModel::PythonAssetModel(std::string name, std::string type, std::string docstring, std::vector<std::string> params, std::string sourcecode)
    : m_name(name.c_str()),
      m_type(type.c_str()),
      m_docstring(docstring.c_str()),
      m_sourcecode(sourcecode.c_str())
{
    for (const auto& p : params)
        m_params.push_back(p.c_str());
}

const QString& PythonAssetModel::getName() const { return m_name; }
const QString& PythonAssetModel::getType() const { return m_type; }
const QString& PythonAssetModel::getDocstring() const { return m_docstring; }
const QStringList& PythonAssetModel::getParams() const { return m_params; }
const QString& PythonAssetModel::getSourceCode() const { return m_sourcecode; }

void PythonAssetModel::setName(const QString& name) { m_name = name; }
void PythonAssetModel::setType(const QString& type) { m_type = type; }
void PythonAssetModel::setDocstring(const QString& docstring) { m_docstring = docstring; }
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
