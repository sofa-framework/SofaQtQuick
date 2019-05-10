#include "PythonAsset.h"

#include <SofaPython/SceneLoaderPY.h>
#include <SofaPython/PythonEnvironment.h>

#include <memory>
#include <experimental/filesystem>
#include <SofaPython/PythonEnvironment.h>
#include <SofaPython/PythonFactory.h>
using sofa::simulation::PythonEnvironment;

namespace fs = std::experimental::filesystem;

namespace sofa::qtquick
{

const QUrl PythonAsset::iconPath = QUrl("qrc:/icon/ICON_PYTHON.png");
const QString PythonAsset::typeString = "Python prefab";
const PythonAsset::LoaderMap PythonAsset::loaders = PythonAsset::createLoaders();

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

sofaqtquick::bindings::SofaNode* PythonAsset::getAsset(const std::string& assetName)
{
    if (loaders.find(m_extension) == loaders.end() ||
            loaders.find(m_extension)->second == nullptr)
    {
        std::cout << "Unknown file format." << std::endl;
        return new sofaqtquick::bindings::SofaNode(this);
    }

    fs::path obj(m_path);

    std::string stem = obj.stem();
    std::string path = obj.parent_path().string();

    Node::SPtr root = sofa::core::objectmodel::New<DAGNode>();


    PythonEnvironment::gil lock(__func__);
    PyObject* mpath = PyString_FromString(path.c_str());
    PyObject* mname = PyString_FromString(stem.c_str());
    PyObject* pname = PyString_FromString(assetName.c_str());
    PyObject* assetnode = sofa::PythonFactory::toPython(root.get());

    PyObject* args = PyTuple_Pack(4, mpath, mname, pname, assetnode);
    PyObject* ret = PythonEnvironment::callObject("loadPythonAsset", "SofaPython", args);
    if (PyObject_IsTrue(ret))
    {
        DAGNode::SPtr node = DAGNode::SPtr(dynamic_cast<DAGNode*>(root.get()));
        node->init(sofa::core::ExecParams::defaultInstance());

        return new sofaqtquick::bindings::SofaNode(node, dynamic_cast<QObject*>(this));
    }
    return nullptr;
}

PythonAssetModel::PythonAssetModel(std::string name, std::string type)
    : m_name(name),
      m_type(type)
{}

QString PythonAssetModel::name() const { return m_name.c_str(); }
QString PythonAssetModel::type() const { return m_type.c_str(); }

QList<QObject*> PythonAsset::getAssetMetaInfo()
{
    fs::path obj(m_path);

    std::string stem = obj.stem();
    std::string path = obj.parent_path().string();

    std::map<std::string, std::string> map;
    map = PythonEnvironment::getPythonModuleContent(path, stem);

    QList<QObject*> list;
    for (auto pair : map)
        list.append(new PythonAssetModel(pair.first, pair.second));
    return list;
}


} // namespace sofa::qtquick
