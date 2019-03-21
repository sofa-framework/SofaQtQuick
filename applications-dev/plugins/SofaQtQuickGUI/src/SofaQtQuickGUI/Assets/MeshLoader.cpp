#include "MeshLoader.h"

#include <SofaLoader/MeshObjLoader.h>
#include <SofaLoader/MeshVTKLoader.h>
#include <SofaGeneralLoader/MeshSTLLoader.h>
#include <SofaGeneralLoader/MeshGmshLoader.h>
#include <SofaGeneralLoader/MeshXspLoader.h>
using namespace sofa::component::loader;

namespace sofa
{
namespace qtquick
{

const QUrl MeshLoader::iconPath = QUrl("qrc:/icon/ICON_MESH.png");

MeshLoader::MeshLoader(std::string path, std::string extension)
    : AssetLoader(path, extension)
{
    m_loaders["stl"] = new MeshLoaderCreator<MeshSTLLoader>();
    m_loaders["obj"] = new MeshLoaderCreator<MeshObjLoader>();
    m_loaders["vtk"] = new MeshLoaderCreator<MeshVTKLoader>();
    m_loaders["gmsh"] = new MeshLoaderCreator<MeshGmshLoader>();
}

} // namespace qtquick
} // namespace sofa
