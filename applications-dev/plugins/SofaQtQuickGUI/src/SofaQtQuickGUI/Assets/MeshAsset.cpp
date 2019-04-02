#include "MeshAsset.h"

#include <SofaOpenglVisual/OglModel.h>
#include <SofaBaseVisual/InteractiveCamera.h>
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
    if (loaders.find(m_extension) == loaders.end())
    {
        msg_error("Unknown file format.");
        return new SofaComponent(nullptr, this);
    }

    this->setName("root");
    MeshLoader::SPtr loader(dynamic_cast<MeshLoader*>(loaders.find(m_extension)->second->New()));
    loader->setName("loader");
    loader->setFilename(m_path);

    OglModel::SPtr vmodel = sofa::core::objectmodel::New<OglModel>();
    vmodel->setSrc("@loader", loader.get());
    vmodel->setName("vmodel");


    InteractiveCamera::SPtr camera = sofa::core::objectmodel::New<InteractiveCamera>();
    camera->setName("camera");

    this->addObject(camera);
    this->addObject(loader);
    this->addObject(vmodel);
    this->init(sofa::core::ExecParams::defaultInstance());
    return new SofaComponent(nullptr, this);
}

} // namespace qtquick
} // namespace sofa
