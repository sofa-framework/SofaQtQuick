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
  public:
    SOFA_ABSTRACT_CLASS(Asset, sofa::simulation::graph::DAGNode);
    typedef std::map<std::string, BaseAssetLoader *> LoaderMap;

    Asset(std::string path, std::string extension);
    void initAsset();
    virtual ~Asset() override;
    virtual sofa::qtquick::SofaComponent* getAsset(const std::string& assetName = "") = 0;
    virtual QList<QObject*> getAssetMetaInfo() = 0;
    static const QString typeString;
    static const QUrl iconPath;

    static const LoaderMap loaders;

  protected:
    const std::string m_path;
    const std::string m_extension;
    QList<QObject*> m_metaDataModel;
};

struct BaseAssetLoader
{
    virtual ~BaseAssetLoader();
    virtual BaseObject::SPtr New() = 0;
    virtual const std::string &getClassName() = 0;
};

template <class T> struct TBaseAssetLoader : public BaseAssetLoader
{
    virtual ~TBaseAssetLoader();
    virtual const std::string &getClassName() override;
    virtual BaseObject::SPtr New() override;
};

template <class T> TBaseAssetLoader<T>::~TBaseAssetLoader() {}

template <class T> const std::string &TBaseAssetLoader<T>::getClassName()
{
    return T::GetClass()->className;
}
template <class T> BaseObject::SPtr TBaseAssetLoader<T>::New()
{
    return sofa::core::objectmodel::New<T>();
}

} // namespace qtquick
} // namespace sofa
