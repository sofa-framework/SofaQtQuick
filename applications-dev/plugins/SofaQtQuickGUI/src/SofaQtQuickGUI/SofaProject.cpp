#include "SofaProject.h"
#include "SofaApplication.h"
#include <QWindow>

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

AssetFactory* SofaProject::getAssetFactory() { return &m_assetFactory; }

void SofaProject::scanProject(const QUrl& folder)
{
    _scanProject(QDir(folder.path()));
}

void SofaProject::scanProject(const QDir& folder)
{
    _scanProject(folder);
}

void SofaProject::_scanProject(const QDir& folder)
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
            {
                continue;
            }
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


sofa::qtquick::SofaComponent* SofaProject::getAsset(const QUrl& url, const QString& assetName)
{
    assetMapIterator it = m_assets.find(url.toLocalFile());
    if (it == m_assets.end())
    {
        std::cout << ("No such asset in project! rescan required") << std::endl;
        return nullptr;
    }
    if (!it->second.get())
    {
        std::cout << ("Could not load that asset properly... sorry!") << std::endl;
        return nullptr;
    }
    SofaComponent* component = it->second->getAsset(assetName.toStdString());
    if (component == nullptr)
    {
        std::cout << ("Could not load that asset's preview... sorry!") << std::endl;
        return nullptr;
    }

    return component;
}

QList<QObject*> SofaProject::getAssetMetaInfo(const QUrl& url)
{
    assetMapIterator it = m_assets.find(url.toLocalFile());
    if (it == m_assets.end())
    {
        std::cout << ("No such asset in project! rescan required") << std::endl;
        return QList<QObject*>();
    }
    if (!it->second.get())
    {
        std::cout << ("Could not load that asset properly... sorry!") << std::endl;
        return QList<QObject*>();
    }
    return it->second->getAssetMetaInfo();
}

}  // namespace qtquick
}  // namespace sofa
