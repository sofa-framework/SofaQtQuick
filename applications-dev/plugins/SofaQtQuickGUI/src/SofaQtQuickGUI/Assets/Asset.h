#pragma once

#include "SofaQtQuickGUI/Bindings/SofaComponent.h"
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
class Asset : public sofa::simulation::graph::DAGNode
{
//    Q_OBJECT

  public:
    typedef std::map<std::string, BaseAssetLoader *> LoaderMap;

    Asset(std::string path, std::string extension);
    virtual ~Asset();
    virtual SofaComponent* getPreviewNode() = 0;

//    Q_PROPERTY(QString fileName READ fileName NOTIFY fileNameChanged)
//    Q_SIGNAL void fileNameChanged(QString fileName);
//    QString fileName()
//    {
//        return QUrl(QString::fromStdString(m_path)).fileName();
//    }

    static const QUrl iconPath;
    static const QString typeString;
    static const LoaderMap loaders;

  protected:
    const std::string m_path;
    const std::string m_extension;
};

struct BaseAssetLoader
{
    virtual ~BaseAssetLoader();
    virtual BaseObject *New() = 0;
    virtual const std::string &getClassName() = 0;
};

template <class T> struct TBaseAssetLoader : public BaseAssetLoader
{
    virtual ~TBaseAssetLoader();
    virtual const std::string &getClassName() override;
};

template <class T> TBaseAssetLoader<T>::~TBaseAssetLoader() {}

template <class T> const std::string &TBaseAssetLoader<T>::getClassName()
{
    return T::GetClass()->className;
}

} // namespace qtquick
} // namespace sofa
