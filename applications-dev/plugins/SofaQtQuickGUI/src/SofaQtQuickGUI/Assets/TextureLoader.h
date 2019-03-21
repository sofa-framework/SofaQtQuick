#pragma once

#include "AssetLoader.h"

namespace sofa
{
namespace qtquick
{

template<class T>
struct TextureLoaderCreator : public TBaseSofaLoaderCreator<T>
{
    core::objectmodel::BaseObject::SPtr createInstance() {
        return core::objectmodel::New<T>();
    }
};

class TextureLoader : public AssetLoader
{
public:
    TextureLoader(std::string path, std::string extension);

    static const QUrl iconPath;
};

} // namespace qtquick
} // namespace sofa
