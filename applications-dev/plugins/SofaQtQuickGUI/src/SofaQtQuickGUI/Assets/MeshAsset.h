#pragma once

#include "Asset.h"
#include <SofaGeneralLoader/MeshGmshLoader.h>
#include <SofaGeneralLoader/MeshSTLLoader.h>
#include <SofaGeneralLoader/MeshXspLoader.h>
#include <SofaLoader/MeshObjLoader.h>
#include <SofaLoader/MeshVTKLoader.h>
#include <sofa/core/loader/MeshLoader.h>
using namespace sofa::component::loader;

namespace sofa
{
namespace qtquick
{

template <class T>
struct MeshAssetLoader : public TBaseAssetLoader<T>
{
};


class MeshAsset : public Asset
{
  public:
    MeshAsset(std::string path, std::string extension);
    virtual SofaComponent* getAsset(const std::string& assetName = "") override;
    virtual QList<QObject*> getAssetMetaInfo() override;

//    Q_PROPERTY(QString bbox READ bbox NOTIFY bboxChanged)
//    Q_SIGNAL void bboxChanged(QString bbox);
//    SofaData* bbox();


    static const QUrl iconPath;
    static const QString typeString;
    static const LoaderMap loaders;

    DAGNode::SPtr m_node;
    static LoaderMap createLoaders();
};

} // namespace qtquick
} // namespace sofa
