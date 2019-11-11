#pragma once

#include <QFileInfoList>
#include "Asset.h"

namespace sofaqtquick
{

class DirectoryAsset
{
public:
    void getDetails() ;

    DirectoryAsset(QString fullpath);
    QFileInfoList content;
    QString m_path;
};

} // namespace sofaqtquick
