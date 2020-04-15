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

#include "AssetFactory.h"
#include "AssetFactory.inl"

namespace fs = std::experimental::filesystem;

namespace sofaqtquick
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
    sofapython3::PythonEnvironment::gil acquire;

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

sofaqtquick::bindings::SofaNode* PythonAsset::create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName)
{    
    sofapython3::PythonEnvironment::gil acquire;

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

    QString asset_name = assetName;
    if (asset_name == "" && m_assetsContent.size() > 0)
        asset_name = m_assetsContent.first()["name"].toString();
    /// An asset needs a context.
    bool isContextFree { m_assetsContent[asset_name]["type"] == "SofaPrefab" };

    /// Some python asset can be created in a root less manner. This is the case for
    /// the SofaPrefab.
    if( !isContextFree )
        args.append(sofapython3::PythonFactory::toPython(root.get()));

    /// call the function
    py::object res = RSPythonEnvironment::CallFunction(filePath, asset_name, args, py::dict(), root.get());

    if(res.is_none())
    {
        return parent;
    }

    auto base = py::cast<sofa::core::objectmodel::Base*>(res);
    if(base->toBaseNode() != nullptr)
    {
        auto resnode = dynamic_cast<sofa::simulation::graph::DAGNode*>(base->toBaseNode());

        if(isContextFree)
        {
            parent->addChild(resnode);
        }
        resnode->init(sofa::core::ExecParams::defaultInstance());
        return new sofaqtquick::bindings::SofaNode(resnode, dynamic_cast<QObject*>(this));
    }
    if(base->toBaseObject() != nullptr)
    {
        auto object = base->toBaseObject();
        parent->addObject( new SofaBaseObject(sofa::core::objectmodel::BaseObject::SPtr(object)) );
        object->init();
    }
    return parent;
}

QString py2qt(const py::handle s)
{
    return QString::fromStdString(py::cast<std::string>(s));
}

void PythonAsset::getDetails()
{
    sofapython3::PythonEnvironment::gil acquire;

    /// Let's clear the old content.
    m_assetsContent.clear();

    /// The file is destructed when the QTemporaryFile is removed.
    QString inFile { QString::fromStdString(m_path) };
    QString outFile { getTemporaryFileName(inFile) };
    QFileInfo outFile_finfo { outFile };
    copyFileToCache(inFile, outFile);

    if (!RSPythonEnvironment::IsASofaPythonModule(outFile))
    {
        msg_warning_withfile("PythonAsset", m_path, 0) << "This python module '"<< inFile.toStdString() <<"' does not contain the safe-guard module docstring" << msgendl
                                                       << "To be able to load this asset in runSofa2, "
                                                          "append these lines at the top of the python "
                                                          "script:\n \"\"\"type: SofaContent\"\"\"";
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
        m_assetsContent[py2qt(pair.first)] = { {{"name", py2qt(pair.first)},
                                               {"type", py2qt(data["type"])},
                                               {"docstring", py2qt(data["docstring"])},
                                               {"sourcecode", py2qt(data["sourcecode"])},
                                               {"lineno", py2qt(data["lineno"])}}
                                             };
    }
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

QVariantList PythonAsset::scriptActions()
{
    getDetails();
    QVariantList list;
    for (const auto& item : m_assetsContent)
    {
        if (item["type"] == "SofaScene") {
            QVariantMap v = QMap(item);
            v["type"] = QVariant::fromValue(QString("SofaScene (load)"));
            list.append(v);
        }
        list.append( item );
    }
    return list;
}

QVariantList PythonAsset::scriptContent()
{
    getDetails();
    QVariantList list;
    for (const auto& item : m_assetsContent)
        list.append( item );
    return list;
}

bool PythonAsset::isScene()
{
    getDetails();
    for (const auto& item : m_assetsContent){
        if (item["type"] == "SofaScene")
            return true;
    }
    return false;
}

} // namespace sofaqtquick
