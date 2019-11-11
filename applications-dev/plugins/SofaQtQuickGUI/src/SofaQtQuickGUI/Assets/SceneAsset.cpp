#include "SceneAsset.h"

#include <sofa/helper/cast.h>

#include "AssetFactory.h"
#include "AssetFactory.inl"

namespace sofaqtquick
{

/// Register all mesh assets extensions to the factory
bool __scn   = AssetFactory::registerAsset("scn", new AssetCreator<SceneAsset>());
bool __pyscn = AssetFactory::registerAsset("pyscn", new AssetCreator<SceneAsset>());

SceneAsset::SceneAsset(std::string path, std::string extension)
    : Asset(path, extension)
{
}

sofaqtquick::bindings::SofaNode* SceneAsset::create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName)
{
    SOFA_UNUSED(parent);
    SOFA_UNUSED(assetName);
    return nullptr;
}

void SceneAsset::getDetails()
{
    m_detailsLoaded = true;
}

QUrl SceneAsset::getAssetInspectorWidget() {
    return QUrl("");
}


} // namespace sofaqtquick
