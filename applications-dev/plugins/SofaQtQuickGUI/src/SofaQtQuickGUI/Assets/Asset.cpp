#include "Asset.h"

namespace sofa
{
namespace qtquick
{

const QUrl Asset::iconPath = QUrl("qrc:/icon/ICON_FILE_BLANK");
const QString Asset::typeString = "Unknown file format";
const Asset::LoaderMap Asset::loaders = Asset::LoaderMap();

BaseAssetLoader::~BaseAssetLoader() {}

Asset::Asset(std::string path, std::string extension)
    : DAGNode("NewAsset", nullptr),
      m_path(path), m_extension(extension)
{
}

void Asset::initAsset()
{
    m_metaDataModel = getAssetMetaInfo();
}

Asset::~Asset()
{
    for (auto data : m_metaDataModel)
        delete data;
}

} // namespace qtquick
} // namespace sofa
