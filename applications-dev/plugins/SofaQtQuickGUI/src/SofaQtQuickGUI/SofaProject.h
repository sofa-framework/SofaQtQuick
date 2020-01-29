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


namespace sofaqtquick
{

using sofaqtquick::bindings::SofaNode;

class ProjectMonitor : public QObject, public FileEventListener
{
    Q_OBJECT
public:

    QFileSystemWatcher m_dirwatcher;

    ProjectMonitor();

public slots:
    void update();

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
    Q_PROPERTY(QUrl rootDir READ getRootDir WRITE setRootDir NOTIFY rootDirChanged)
    Q_PROPERTY(bool isDebugPrintEnabled READ getDebug WRITE setDebug NOTIFY debugChanged)

public:
    SofaProject();
    ~SofaProject();

    const QUrl& getRootDir();
    void setRootDir(const QUrl& rootDir);

    Q_INVOKABLE bool getDebug() const;
    Q_INVOKABLE void setDebug(bool state);


    Q_INVOKABLE void scan(const QFileInfo& folder);
    Q_INVOKABLE void scan(const QUrl& url);

    Q_INVOKABLE const QString getFileCount(const QUrl& url);

    Q_INVOKABLE Asset* getAsset(const QString& filePath);
    Q_INVOKABLE QStringList getSupportedTypes() const;

    /// These are for assets "creation"
    Q_INVOKABLE QString createTemplateFile(const QString& directory, const QString& templateType);
    Q_INVOKABLE bool createPrefab(SofaBase* node);
    Q_INVOKABLE bool createPythonPrefab(QString name, SofaBase* node);
    Q_INVOKABLE QUrl chooseProjectDir();
    Q_INVOKABLE QString createProject(const QUrl& dir);
    Q_INVOKABLE bool createProjectTree(const QUrl& dir);
    Q_INVOKABLE QString importProject(const QUrl& archive);
    Q_INVOKABLE bool exportProject(const QUrl& projectName);

    Q_INVOKABLE void saveScene(const QString filepath, SofaNode* node);

private slots:
    void onDirectoryChanged(const QString &path);
    void onFileChanged(const QString &path);

signals:
    void filesChanged();
    void rootDirChanged(QUrl& rootDir);
    void debugChanged(bool value);

private:
    void removeDirEntries(DirectoryAsset& folder);
    void updateDirectory(const QFileInfo& finfo);
    void updateAsset(const QFileInfo& path);
    bool isFileExcluded(const QFileInfo& f);

    QUrl m_rootDir; // The Project's root fs directory

    QMap<QString, std::shared_ptr<Asset> > m_assets; /// project asset's URLs with their associated loaders
    QMap<QString, std::shared_ptr<DirectoryAsset> > m_directories;

    ProjectMonitor* m_watcher {nullptr};
    bool m_debug {false};
};

}  // namespace sofaqtquick


