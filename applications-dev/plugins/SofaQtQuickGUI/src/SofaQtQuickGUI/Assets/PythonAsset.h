#pragma once

#include "Asset.h"
#include <SofaPython/Binding.h>

namespace sofa::qtquick
{

template <class T>
struct PythonAssetLoader : public TBaseAssetLoader<T>
{
};

class PythonAssetModel : public QObject
{
    Q_OBJECT

public:
    PythonAssetModel(std::string name, std::string type);

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)

private:
    Q_SIGNAL void nameChanged(QString);
    QString name() const;
    const std::string m_name;

    Q_SIGNAL void typeChanged(QString);
    QString type() const;
    const std::string m_type;
};

class PythonAsset : public Asset
{
  public:
    PythonAsset(std::string path, std::string extension);
    virtual sofaqtquick::bindings::SofaNode* getAsset(const std::string& assetName = "") override;
    virtual QList<QObject*> getAssetMetaInfo() override;

    static const QUrl iconPath;
    static const QString typeString;
    static const LoaderMap loaders;

    static LoaderMap createLoaders();
};

} // namespace sofa::qtquick

