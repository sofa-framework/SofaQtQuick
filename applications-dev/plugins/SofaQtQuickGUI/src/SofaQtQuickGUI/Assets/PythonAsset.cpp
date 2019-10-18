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


QList<QObject*> getPrefabParams(py::list args, py::tuple defaults, py::dict annotations)
{
    QList<QObject*> params;
    size_t diff = args.size() - defaults.size();
    for (size_t i = 0 ; i < diff ; ++i)
    {
        py::dict locals = py::dict();
        locals["defaults"] = defaults;
        defaults = py::eval("('',) + defaults", py::dict(), locals);
    }
    for (size_t i = 0 ; i < args.size() ; i++)
    {
        std::string argname = py::cast<std::string>(args[i]);
        std::string value = py::cast<std::string>(py::str(defaults[i]));
        std::string annotation = annotations.contains(args[i]) ? py::str(annotations[args[i]]) : "";

        if (argname == "self" || argname == "parent")
            continue;

        params.append(new PythonParam(argname, value, annotation));
    }
    return params;
}

QWizardPage* createArgsPage(QList<QObject*> params, std::list<QLineEdit*>& values, sofa::simulation::Node::SPtr parent = nullptr)
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Positional Arguments");

    QLabel *label = new QLabel("This Prefab requires " + QString(std::to_string(params.size() + 1).c_str()) + " positional arguments.\nPlease fill in the required fields:");
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    page->setLayout(layout);
    QVBoxLayout* form = new QVBoxLayout;

    // Add the parent param:
    QHBoxLayout* entry = new QHBoxLayout;
    QLabel* lbl = new QLabel("parent");
    QLineEdit* value = new QLineEdit("${Sofa.Core.Node}");
    if (parent != nullptr)
        value->setText(QString("@") + parent->getPathName().c_str());
    value->setReadOnly(true);
    value->setEnabled(false);
    value->setToolTip("the parent node for this prefab (non-modifyable)");
    value->setToolTipDuration(1000);
    values.push_back(value);
    entry->addWidget(lbl);
    entry->addWidget(value);
    form->addLayout(entry);

    // Add all other editable params:
    for (auto& param : params)
    {
        PythonParam* p = dynamic_cast<PythonParam*>(param);

        entry = new QHBoxLayout;
        lbl = new QLabel(p->getName());
        value = new QLineEdit(p->getValue());
        if (p->getAnnotation().size())
        {
            value->setToolTip(p->getAnnotation());
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
        return nullptr;
    }

    fs::path filePath(m_path);
    std::string stem = filePath.stem();
    std::string parentDirPath = filePath.parent_path().string();

    sofa::simulation::Node::SPtr root = parent->self();

    py::module::import("Sofa.Core");
    py::object assetNode = sofapython3::PythonFactory::toPython(root->toBaseNode());
    py::module inspect = py::module::import("inspect");
    py::module sys = py::module::import("sys");
    sys.attr("path").attr("append")(parentDirPath);

    auto local = py::dict();
    local["math"] = py::module::import("math");
    local["numpy"] = py::module::import("numpy");
    local["Sofa"] = py::module::import("Sofa");
    local["Sofa"] = py::dict();
    local["Sofa"]["Core"] = py::module::import("Sofa.Core");

    py::object module = py::module::import(stem.c_str());
    if(!module)
        return nullptr;

    py::object callable = module.attr(assetName.toStdString().c_str());
    py::object prefab;
    if (assetName != "createScene")
    {
        QWizard wizard;
        std::list<QLineEdit*> values;
        py::list expressions;
        PythonAssetModel* model = nullptr;
        for (auto& m : m_scriptContent)
            if (m->getName() == assetName)
                model = m;
        if (model == nullptr)
            return nullptr;
        QWizardPage* page = createArgsPage(model->getParams(), values, root);
        if (page != nullptr)
            wizard.addPage(page);
        wizard.setWindowTitle("Prefab instantiation wizard");

        try {
            for (auto entry : values)
            {
                if (entry->isReadOnly()) // The only field read only is the parent, which is imposed during drag n drop
                    expressions.append(sofapython3::PythonFactory::toPython(root->toBaseNode()));
                else
                    expressions.append(py::eval(entry->text().toStdString(), py::dict(), local));
            }
        } catch (py::error_already_set&){
            if (!wizard.exec())
                return nullptr;
            expressions = py::list();
            for (auto entry : values)
            {
                if (entry->isReadOnly()) // The only field read only is the parent, which is imposed during drag n drop
                    expressions.append(sofapython3::PythonFactory::toPython(root->toBaseNode()));
                else {
                    try {
                        expressions.append(py::eval(entry->text().toStdString(), py::dict(), local));
                    } catch (py::error_already_set&){
                        msg_error("PythonAsset") << "The provided value for parameter '" << entry->text().toStdString() << "' is invalid.";
                        return nullptr;
                    }
                }
            }
        }
        prefab = callable(*expressions);
    }
    else
    {
        prefab = callable(sofapython3::PythonFactory::toPython(root->toBaseNode()));
    }
    sofa::simulation::graph::DAGNode::SPtr node = sofa::simulation::graph::DAGNode::SPtr(
                dynamic_cast<sofa::simulation::graph::DAGNode*>(py::cast<sofa::simulation::Node*>(prefab)));

    if(node.get()!=nullptr)
       node->init(sofa::core::ExecParams::defaultInstance());

    return new sofaqtquick::bindings::SofaNode(node, dynamic_cast<QObject*>(this));
}

PythonAssetModel::PythonAssetModel(std::string name, std::string type, std::string docstring, std::string sourcecode, QList<QObject*> params)
    : m_name(name.c_str()),
      m_type(type.c_str()),
      m_docstring(docstring.c_str()),
      m_sourcecode(sourcecode.c_str()),
      m_params(params)
{
}

void PythonAssetModel::openSettings()
{
    QWizard wizard;
    std::list<QLineEdit*> values;
    py::list expressions;
    QWizardPage* page = createArgsPage(m_params, values);
    if (page != nullptr)
        wizard.addPage(page);
    wizard.setWindowTitle("Prefab instantiation wizard");
    wizard.exec();

    int i = 0;
    for (auto entry : values)
    {
        if (entry->isReadOnly())
            continue;
        dynamic_cast<PythonParam*>(m_params[i])->setValue(entry->text());
        i++;
    }
}

const QString& PythonAssetModel::getName() const { return m_name; }
const QString& PythonAssetModel::getType() const { return m_type; }
const QString& PythonAssetModel::getDocstring() const { return m_docstring; }
const QList<QObject*>& PythonAssetModel::getParams() const { return m_params; }
const QString& PythonAssetModel::getSourceCode() const { return m_sourcecode; }

void PythonAssetModel::setName(const QString& name) { m_name = name; }
void PythonAssetModel::setType(const QString& type) { m_type = type; }
void PythonAssetModel::setDocstring(const QString& docstring) { m_docstring = docstring; }
void PythonAssetModel::setParams(const QList<QObject*>& params) { m_params = params; }
void PythonAssetModel::setSourceCode(const QString& sourcecode) { m_sourcecode = sourcecode; }

void PythonAsset::getDetails()
{
    if (m_detailsLoaded)
        return;

    fs::path filePath(m_path);
    std::string stem = filePath.stem();
    std::string ext = filePath.extension();
    std::string parentDirPath = filePath.parent_path().string();
    if (ext == ".pyscn")
    {
        QProcess process;
        process.start("/bin/mkdir", QStringList() << "-p" << "/tmp/runSofa2");
        process.waitForFinished(-1);
        process.start("/bin/cp", QStringList() << filePath.string().c_str() << QString("/tmp/runSofa2/") + stem.c_str() + ".py");
        process.waitForFinished(-1);
        parentDirPath = "/tmp/runSofa2";
    }

    std::string docstring;
    if (!PythonEnvironment::getPythonScriptDocstring(parentDirPath, stem, docstring))
    {
        msg_warning_withfile("PythonAsset", m_path, 0) << "Unable to load the module";
        return;
    }
    if (!QString(docstring.c_str()).contains("type: SofaContent"))
    {
        msg_warning_withfile("PythonAsset", m_path, 0) << "This python module does not contain the safe-guard module docstring" << msgendl
                                   << "To be able to load this asset in runSofa2, "
                                      "append these lines at the top of the python "
                                      "script:\n \"\"\"type: SofaContent\"\"\"";
        return;
    }


    auto dict = PythonEnvironment::getPythonScriptContent(parentDirPath, stem);

    for (auto item : m_scriptContent)
        delete item;
    m_scriptContent.clear();

    for (auto pair : dict)
    {
        if (!py::isinstance<py::dict>(pair.second))
            return;
        py::dict data = py::cast<py::dict>(pair.second);
        m_scriptContent.append(
                    new PythonAssetModel(
                        py::cast<std::string>(pair.first),
                        py::cast<std::string>(data["type"]),
                    py::cast<std::string>(data["docstring"]),
                py::cast<std::string>(data["sourcecode"]),
                getPrefabParams(
                    data["params"].attr("args").is_none() ? py::list() : data["params"].attr("args"),
                data["params"].attr("defaults").is_none() ? py::tuple() : data["params"].attr("defaults"),
                data["params"].attr("args").is_none() ? py::dict() : data["params"].attr("annotations"))
                ));
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
