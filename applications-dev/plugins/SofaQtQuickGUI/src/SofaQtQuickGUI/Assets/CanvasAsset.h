#pragma once

#include "Asset.h"

namespace sofaqtquick
{

template <class T>
struct CanvasAssetLoader : public sofaqtquick::TBaseAssetLoader<T>{};

class CanvasAsset : public sofaqtquick::Asset
{
public:
    CanvasAsset(std::string path, std::string extension);
    virtual sofaqtquick::bindings::SofaNode* create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName = "") override;
    virtual void getDetails() override;

    static LoaderMap createLoaders();

    Q_INVOKABLE virtual QUrl getIconPath() override { return QUrl("qrc:/icon/ICON_CANVAS2.png"); }
    Q_INVOKABLE virtual QString getTypeString() override { return "Qml UI Canvas"; }
    Q_INVOKABLE virtual bool getIsSofaContent() override { return true; }

protected:
    static const LoaderMap _loaders;
};

}
