#include "SofaProject.h"

namespace sofa {
namespace qtquick {

SofaProject::SofaProject()
{

}
SofaProject::~SofaProject()
{

}
const QUrl& SofaProject::getRootDir()
{
    return m_rootDir;

}

void SofaProject::setRootDir(const QUrl& rootDir)
{
    m_rootDir = rootDir;
}

}  // namespace qtquick
}  // namespace sofa
