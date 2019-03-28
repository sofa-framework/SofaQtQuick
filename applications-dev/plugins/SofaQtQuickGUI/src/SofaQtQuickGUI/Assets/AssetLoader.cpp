#include "AssetLoader.h"

namespace sofa {
namespace qtquick {

const QUrl AssetLoader::iconPath = QUrl("qrc:/icon/ICON_FILE_BLANK");
const QString AssetLoader::typeString = "Unknown file format";

BaseSofaLoaderCreator::~BaseSofaLoaderCreator() {}

AssetLoader::AssetLoader(std::string path, std::string extension)
    :    m_path(path),
         m_extension(extension)
{

}
AssetLoader::~AssetLoader()
{

}

SofaComponent* AssetLoader::createSofaLoader()
{
    const auto& loader = m_loaders.find(m_extension);
    if (loader == m_loaders.end())
        return nullptr;
    if (loader->second->getClassName() == "BaseObject")
    {
        msg_error("Sofa AssetLoader") << "Loader not implemented yet";
        return nullptr;
    }
    return loader->second->createInstance();
}

} // namespace qtquick
} // namespace sofa
