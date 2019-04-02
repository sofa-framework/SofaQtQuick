#include "TextureAsset.h"


#include <SofaOpenglVisual/OglModel.h>
using sofa::component::visualmodel::OglModel;
namespace sofa
{
namespace qtquick
{

const QUrl TextureAsset::iconPath = QUrl("qrc:/icon/ICON_FILE_IMAGE.png");
const QString TextureAsset::typeString = "Image file";
const TextureAsset::LoaderMap TextureAsset::loaders = TextureAsset::createLoaders();

std::map<std::string, BaseAssetLoader*> TextureAsset::createLoaders()
{
    TextureAsset::LoaderMap m;
    m["tex"] = new TextureAssetLoader<BaseObject>();
    m["svg"] = new TextureAssetLoader<BaseObject>();
    m["bmp"] = new TextureAssetLoader<BaseObject>();
    m["tif"] = new TextureAssetLoader<BaseObject>();
    m["png"] = new TextureAssetLoader<BaseObject>();
    m["jpg"] = new TextureAssetLoader<BaseObject>();
    return m;
}

TextureAsset::TextureAsset(std::string path, std::string extension)
    : Asset(path, extension)
{
}

SofaComponent* TextureAsset::getPreviewNode()
{
    if (loaders.find(m_extension) == loaders.end())
    {
        msg_error("Unknown file format.");
        return new SofaComponent(nullptr, this);
    }

    this->setName("root");
    OglModel::SPtr vmodel = sofa::core::objectmodel::New<OglModel>();
    vmodel->setFilename("mesh/cube.obj");
    vmodel->setName("vmodel");
    vmodel->texturename.setValue(m_path);

    this->addObject(vmodel);
    this->init(sofa::core::ExecParams::defaultInstance());
    return new SofaComponent(nullptr, this);
}

} // namespace qtquick
} // namespace sofa
