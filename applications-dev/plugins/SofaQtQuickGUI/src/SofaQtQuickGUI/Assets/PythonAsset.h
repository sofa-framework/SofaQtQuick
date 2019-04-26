#pragma once

#include "Asset.h"
#include <SofaPython/Binding.h>

namespace sofa::qtquick
{

template <class T>
struct PythonAssetLoader : public TBaseAssetLoader<T>
{
};


class PythonAsset : public Asset
{
  public:
    PythonAsset(std::string path, std::string extension);
    virtual SofaComponent* getAsset() override;
    virtual void getAssetMetaInfo() override;

    static const QUrl iconPath;
    static const QString typeString;
    static const LoaderMap loaders;

    DAGNode::SPtr m_node;
    static LoaderMap createLoaders();
};

} // namespace sofa::qtquick

