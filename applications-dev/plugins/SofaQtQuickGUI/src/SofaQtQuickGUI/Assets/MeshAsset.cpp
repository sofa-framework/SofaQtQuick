#include "MeshAsset.h"

#include <SofaOpenglVisual/OglModel.h>
#include <SofaBaseVisual/InteractiveCamera.h>
#include <sofa/helper/cast.h>
#include <sofa/simulation/InitVisitor.h>

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
    BaseObject::SPtr b = loaders.find(m_extension)->second->New();
    MeshLoader::SPtr loader(dynamic_cast<MeshLoader*>(b.get()));
    loader->setFilename(m_path);
    loader->setName("loader");
    loader->name.cleanDirty();
    std::cout << loader->getName() << std::endl;

    OglModel::SPtr vmodel = sofa::core::objectmodel::New<OglModel>();
    vmodel->setSrc("@loader", loader.get());
    vmodel->setName("vmodel");
    vmodel->name.cleanDirty();
    std::cout << vmodel->getName() << std::endl;


//    InteractiveCamera::SPtr camera = sofa::core::objectmodel::New<InteractiveCamera>();
//    camera->setName("camera");

//    this->addObject(camera);
    m_node->addObject(loader);
    m_node->addObject(vmodel);

    loader->init();
    loader->reinit();
    loader->bwdInit();
    vmodel->init();
    vmodel->reinit();
    vmodel->bwdInit();
    return new SofaComponent(nullptr, m_node.get());
}

} // namespace qtquick
} // namespace sofa
