#include "AssetFactory.h"

#include "MeshAsset.h"
#include "TextureAsset.h"
#include "PythonAsset.h"

namespace sofa
{
namespace qtquick
{

BaseAssetCreator::~BaseAssetCreator() {}

void AssetFactory::clearLoaders()
{
    for (const auto& creator : getFactoryCreators())
        delete creator.second;
    getFactoryCreators().clear();
}

QStringList AssetFactory::getSupportedTypes()
{
    QStringList keys;
    for (const auto& [k, v] : getFactoryCreators())
    {
        SOFA_UNUSED(v);
        keys.push_back(QString("*.") + k.c_str());
    }
    return keys;
}

std::shared_ptr<Asset>
AssetFactory::createInstance(const QString& path,
                             const QString& extension)
{
    const auto& creator = getFactoryCreators().find(extension.toStdString());
    if (creator == getFactoryCreators().end())
        return nullptr; // TODO:@marques-bruno Instead of discarding unknown extensions, load custom templates from a user defined python templates dir
    return creator->second->createInstance(path.toStdString(),
                                          extension.toStdString());
}

bool AssetFactory::registerAsset(const std::string& extension,
                                 BaseAssetCreator* creator)
{
    if (getFactoryCreators().find(extension) == getFactoryCreators().end())
        delete getFactoryCreators()[extension];
    getFactoryCreators()[extension] = creator;
    return true;
}

} // namespace qtquick
} // namespace sofa
