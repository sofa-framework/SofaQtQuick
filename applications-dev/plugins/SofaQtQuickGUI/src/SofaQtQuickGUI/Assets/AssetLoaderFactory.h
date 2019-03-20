#pragma once

#include "MeshLoader.h"
#include <iostream>
#include <map>

namespace sofa {
namespace qtquick {

class AssetLoaderFactory
{
public:
    AssetLoaderFactory();
    const QUrl& getIcon(const QString& extension) const;
private:
    std::map<std::string, AssetLoader*> m_loaders;
};

} // namespace qtquick
} // namespace sofa
