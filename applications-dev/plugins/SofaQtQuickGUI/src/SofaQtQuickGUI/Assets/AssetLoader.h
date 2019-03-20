#ifndef ASSETLOADER_H
#define ASSETLOADER_H

#include <sofa/core/objectmodel/BaseObject.h>
using sofa::core::objectmodel::BaseObject;

#include <QUrl>

namespace sofa {
namespace qtquick {

class AssetLoader
{
public:
    AssetLoader();
    virtual ~AssetLoader();

    virtual BaseObject::SPtr createAssetLoader(std::string extension) = 0;

    static const QUrl iconPath;
};

} // namespace qtquick
} // namespace sofa

#endif // ASSETLOADER_H
