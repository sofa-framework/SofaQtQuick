#pragma once

#include <QObject>
#include <QDir>
#include <QMap>
#include <QUrl>
#include <QFileSystemWatcher>

#include "SofaQtQuickGUI/Assets/AssetFactory.h"
#include "SofaQtQuickGUI/Assets/DirectoryAsset.h"
#include "SofaQtQuickGUI/config.h"
#include "SofaQtQuickGUI/Bindings/SofaBase.h"
using sofaqtquick::bindings::SofaBase;

#include "LiveFileMonitor.h"

namespace sofa {
namespace qtquick {

class ProjectMonitor : public QObject, public FileEventListener
{
    Q_OBJECT
public:

    QFileSystemWatcher m_dirwatcher;

    ProjectMonitor();

    void addDirectory(const QString& filepath);
    void addFile(const QString& filepath);
    void addPath(const QString& path);
    void fileHasChanged(const std::string& filename);
    void removePath(const QString& path);
signals:
    void directoryChanged(const QString& filename);
    void fileChanged(const QString& filename);
};

/**
 *  \brief Holds the current project's properties
 *
 *   A SOFA project is defined by a directory, containing scene files, assets, plugins etc, and respecting a specific directory tree
 */

class SOFA_SOFAQTQUICKGUI_API SofaProject : public QObject
{
    Q_OBJECT
public:
    SofaProject();
    ~SofaProject();

    Q_PROPERTY(QUrl rootDir READ getRootDir WRITE setRootDir NOTIFY rootDirChanged)
    const QUrl& getRootDir();
    void setRootDir(const QUrl& rootDir);
    Q_SIGNAL void rootDirChanged(QUrl& rootDir);


    Q_INVOKABLE QString createProject(const QUrl& dir);
    Q_INVOKABLE bool createProjectTree(const QUrl& dir);
    Q_INVOKABLE QString importProject(const QUrl& archive);
    Q_INVOKABLE bool exportProject(const QUrl& projectName);

    Q_INVOKABLE void scanProject(const QFileInfo& folder);
    Q_INVOKABLE void scanProject(const QUrl& url);

    Q_INVOKABLE const QString getFileCount(const QUrl& url);

    Q_INVOKABLE Asset* getAsset(const QString& filePath);
    Q_INVOKABLE QStringList getSupportedTypes() const;

    Q_INVOKABLE bool createPrefab(SofaBase* node);

    void removeDirEntries(DirectoryAsset& folder);
    void updateDirectory(const QFileInfo& finfo);
    void updateAsset(const QFileInfo& path);

private slots:
    void onDirectoryChanged(const QString &path);
    void onFileChanged(const QString &path);

signals:
    void filesChanged();

private:
    QUrl m_rootDir; // The Project's root fs directory

    QMap<QString, std::shared_ptr<Asset> > m_assets; /// project asset's URLs with their associated loaders
    QMap<QString, std::shared_ptr<DirectoryAsset> > m_directories;

    void _scanProject(const QFileInfo &file);

    ProjectMonitor* m_watcher;
};

}  // namespace qtquick
}  // namespace sofa


