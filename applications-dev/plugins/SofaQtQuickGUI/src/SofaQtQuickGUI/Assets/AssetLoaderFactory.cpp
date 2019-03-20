#include "AssetLoaderFactory.h"

namespace sofa {
namespace qtquick {

AssetLoaderFactory::AssetLoaderFactory()
{
    m_loaders[".stl"] = new MeshLoader();
    m_loaders[".obj"] = new MeshLoader();
    m_loaders[".vtk"] = new MeshLoader();
    m_loaders[".gmsh"] = new MeshLoader();
}

const QUrl& AssetLoaderFactory::getIcon(const QString& extension) const
{
    return m_loaders.find(extension.toStdString())->second->iconPath;
}

const AssetLoader* get(const QString& extension) const
{
    return m_loaders.find(extension.toStdString())->second;
}


} // namespace qtquick
} // namespace sofa
