#include <QDir>
#include "DirectoryAsset.h"

namespace sofa
{
namespace qtquick
{

DirectoryAsset::DirectoryAsset(QString fullpath) : m_path(fullpath)
{

}

void DirectoryAsset::getDetails()
{
    QDir dir {m_path} ;
    content = dir.entryInfoList();
}

} // namespace qtquick
} // namespace sofa

