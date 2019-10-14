#pragma once

#include "Asset.h"

namespace sofa
{
namespace qtquick
{

template <class T>
struct SceneAssetLoader : public TBaseAssetLoader<T>{};


class SceneAsset : public Asset
{
    Q_OBJECT
  public:
    SceneAsset(std::string path, std::string extension);
    virtual sofaqtquick::bindings::SofaNode* create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName = "") override;
    virtual void getDetails() override;
    virtual QUrl getAssetInspectorWidget() override;

protected:
    Q_INVOKABLE virtual QUrl getIconPath() override { return QUrl("qrc:/icon/ICON_PYSCN.png"); }
    Q_INVOKABLE virtual QString getTypeString() override { return "Sofa Scene"; }
    Q_INVOKABLE virtual bool getIsSofaContent() override { return true; }
    virtual bool isScene() override { return true; }
};

} // namespace qtquick
} // namespace sofa
