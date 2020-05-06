#pragma once

#include "Asset.h"

namespace sofaqtquick
{

template <class T>
struct TextureAssetLoader : public TBaseAssetLoader<T>{};

class TextureAsset : public Asset
{
  public:
    TextureAsset(std::string path, std::string extension);
    virtual sofaqtquick::bindings::SofaNode* create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName = "") override;
    virtual void getDetails() override;

    static LoaderMap createLoaders();
protected:
    Q_INVOKABLE virtual QString getTypeString() override { return "Image file"; }
    Q_INVOKABLE virtual QUrl getIconPath() override { return QUrl("qrc:/icon/ICON_FILE_IMAGE.png"); }
    Q_INVOKABLE virtual bool getIsSofaContent() override { return true; }

    static const LoaderMap _loaders;
};

} // namespace sofaqtquick
