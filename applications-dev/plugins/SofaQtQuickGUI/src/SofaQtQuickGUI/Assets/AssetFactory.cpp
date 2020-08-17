#include "AssetFactory.h"

#include "FileAsset.h"
//#include "MeshAsset.h"
#include "TextureAsset.h"
#include "TemplateAsset.h"
#include "PythonAsset.h"
#include <QQmlEngine>

namespace sofaqtquick
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

QSharedPointer<Asset>
AssetFactory::createInstance(const QString& path,
                             const QString& extension)
{
    const auto& creator = getFactoryCreators().find(extension.toStdString());
    if (creator == getFactoryCreators().end())
        return QSharedPointer<Asset>{new TemplateAsset(path.toStdString(),
                                          extension.toStdString())};
    auto ptr = creator->second->createInstance(path.toStdString(),
                                          extension.toStdString());
    return QSharedPointer<Asset>(ptr);
}

bool AssetFactory::registerAsset(const std::string& extension,
                                 BaseAssetCreator* creator)
{
    if (getFactoryCreators().find(extension) == getFactoryCreators().end())
        delete getFactoryCreators()[extension];
    getFactoryCreators()[extension] = creator;
    return true;
}

} // namespace sofaqtquick

