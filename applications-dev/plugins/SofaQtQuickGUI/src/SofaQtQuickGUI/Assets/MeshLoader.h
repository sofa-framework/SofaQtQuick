#pragma once

#include "AssetLoader.h"
#include <sofa/core/loader/MeshLoader.h>

namespace sofa {
namespace qtquick {

struct BaseMeshLoaderCreator
{
    virtual ~BaseMeshLoaderCreator() {}
    virtual core::objectmodel::BaseObject::SPtr createInstance() = 0;
};

template<class T>
struct MeshLoaderCreator : public BaseMeshLoaderCreator
{
    core::objectmodel::BaseObject::SPtr createInstance() {
        return core::objectmodel::New<T>();
    }
};



class MeshLoader : public AssetLoader
{
public:
    MeshLoader();

    static const QUrl iconPath;
    core::objectmodel::BaseObject::SPtr createAssetLoader(std::string extension);

private:
    std::map<std::string, BaseMeshLoaderCreator*> m_loaders;
};

} // namespace qtquick
} // namespace sofa
