#include <QDir>
#include "FileAsset.h"

namespace sofa
{
namespace qtquick
{

FileAsset::FileAsset(const std::string& path, const std::string& extension) : Asset(path, extension)
{

}

void FileAsset::getDetails()
{
}

sofaqtquick::bindings::SofaNode* FileAsset::create(sofaqtquick::bindings::SofaNode* parent,
                                                   const QString& assetName)
{
    return nullptr;
}

} // namespace qtquick
} // namespace sofa

