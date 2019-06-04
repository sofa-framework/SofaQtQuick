#include "Asset.h"


namespace sofa
{
namespace qtquick
{

const Asset::LoaderMap Asset::_loaders = Asset::LoaderMap();

BaseAssetLoader::~BaseAssetLoader() {}

Asset::Asset(std::string path, std::string extension)
    : m_path(path), m_extension(extension), m_detailsLoaded(false)
{
}

Asset::~Asset()
{
}

} // namespace qtquick
} // namespace sofa
