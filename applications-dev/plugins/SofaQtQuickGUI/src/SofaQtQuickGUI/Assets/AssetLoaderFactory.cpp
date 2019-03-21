#include "AssetLoaderFactory.h"

namespace sofa {
namespace qtquick {

BaseAssetLoaderCreator::~BaseAssetLoaderCreator() {}

AssetLoaderFactory::AssetLoaderFactory(QObject *parent)
    : QObject (parent)
{
    m_loaders["stl"] = new AssetLoaderCreator<MeshLoader>();
    m_loaders["obj"] = new AssetLoaderCreator<MeshLoader>();
    m_loaders["vtk"] = new AssetLoaderCreator<MeshLoader>();
    m_loaders["gmsh"] = new AssetLoaderCreator<MeshLoader>();

    m_loaders["png"] = new AssetLoaderCreator<TextureLoader>();
    m_loaders["jpg"] = new AssetLoaderCreator<TextureLoader>();
    m_loaders["tif"] = new AssetLoaderCreator<TextureLoader>();
    m_loaders["bmp"] = new AssetLoaderCreator<TextureLoader>();
    m_loaders["svg"] = new AssetLoaderCreator<TextureLoader>();
    m_loaders["tex"] = new AssetLoaderCreator<TextureLoader>();

}

AssetLoaderFactory::~AssetLoaderFactory()
{
    for(auto loader : m_loaders)
        delete(loader.second);
    m_loaders.clear();
}

QUrl AssetLoaderFactory::getIcon(QString extension) const
{
    std::cout << extension.toStdString() << std::endl;
    const auto& loader = m_loaders.find(extension.toStdString());
    if (loader == m_loaders.end())
        return AssetLoader::iconPath;
    return loader->second->getIcon();
}

std::shared_ptr<AssetLoader> AssetLoaderFactory::createInstance(const QString& path, const QString& extension) const
{
    std::cout << extension.toStdString() << std::endl;
    const auto& loader = m_loaders.find(extension.toStdString());
    if (loader == m_loaders.end())
        return nullptr;
    return loader->second->createInstance(path.toStdString(), extension.toStdString());
}


} // namespace qtquick
} // namespace sofa
