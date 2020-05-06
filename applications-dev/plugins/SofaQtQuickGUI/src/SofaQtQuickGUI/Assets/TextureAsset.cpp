#include "TextureAsset.h"


#include <SofaOpenglVisual/OglModel.h>
using sofa::component::visualmodel::OglModel;

#include "AssetFactory.h"
#include "AssetFactory.inl"

namespace sofaqtquick
{

/// Register all python assets extensions to the factory
bool __tex = AssetFactory::registerAsset("tex", new AssetCreator<TextureAsset>());
bool __svg = AssetFactory::registerAsset("svg", new AssetCreator<TextureAsset>());
bool __bmp = AssetFactory::registerAsset("bmp", new AssetCreator<TextureAsset>());
bool __tif = AssetFactory::registerAsset("tif", new AssetCreator<TextureAsset>());
bool __png = AssetFactory::registerAsset("png", new AssetCreator<TextureAsset>());
bool __jpg = AssetFactory::registerAsset("jpg", new AssetCreator<TextureAsset>());

const TextureAsset::LoaderMap TextureAsset::_loaders = TextureAsset::createLoaders();

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

sofaqtquick::bindings::SofaNode* TextureAsset::create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName)
{
    SOFA_UNUSED(parent);
    SOFA_UNUSED(assetName);
//    if (_loaders.find(m_extension) == _loaders.end())
//    {
//        msg_error("Unknown file format.");
//        return nullptr;
//    }
//    sofa::simulation::Node::SPtr root;
//    root->setName("NEWTEXTURE");
//    OglModel::SPtr vmodel = sofa::core::objectmodel::New<OglModel>();
//    vmodel->setFilename("mesh/cube.obj");
//    vmodel->setName("vmodel");
//    vmodel->texturename.setValue(m_path);

//    root->addObject(vmodel);
//    root->init(sofa::core::ExecParams::defaultInstance());
//    sofa::simulation::graph::DAGNode::SPtr node = sofa::simulation::graph::DAGNode::SPtr(
//                dynamic_cast<sofa::simulation::graph::DAGNode*>(root.get()));
//    parent->self()->addChild(node);
//    return new sofaqtquick::bindings::SofaNode(node, dynamic_cast<QObject*>(this));
    return nullptr;
}

void TextureAsset::getDetails()
{
}

} // namespace sofaqtquick
