#pragma once

#include <QFileInfoList>
#include "Asset.h"

namespace sofa
{
namespace qtquick
{

class FileAsset : public Asset
{
    Q_OBJECT
public:
    void getDetails() override ;

    FileAsset(const std::string& path, const std::string& extension);
    Q_INVOKABLE sofaqtquick::bindings::SofaNode* create(sofaqtquick::bindings::SofaNode* parent,
                                                        const QString& assetName = "") override;

};

} // namespace qtquick
} // namespace sofa
