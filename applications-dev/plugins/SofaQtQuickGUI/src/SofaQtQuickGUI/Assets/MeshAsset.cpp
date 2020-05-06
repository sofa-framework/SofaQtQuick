#include "MeshAsset.h"

#include <SofaQtQuickGUI/SofaBaseApplication.h>
#include <SofaOpenglVisual/OglModel.h>
#include <SofaBaseVisual/InteractiveCamera.h>
#include <sofa/helper/cast.h>
#include <sofa/helper/Utils.h>
#include <sofa/simulation/InitVisitor.h>
#include <sofa/simulation/DefaultAnimationLoop.h>
#include <sofa/simulation/DefaultVisualManagerLoop.h>

#include <sofa/simulation/VisualVisitor.h>
using sofa::simulation::VisualInitVisitor;

#include <SofaPython3/PythonEnvironment.h>
#include <SofaPython3/PythonFactory.h>
using sofapython3::PythonEnvironment;
namespace py = pybind11;

using sofa::component::visualmodel::OglModel;
using sofa::core::loader::MeshLoader;
using sofa::component::visualmodel::InteractiveCamera;

#include <memory>

#include "PythonAsset.h"
#include <QProcess>

#include "AssetFactory.h"
#include "AssetFactory.inl"

namespace sofaqtquick
{

/// Register all mesh assets extensions to the factory
static bool __stl = AssetFactory::registerAsset("stl", new AssetCreator<MeshAsset>());
static bool __obj = AssetFactory::registerAsset("obj", new AssetCreator<MeshAsset>());
static bool __vtk = AssetFactory::registerAsset("vtk", new AssetCreator<MeshAsset>());
static bool __gmsh = AssetFactory::registerAsset("gmsh", new AssetCreator<MeshAsset>());

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

sofaqtquick::bindings::SofaNode* MeshAsset::create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName)
{

    SOFA_UNUSED(assetName);
    sofapython3::PythonEnvironment::gil acquire;
    if (_loaders.find(m_extension) == _loaders.end() ||
            _loaders.find(m_extension)->second == nullptr)
    {
        msg_error("Unknown file format.");
        return new sofaqtquick::bindings::SofaNode(nullptr);
    }

    BaseObject::SPtr b = _loaders.find(m_extension)->second->New();

    sofa::simulation::Node::SPtr root = sofa::core::objectmodel::New<sofa::simulation::graph::DAGNode>();
    py::module::import("Sofa.Core");
    py::module::import("SofaQtQuick").attr("loadMeshAsset")(b->getClassName(), m_path, sofapython3::PythonFactory::toPython(root->toBaseNode()));

    sofa::simulation::graph::DAGNode::SPtr node = sofa::simulation::graph::DAGNode::SPtr(
                dynamic_cast<sofa::simulation::graph::DAGNode*>(root.get()));

    auto params = sofa::core::ExecParams::defaultInstance();
    /// Initialize the object
    node->init(params);

//    parent->self()->addChild(node);

    if (!parent->addChild(node.get())) return nullptr;

    /// Initialize the object assets.
    node->init(params);

    return new sofaqtquick::bindings::SofaNode(node, dynamic_cast<QObject*>(this));
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

QUrl MeshAsset::getAssetInspectorWidget() {
    return QUrl("qrc:/SofaWidgets/MeshAssetInspector.qml");
}

void MeshAsset::openThirdParty()
{
    QProcess p;

    if (m_extension == "vtk" || m_extension == "vtu")
        p.start("/usr/bin/paraview", QStringList() << path());
    else
        p.start("/snap/bin/blender", QStringList() << "--python" << QString::fromStdString(sofa::helper::Utils::getExecutableDirectory() + "/config/templates/open" + m_extension + "InBlender.py") << "--" << path());
    p.waitForFinished(-1);
}

} // namespace sofaqtquick
