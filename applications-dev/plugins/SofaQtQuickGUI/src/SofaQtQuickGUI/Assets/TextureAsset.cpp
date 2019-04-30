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

sofaqtquick::bindings::SofaNode* TextureAsset::getAsset(const std::string& assetName)
{
    SOFA_UNUSED(assetName);
    if (loaders.find(m_extension) == loaders.end())
    {
        msg_error("Unknown file format.");
        return new sofaqtquick::bindings::SofaNode(this);
    }
    Node::SPtr root;
    root->setName("root");
    OglModel::SPtr vmodel = sofa::core::objectmodel::New<OglModel>();
    vmodel->setFilename("mesh/cube.obj");
    vmodel->setName("vmodel");
    vmodel->texturename.setValue(m_path);

    root->addObject(vmodel);
    root->init(sofa::core::ExecParams::defaultInstance());
    DAGNode::SPtr node = DAGNode::SPtr(dynamic_cast<DAGNode*>(root.get()));
    return new sofaqtquick::bindings::SofaNode(node, dynamic_cast<QObject*>(this));
}

QList<QObject*> TextureAsset::getAssetMetaInfo()
{
    return QList<QObject*>();
}

} // namespace qtquick
} // namespace sofa
