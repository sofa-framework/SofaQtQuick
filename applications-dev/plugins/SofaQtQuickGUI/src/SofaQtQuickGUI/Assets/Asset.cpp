#include "Asset.h"
#include <SofaQtQuickGUI/SofaBaseApplication.h>


namespace sofaqtquick
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

QUrl Asset::getAssetInspectorWidget() {
    return QUrl("qrc:/SofaBasics/DynamicContent_Error.qml");
}

void Asset::openThirdParty() {
    SofaBaseApplication::Instance()->openInEditor(path());
}


} // namespace sofaqtquick
