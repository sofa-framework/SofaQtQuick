#include <QFile>
#include <QDir>
#include <QTemporaryFile>
#include <QFileInfo>
#include "PythonAsset.h"

#include "SofaQtQuickGUI/SofaQtQuick_PythonEnvironment.h"
using RSPythonEnvironment = sofaqtquick::PythonEnvironment;

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
        std::string value = py::cast<std::string>(py::repr(defaults[i]));
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
        msg_warning("PythonAsset") << " Unsupported asset type: '"<< assetName.toStdString() << "'" ;
        return nullptr;
    }

    QFileInfo fpath { QString::fromStdString(m_path) };
    QString filePath = fpath.absoluteFilePath();
    QString parentDirPath = fpath.dir().absolutePath();

    /// Construct the arguments to the creation function.
    py::list args;
    py::dict kwargs;

    /// There must have a "root/parent" argument.
    sofa::simulation::Node::SPtr root = parent->self();

    /// call the function
    py::object res = RSPythonEnvironment::CallFunction(filePath, assetName, args, py::dict(), root.get());

    ///py::print(py::str()),
    sofa::simulation::Node* resnode = py::cast<sofa::simulation::Node*>(res);
    resnode->init(sofa::core::ExecParams::defaultInstance());
    if( resnode )
        root->addChild( resnode );

    return new sofaqtquick::bindings::SofaNode(dynamic_cast<sofa::simulation::graph::DAGNode*>(resnode), dynamic_cast<QObject*>(this));
}

PythonAssetModel::PythonAssetModel(const QString& name, const QString& type,
                                   const QString& docstring, const QString& sourcecode, QList<QObject*> params)
    : m_name(name),
      m_type(type),
      m_docstring(docstring),
      m_sourcecode(sourcecode),
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

QString py2qt(const py::handle s)
{
    return QString::fromStdString(py::cast<std::string>(s));
}

void PythonAsset::getDetails()
{
    if (m_detailsLoaded)
        return;

    /// The file is destructed when the QTemporaryFile is removed.
    QString inFile { QString::fromStdString(m_path) };
    QString outFile { getTemporaryFileName(inFile) };
    QFileInfo outFile_finfo { outFile };
    copyFileToCache(inFile, outFile);

    for (auto item : m_scriptContent)
        delete item;
    m_scriptContent.clear();

    if (!RSPythonEnvironment::IsASofaPythonModule(outFile))
    {
        msg_warning_withfile("PythonAsset", m_path, 0) << "This python module '"<< inFile.toStdString() <<"' does not contain the safe-guard module docstring" << msgendl
                                                       << "To be able to load this asset in runSofa2, "
                                                          "append these lines at the top of the python "
                                                          "script:\n \"\"\"type: SofaContent\"\"\"";
        m_detailsLoaded=true;
        return;
    }

    auto dict = RSPythonEnvironment::GetPythonModuleContent(outFile_finfo.absolutePath(),
                                                            outFile_finfo.fileName());

    if(dict.is_none())
        return;

    for (auto pair : dict)
    {
        if (!py::isinstance<py::dict>(pair.second))
            continue;

        py::dict data = py::cast<py::dict>(pair.second);
        m_scriptContent.append(
                    new PythonAssetModel(
                        py2qt(pair.first),
                        py2qt(data["type"]),
                    py2qt(data["docstring"]),
                py2qt(data["sourcecode"]),
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

QString PythonAsset::getTemporaryFileName(const QString& inFile) const
{
    return QDir::tempPath() + "/" + QFileInfo(inFile).completeBaseName()+".py";
}

void PythonAsset::copyFileToCache(const QString& inFile, const QString& outFile) const
{
    /// The file is destructed when the QTemporaryFile is removed.
    QFileInfo foutFile { outFile };

    /// Copy does not work if the file already exists so we remove it.
    if( foutFile.exists() )
        QFile::remove(foutFile.absoluteFilePath());

    /// Copy the file.
    QFile::copy(inFile, foutFile.absoluteFilePath());
}

bool PythonAsset::getIsSofaContent()
{
    /// The file is destructed when the QTemporaryFile is removed.
    QString inFile { QString::fromStdString(m_path) };
    QString outFile { getTemporaryFileName(inFile) };

    copyFileToCache(inFile, outFile);
    return RSPythonEnvironment::IsASofaPythonModule(outFile);
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
