#include <QDir>
#include "DirectoryAsset.h"

namespace sofaqtquick
{

DirectoryAsset::DirectoryAsset(QString fullpath) : m_path(fullpath)
{

}

void DirectoryAsset::getDetails()
{
    QDir dir {m_path} ;
    content = dir.entryInfoList();
}

} // namespace sofaqtquick

