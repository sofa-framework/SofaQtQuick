#include "AssetFactory.h"

namespace sofa
{
namespace qtquick
{

BaseAssetCreator::~BaseAssetCreator() {}

AssetFactory::AssetFactory(QObject* parent) : QObject(parent)
{
    m_loaders["stl"] = new AssetCreator<MeshAsset>();
    m_loaders["obj"] = new AssetCreator<MeshAsset>();
    m_loaders["vtk"] = new AssetCreator<MeshAsset>();
    m_loaders["gmsh"] = new AssetCreator<MeshAsset>();

    m_loaders["png"] = new AssetCreator<TextureAsset>();
    m_loaders["jpg"] = new AssetCreator<TextureAsset>();
    m_loaders["tif"] = new AssetCreator<TextureAsset>();
    m_loaders["bmp"] = new AssetCreator<TextureAsset>();
    m_loaders["svg"] = new AssetCreator<TextureAsset>();
    m_loaders["tex"] = new AssetCreator<TextureAsset>();
    m_loaders["ico"] = new AssetCreator<TextureAsset>();
}

AssetFactory::~AssetFactory()
{
    for (auto loader : m_loaders)
        delete (loader.second);
    m_loaders.clear();
}

QUrl AssetFactory::getIcon(QString extension) const
{
    const auto& loader = m_loaders.find(extension.toStdString());
    if (loader == m_loaders.end())
        return Asset::iconPath;
    return loader->second->getIcon();
}
QString AssetFactory::getTypeString(QString extension) const
{
    const auto& loader = m_loaders.find(extension.toStdString());
    if (loader == m_loaders.end())
        return Asset::typeString;
    return loader->second->getTypeString();
}

std::shared_ptr<Asset>
AssetFactory::createInstance(const QString& path,
                             const QString& extension) const
{
    const auto& loader = m_loaders.find(extension.toStdString());
    if (loader == m_loaders.end())
        return nullptr;
    return loader->second->createInstance(path.toStdString(),
                                          extension.toStdString());
}

} // namespace qtquick
} // namespace sofa
