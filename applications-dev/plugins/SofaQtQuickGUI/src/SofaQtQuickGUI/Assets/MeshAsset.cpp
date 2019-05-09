#include "MeshAsset.h"

#include <SofaOpenglVisual/OglModel.h>
#include <SofaBaseVisual/InteractiveCamera.h>
#include <sofa/helper/cast.h>
#include <sofa/simulation/InitVisitor.h>
#include <sofa/simulation/DefaultAnimationLoop.h>
#include <sofa/simulation/DefaultVisualManagerLoop.h>
#include <SofaPython/SceneLoaderPY.h>

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

    sofa::simulation::SceneLoaderPY scnLoader;

    BaseObject::SPtr b = loaders.find(m_extension)->second->New();

    Node::SPtr root;
    std::vector<std::string> args;
    args.push_back(b->getClassName());
    args.push_back(m_path);
    scnLoader.loadSceneWithArguments("config/templates/MeshAsset.py",
                                     args, &root);

    root->setName("NEWNODE");
    DAGNode::SPtr node = DAGNode::SPtr(dynamic_cast<DAGNode*>(root.get()));
    node->init(sofa::core::ExecParams::defaultInstance());
    return new sofaqtquick::bindings::SofaNode(node, dynamic_cast<QObject*>(this));
}

QList<QObject*> MeshAsset::getAssetMetaInfo()
{
    return QList<QObject*>();
}


} // namespace sofa::qtquick
