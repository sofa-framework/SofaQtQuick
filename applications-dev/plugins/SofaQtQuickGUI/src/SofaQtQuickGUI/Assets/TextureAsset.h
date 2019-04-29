#pragma once

#include "Asset.h"

namespace sofa
{
namespace qtquick
{

template <class T>
struct TextureAssetLoader : public TBaseAssetLoader<T>
{
    BaseObject::SPtr New() override { return BaseObject::SPtr(sofa::core::objectmodel::New<T>().get()); }
};

class TextureAsset : public Asset
{
  public:
    TextureAsset(std::string path, std::string extension);
    virtual SofaComponent* getAsset(const std::string& assetName = "") override;
    virtual QList<QObject*> getAssetMetaInfo() override;

    static const QUrl iconPath;
    static const QString typeString;
    static const LoaderMap loaders;

    static LoaderMap createLoaders();
};

} // namespace qtquick
} // namespace sofa
