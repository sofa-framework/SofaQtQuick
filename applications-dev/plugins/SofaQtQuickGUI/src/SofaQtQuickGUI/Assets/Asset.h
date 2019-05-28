#pragma once

#include "SofaQtQuickGUI/Bindings/SofaNode.h"
#include <SofaSimulationGraph/DAGNode.h>
#include <sofa/core/objectmodel/BaseObject.h>

using sofa::core::objectmodel::BaseObject;

#include <QUrl>

namespace sofa
{
namespace qtquick
{

struct BaseAssetLoader;

/**
 *  \brief Base class for Assets loaded through the AssetFactory
 *
 *  - Can query meta data on its templated type
 *  - Instantiates a SofaScene template given a asset file
 */
class Asset : public QObject
{
    Q_OBJECT
  public:

    Asset(std::string path, std::string extension);
    virtual ~Asset();


    Q_INVOKABLE virtual void getDetails() = 0; // Loads the asset
    Q_INVOKABLE virtual sofaqtquick::bindings::SofaNode* getAsset(const std::string& assetName = "") = 0; // instantiates the asset

    Q_PROPERTY(QString typeString READ getTypeString NOTIFY typeStringChanged)
    Q_PROPERTY(QUrl iconPath READ getIconPath NOTIFY iconPathChanged)
    Q_PROPERTY(bool isSofaContent READ getIsSofaContent NOTIFY isSofaContentChanged)
    Q_PROPERTY(QString path READ path NOTIFY pathChanged)

  protected:
    Q_INVOKABLE virtual QString getTypeString() { return "Unknown file format"; }
    Q_INVOKABLE virtual QUrl getIconPath() { return QUrl("qrc:/icon/ICON_FILE_BLANK"); }
    Q_INVOKABLE virtual bool getIsSofaContent() { return false; }

    Q_INVOKABLE QString path() { return QString(m_path.c_str()); }

    Q_SIGNAL void typeStringChanged(const QUrl& type);
    Q_SIGNAL void iconPathChanged(const QUrl& path);
    Q_SIGNAL void pathChanged(const QString& type);
    Q_SIGNAL void isSofaContentChanged(bool val);


    typedef std::map<std::string, BaseAssetLoader *> LoaderMap;
    static const LoaderMap _loaders;

    const std::string m_path;
    const std::string m_extension;
};

struct BaseAssetLoader
{
    virtual ~BaseAssetLoader();
    virtual BaseObject::SPtr New() = 0;
};

template <class T> struct TBaseAssetLoader : public BaseAssetLoader
{
    virtual ~TBaseAssetLoader();
    virtual BaseObject::SPtr New() override;
};

template <class T> TBaseAssetLoader<T>::~TBaseAssetLoader() {}


template <class T> BaseObject::SPtr TBaseAssetLoader<T>::New()
{
    return sofa::core::objectmodel::New<T>();
}

} // namespace qtquick
} // namespace sofa
