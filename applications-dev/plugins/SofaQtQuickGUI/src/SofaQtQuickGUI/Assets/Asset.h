#pragma once

#include <SofaSimulationGraph/DAGNode.h>
#include <sofa/core/objectmodel/BaseObject.h>

#include "SofaQtQuickGUI/Bindings/SofaNode.h"
using sofa::core::objectmodel::BaseObject;

#include <QDateTime>
#include <QUrl>

namespace sofaqtquick
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
    Q_INVOKABLE virtual sofaqtquick::bindings::SofaNode* create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName = "") = 0; // instantiates the asset
    Q_INVOKABLE virtual QUrl getAssetInspectorWidget();
    Q_INVOKABLE virtual void openThirdParty(); // Opens a 3rd party program to edit / visualize the asset


    Q_PROPERTY(QString typeString READ getTypeString NOTIFY typeStringChanged)
    Q_PROPERTY(QUrl iconPath READ getIconPath NOTIFY iconPathChanged)
    Q_PROPERTY(bool isSofaContent READ getIsSofaContent NOTIFY isSofaContentChanged)
    Q_PROPERTY(QString path READ path NOTIFY pathChanged)
    Q_PROPERTY(bool isScene READ isScene NOTIFY isSceneChanged)
    Q_PROPERTY(QString extension READ extension NOTIFY extensionChanged)

    QDateTime getLastModified() { return m_lastModified; }
    void setLastModified(const QDateTime& t) { m_lastModified = t; }

    virtual bool isScene() { return false; }
    Q_INVOKABLE virtual QString getTypeString() { return "Unknown file format"; }
    Q_INVOKABLE virtual QUrl getIconPath() { return QUrl("qrc:/icon/ICON_FILE_BLANK"); }
    Q_INVOKABLE virtual bool getIsSofaContent() { return false; }
    Q_INVOKABLE QString path() { return QString(m_path.c_str()); }
    Q_INVOKABLE QString extension() { return QString(m_extension.c_str()); }

protected:

    Q_SIGNAL void typeStringChanged(const QUrl& type);
    Q_SIGNAL void iconPathChanged(const QUrl& path);
    Q_SIGNAL void pathChanged(const QString& type);
    Q_SIGNAL void isSofaContentChanged(bool val);
    Q_SIGNAL void isSceneChanged(bool);
    Q_SIGNAL void extensionChanged(const QString& ext);


    typedef std::map<std::string, BaseAssetLoader *> LoaderMap;
    static const LoaderMap _loaders;

    QDateTime m_lastModified;
    const std::string m_path;
    const std::string m_extension;
    bool m_detailsLoaded {false};
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

} // namespace sofaqtquick
