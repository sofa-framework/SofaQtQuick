#pragma once

#include <QFileInfoList>
#include "Asset.h"

namespace sofa
{
namespace qtquick
{

class DirectoryAsset
{
public:
    void getDetails() ;

    DirectoryAsset(QString fullpath);
    QFileInfoList content;
    QString m_path;
};

} // namespace qtquick
} // namespace sofa
