#pragma once

#include "AssetLoader.h"

namespace sofa
{
namespace qtquick
{

template<class T>
struct TextureLoaderCreator : public TBaseSofaLoaderCreator<T>
{
    virtual SofaComponent* createInstance() {
        return nullptr;
    }
};

class TextureLoader : public AssetLoader
{
public:
    TextureLoader(std::string path, std::string extension);

    static const QUrl iconPath;
    static const QString typeString;
};

} // namespace qtquick
} // namespace sofa
