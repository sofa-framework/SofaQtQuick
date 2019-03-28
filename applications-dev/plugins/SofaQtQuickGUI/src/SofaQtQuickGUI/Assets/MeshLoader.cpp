#include "MeshLoader.h"

namespace sofa
{
namespace qtquick
{

const QUrl MeshLoader::iconPath = QUrl("qrc:/icon/ICON_MESH.png");
const QString MeshLoader::typeString = "3D mesh file";

MeshLoader::MeshLoader(std::string path, std::string extension)
    : AssetLoader(path, extension)
{
    m_loaders["stl"] = new MeshLoaderCreator<MeshSTLLoader>(path);
    m_loaders["obj"] = new MeshLoaderCreator<MeshObjLoader>(path);
    m_loaders["vtk"] = new MeshLoaderCreator<MeshVTKLoader>(path);
    m_loaders["gmsh"] = new MeshLoaderCreator<MeshGmshLoader>(path);
}

} // namespace qtquick
} // namespace sofa
