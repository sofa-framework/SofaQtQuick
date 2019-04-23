#include "MeshAsset.h"

#include <SofaOpenglVisual/OglModel.h>
#include <SofaBaseVisual/InteractiveCamera.h>
#include <sofa/helper/cast.h>
#include <sofa/simulation/InitVisitor.h>
#include <sofa/simulation/DefaultAnimationLoop.h>
#include <sofa/simulation/DefaultVisualManagerLoop.h>

using sofa::component::visualmodel::OglModel;
using sofa::core::loader::MeshLoader;
using sofa::component::visualmodel::InteractiveCamera;

#include <memory>

namespace sofa
{
namespace qtquick
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

SofaComponent* MeshAsset::getPreviewNode()
{
    if (loaders.find(m_extension) == loaders.end() ||
            loaders.find(m_extension)->second == nullptr)
    {
        std::cout << "Unknown file format." << std::endl;
        return new SofaComponent(nullptr, this);
    }

    m_node = sofa::core::objectmodel::New<DAGNode>("3DMesh");
    std::cout << m_node->getName() << std::endl;

    sofa::simulation::DefaultAnimationLoop::SPtr aloop = sofa::core::objectmodel::New<sofa::simulation::DefaultAnimationLoop>(m_node.get());
    aloop->setName(sofa::core::objectmodel::BaseObject::shortName(aloop.get()));
    m_node->addObject(aloop);

    sofa::simulation::DefaultVisualManagerLoop::SPtr vloop = sofa::core::objectmodel::New<sofa::simulation::DefaultVisualManagerLoop>(m_node.get());
    vloop->setName(sofa::core::objectmodel::BaseObject::shortName(vloop.get()));
    m_node->addObject(vloop);

    InteractiveCamera::SPtr camera = sofa::core::objectmodel::New<InteractiveCamera>();
    camera->setName(sofa::core::objectmodel::BaseObject::shortName(camera.get()));
    m_node->addObject(camera);

    BaseObject::SPtr b = loaders.find(m_extension)->second->New();
    MeshLoader::SPtr loader(dynamic_cast<MeshLoader*>(b.get()));
    loader->setFilename(m_path);
    loader->setName("loader");
    loader->name.cleanDirty();
    std::cout << loader->getName() << std::endl;
    m_node->addObject(loader);

    OglModel::SPtr vmodel = sofa::core::objectmodel::New<OglModel>();
    vmodel->setSrc("@loader", loader.get());
    vmodel->setName("vmodel");
    vmodel->name.cleanDirty();
    std::cout << vmodel->getName() << std::endl;
    m_node->addObject(vmodel);

    return new SofaComponent(nullptr, m_node.get());
}

} // namespace qtquick
} // namespace sofa
