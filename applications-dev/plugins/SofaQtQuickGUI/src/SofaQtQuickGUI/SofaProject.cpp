#include "SofaProject.h"
#include "SofaApplication.h"
#include <QWindow>
#include <QFileDialog>

namespace sofa {
namespace qtquick {

SofaProject::SofaProject() {}
SofaProject::~SofaProject() {}

const QUrl& SofaProject::getRootDir() { return m_rootDir;  }

void SofaProject::setRootDir(const QUrl& rootDir)
{
    m_rootDir = rootDir;
    if(rootDir.isEmpty())
    {
        msg_warning("SofaProject") << "Empty project directory...skipping." ;
        return;
    }

    msg_info("SofaProject") << "Setting root directory to '" << rootDir.toString().toStdString()<<"'";
    QDir root = QDir(m_rootDir.path());
    scanProject(root);
}

AssetFactory* SofaProject::getAssetFactory() { return &m_assetFactory; }

void SofaProject::scanProject(const QUrl& folder)
{
    _scanProject(QDir(folder.path()));
}

QString SofaProject::importProject(const QUrl& srcUrl)
{
    QString src = srcUrl.path();
    QFileInfo finfo(src);
    if (finfo.exists() && finfo.suffix() == "zip")
    {
        QFileDialog dialog(nullptr, tr("Choose Project Destination"), "~/Documents", tr("All folders (*)"));
        dialog.setFileMode(QFileDialog::Directory);
        dialog.setOption(QFileDialog::ShowDirsOnly);
        if (dialog.exec())
        {
            QList<QUrl> folders = dialog.selectedUrls();
            if (folders.empty())
                return "Error: no destination picked";
            QString dest = folders.first().path();
            QProcess process;
            process.start("/usr/bin/unzip", QStringList() << src << "-d" << dest);
            std::cout << "/usr/bin/unzip " << src.toStdString() << " -d " << dest.toStdString() << std::endl;
            process.waitForFinished(-1);
            return dest + "/" + finfo.baseName();
        }
        return "Error: could not open Dialog";
    }
    return "Error: does not exist or isn't zip file";
}

bool SofaProject::exportProject(const QUrl& srcUrl)
{
    QString src = srcUrl.toLocalFile();
    QFileDialog dialog(nullptr, tr("Choose Project Destination"), "~/Documents");
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    if (dialog.exec())
    {
        QList<QUrl> folders = dialog.selectedUrls();
        if (folders.empty())
            return false;
        QString dest = folders.first().path();
        QProcess process;

        QString filePath = QFileInfo(src).filePath();
        QString baseName = QFileInfo(src).baseName();
        QString fileName = QFileInfo(src).fileName();
        process.start("/bin/ln", QStringList() << "-s" << filePath << fileName);
        process.waitForFinished(-1);
        std::cout << "/bin/ln" << " -s " << filePath.toStdString() << " " << fileName.toStdString() << std::endl;
        process.start("/usr/bin/zip", QStringList() << "-r" << QString(dest + "/" + baseName + ".zip") << fileName);
        process.waitForFinished(-1);
        std::cout << "/usr/bin/zip -r " << QString(dest + "/" + baseName + ".zip").toStdString() << " " << fileName.toStdString() << std::endl;
        process.start("/bin/rm", QStringList() << fileName);
        process.waitForFinished(-1);
        std::cout << "/bin/rm " << fileName.toStdString() << std::endl;
        return true;
    }
    return false;
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


sofaqtquick::bindings::SofaNode* SofaProject::getAsset(const QUrl& url, const QString& assetName)
{
    assetMapIterator it = m_assets.find(url.toLocalFile());
    if (it == m_assets.end())
    {
        return nullptr;
    }
    if (!it->second.get())
    {
        return nullptr;
    }
    sofaqtquick::bindings::SofaNode* component = it->second->getAsset(assetName.toStdString());
    if (component == nullptr)
    {
        return nullptr;
    }

    return component;
}

QList<QObject*> SofaProject::getAssetMetaInfo(const QUrl& url)
{
    assetMapIterator it = m_assets.find(url.toLocalFile());
    if (it == m_assets.end())
    {
        return QList<QObject*>();
    }
    if (!it->second.get())
    {
        return QList<QObject*>();
    }
    return it->second->getAssetMetaInfo();
}

}  // namespace qtquick
}  // namespace sofa
