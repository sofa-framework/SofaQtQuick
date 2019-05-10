#include "MeshAsset.h"

#include <SofaOpenglVisual/OglModel.h>
#include <SofaBaseVisual/InteractiveCamera.h>
#include <sofa/helper/cast.h>
#include <sofa/simulation/InitVisitor.h>
#include <sofa/simulation/DefaultAnimationLoop.h>
#include <sofa/simulation/DefaultVisualManagerLoop.h>
//#include <SofaPython/SceneLoaderPY.h>

#include <SofaPython/PythonEnvironment.h>
#include <SofaPython/PythonFactory.h>
using sofa::simulation::PythonEnvironment;

using sofa::component::visualmodel::OglModel;
using sofa::core::loader::MeshLoader;
using sofa::component::visualmodel::InteractiveCamera;

#include <memory>

namespace sofa::qtquick
{

const QUrl MeshAsset::iconPath = QUrl("qrc:/icon/ICON_MESH.png");
const QString MeshAsset::typeString = "3D mesh file";
const MeshAsset::LoaderMap MeshAsset::loaders = MeshAsset::createLoaders();

std::map<std::string, BaseAssetLoader*> MeshAsset::createLoaders()
{
    MeshAsset::LoaderMap m;
    m["stl"] = new MeshAssetLoader<MeshSTLLoader>();
    m["obj"] = new MeshAssetLoader<MeshObjLoader>();
    m["vtk"] = new MeshAssetLoader<MeshVTKLoader>();
    m["gmsh"] = new MeshAssetLoader<MeshGmshLoader>();
    return m;
}

MeshAsset::MeshAsset(std::string path, std::string extension)
    : Asset(path, extension)
{
}

sofaqtquick::bindings::SofaNode* MeshAsset::getAsset(const std::string& assetName)
{
    SOFA_UNUSED(assetName);
    if (loaders.find(m_extension) == loaders.end() ||
            loaders.find(m_extension)->second == nullptr)
    {
        std::cout << "Unknown file format." << std::endl;
        return new sofaqtquick::bindings::SofaNode(this);
    }

    BaseObject::SPtr b = loaders.find(m_extension)->second->New();

    Node::SPtr root = sofa::core::objectmodel::New<DAGNode>();

    PythonEnvironment::gil lock(__func__);
    PyObject* loaderType = PyString_FromString(b->getClassName().c_str());
    PyObject* meshPath = PyString_FromString(m_path.c_str());
    PyObject* rootNode = sofa::PythonFactory::toPython(root.get()->toBaseNode());

    PyObject* args = PyTuple_Pack(3, loaderType, meshPath, rootNode);
    PyObject* ret = PythonEnvironment::callObject("loadMeshAsset", "SofaPython", args);
    if (PyObject_IsTrue(ret))
    {
        root->setName("NEWNODE");
        DAGNode::SPtr node = DAGNode::SPtr(dynamic_cast<DAGNode*>(root.get()));
        node->init(sofa::core::ExecParams::defaultInstance());

        return new sofaqtquick::bindings::SofaNode(node, dynamic_cast<QObject*>(this));
    }
    std::cout << "Something went wrong..." << std::endl;
    return nullptr;
}

QList<QObject*> MeshAsset::getAssetMetaInfo()
{
    return QList<QObject*>();
}


} // namespace sofa::qtquick
