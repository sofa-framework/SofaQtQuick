#include "TextureLoader.h"

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

const QUrl TextureLoader::iconPath = QUrl("qrc:/icon/ICON_MESH.png");

TextureLoader::TextureLoader(std::string path, std::string extension)
    : AssetLoader(path, extension)
{
    m_loaders["tex"] = new TextureLoaderCreator<BaseObject>();
    m_loaders["svg"] = new TextureLoaderCreator<BaseObject>();
    m_loaders["bmp"] = new TextureLoaderCreator<BaseObject>();
    m_loaders["tif"] = new TextureLoaderCreator<BaseObject>();
    m_loaders["png"] = new TextureLoaderCreator<BaseObject>();
    m_loaders["jpg"] = new TextureLoaderCreator<BaseObject>();
}

} // namespace qtquick
} // namespace sofa
