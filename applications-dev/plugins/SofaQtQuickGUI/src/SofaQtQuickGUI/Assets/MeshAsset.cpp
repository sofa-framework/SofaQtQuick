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

#include "PythonAsset.h"
#include <QProcess>

#include "AssetFactory.h"
#include "AssetFactory.inl"

namespace sofa::qtquick
{

/// Register all mesh assets extensions to the factory
bool __stl = AssetFactory::registerAsset("stl", new AssetCreator<MeshAsset>());
bool __obj = AssetFactory::registerAsset("obj", new AssetCreator<MeshAsset>());
bool __vtk = AssetFactory::registerAsset("vtk", new AssetCreator<MeshAsset>());
bool __gmsh = AssetFactory::registerAsset("gmsh", new AssetCreator<MeshAsset>());

const MeshAsset::LoaderMap MeshAsset::_loaders = MeshAsset::createLoaders();

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

sofaqtquick::bindings::SofaNode* MeshAsset::create(const QString& assetName)
{
    SOFA_UNUSED(assetName);
    if (_loaders.find(m_extension) == _loaders.end() ||
            _loaders.find(m_extension)->second == nullptr)
    {
        msg_error("Unknown file format.");
        return new sofaqtquick::bindings::SofaNode(nullptr);
    }

    BaseObject::SPtr b = _loaders.find(m_extension)->second->New();

    sofa::simulation::Node::SPtr root = sofa::core::objectmodel::New<sofa::simulation::graph::DAGNode>();

    PythonEnvironment::gil lock(__func__);
    PyObject* loaderType = PyString_FromString(b->getClassName().c_str());
    PyObject* meshPath = PyString_FromString(m_path.c_str());
    PyObject* rootNode = sofa::PythonFactory::toPython(root.get()->toBaseNode());

    PyObject* args = PyTuple_Pack(3, loaderType, meshPath, rootNode);
    PyObject* ret = PythonEnvironment::callObject("loadMeshAsset", "SofaPython", args);
    if (PyObject_IsTrue(ret))
    {
        root->setName("NEWNODE");
        sofa::simulation::graph::DAGNode::SPtr node = sofa::simulation::graph::DAGNode::SPtr(
                    dynamic_cast<sofa::simulation::graph::DAGNode*>(root.get()));
        node->init(sofa::core::ExecParams::defaultInstance());

        return new sofaqtquick::bindings::SofaNode(node, dynamic_cast<QObject*>(this));
    }
    msg_error("Something went wrong...");
    return nullptr;
}

void MeshAsset::getDetails()
{
    if (m_detailsLoaded) return;
    QProcess process;
    process.start("/usr/bin/assimp", QStringList() << "info" << QString(m_path.c_str()));
    process.waitForFinished(-1);
    QString output = process.readAllStandardOutput();
    QStringList parameterList = output.split("\n");

    for (QString& line : parameterList)
    {
        if (line.contains("Meshes:")) {
            m_meshes = line.split("Meshes:")[1].toInt();
        } else if (line.contains("Faces:")) {
            m_faces= line.split("Faces:")[1].toInt();
        } else if (line.contains("Vertices:")) {
            m_vertices = line.split("Vertices:")[1].toInt();
        } else if (line.contains("Materials:")) {
            m_materials = line.split("Materials:")[1].toInt();
        } else if (line.contains("Primitive Types:")) {
            m_primitiveType = line.split("Primitive Types:")[1].trimmed();
        }
    }

    m_minPoint = QList<double>() << -1.0 << -1.0 << -1.0;
    m_maxPoint = QList<double>() << 1.0 << 1.0 << 1.0;
    m_centerPoint = QList<double>() << 0.0 << 0.0 << 0.0;

    m_detailsLoaded = true;
}


} // namespace sofa::qtquick
