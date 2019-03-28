#include "SofaProject.h"

namespace sofa {
namespace qtquick {

SofaProject::SofaProject() {}
SofaProject::~SofaProject() {}

const QUrl& SofaProject::getRootDir() { return m_rootDir;  }

void SofaProject::setRootDir(const QUrl& rootDir)
{
    m_rootDir = rootDir;
    msg_info("SofaProject") << "Setting root directory to : " << rootDir.toString().toStdString();

    QDir root = QDir(m_rootDir.path());
    msg_info("SofaProject") << "scanning project directory...";
    scanProject(root);
}

AssetLoaderFactory* SofaProject::getAssetFactory() { return &m_assetFactory; }

void SofaProject::scanProject(const QUrl& folder)
{
    scanProject(QDir(folder.path()));
}

void SofaProject::scanProject(const QDir& folder)
{
    if (!folder.exists())
        return;

    QStringList content = folder.entryList();
    QFileInfoList contentInfo = folder.entryInfoList();
    for (int idx = 0 ; idx < content.size() ; ++idx)
    {
        QFileInfo f = contentInfo[idx];
        if (f.isDir())
        {
            if (f.fileName() == "." || f.fileName() == "..")
                continue;
            scanProject(QDir(f.filePath()));
        }
        else
        {
            m_assets.insert(assetMapPair(f.filePath(), m_assetFactory.createInstance(f.filePath(), f.suffix())));
        }
    }

}

const QString SofaProject::getFileCount(const QUrl& url)
{
    QDir dir(url.path());
    if (!dir.exists())
        return "Corrupted Folder";
    std::string str = std::to_string(dir.count());
    str += " items";
    return QString(str.c_str());
}

}  // namespace qtquick
}  // namespace sofa
