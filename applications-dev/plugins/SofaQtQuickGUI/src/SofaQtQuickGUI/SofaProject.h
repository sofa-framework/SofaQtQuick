#pragma once

#include <QObject>
#include <QSettings>
#include <QDir>
#include <QMap>
#include <QUrl>
#include <QFileSystemWatcher>

#include "SofaQtQuickGUI/Assets/AssetFactory.h"
#include "SofaQtQuickGUI/Assets/DirectoryAsset.h"
#include "SofaQtQuickGUI/config.h"
#include "SofaQtQuickGUI/Bindings/SofaBase.h"
#include "SofaQtQuickGUI/SofaBaseScene.h"
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
    Q_PROPERTY(QString rootDirPath READ getRootDirPath NOTIFY rootDirPathChanged)
    Q_PROPERTY(bool isDebugPrintEnabled READ getDebug WRITE setDebug NOTIFY debugChanged)
    Q_PROPERTY(SofaBaseScene* scene READ getScene WRITE setScene NOTIFY sceneChanged)

public:
    SofaProject();
    ~SofaProject();

    const QUrl& getRootDir();
    void setRootDir(const QUrl& rootDir);

    QString getRootDirPath() {
        return getRootDir().path();
    }

    Q_INVOKABLE bool getDebug() const;
    Q_INVOKABLE void setDebug(bool state);


    Q_INVOKABLE SofaBaseScene* getScene() { return m_currentScene; }
    Q_INVOKABLE void setScene(SofaBaseScene* scene) { m_currentScene = scene; }

    Q_INVOKABLE void newProject();
    Q_INVOKABLE void openProject();
    Q_INVOKABLE void importProject();
    Q_INVOKABLE void exportProject();
    Q_INVOKABLE void saveSceneAsNewProject();



    Q_INVOKABLE void scan(const QFileInfo& folder);
    Q_INVOKABLE void scan(const QUrl& url);

    Q_INVOKABLE const QString getFileCount(const QUrl& url);

    Q_INVOKABLE Asset* getAsset(const QString& filePath);
    Q_INVOKABLE QStringList getSupportedTypes() const;

    /// These are for assets "creation"
    Q_INVOKABLE QString createTemplateFile(const QString& directory, const QString& templateType);
    Q_INVOKABLE bool createPrefab(SofaBase* node);
    Q_INVOKABLE bool createPythonPrefab(QString name, SofaBase* node);
    Q_INVOKABLE QUrl chooseProjectDir(QString windowTitle = tr("Choose Project Location"), QString baseDir = "~/Documents", int opts = 0);
    Q_INVOKABLE QUrl getOpenFile(QString windowTitle = tr("Choose a file"), QString baseDir = "~/Documents",
                                 int opts = 0, QString nameFilters = "");
    Q_INVOKABLE QUrl getSaveFile(QString windowTitle = tr("Choose a file"), QString baseDir = "~/Documents",
                                 int opts = 0, QString nameFilters = "");
    Q_INVOKABLE QString createProject(const QUrl& dir);
    Q_INVOKABLE bool createProjectTree(const QUrl& dir);
    Q_INVOKABLE QString importProject(const QUrl& archive);

private slots:
    void onDirectoryChanged(const QString &path);
    void onFileChanged(const QString &path);

signals:
    void sceneChanged(SofaBaseScene*);
    void filesChanged();
    void rootDirChanged(QUrl& rootDir);
    void rootDirPathChanged(QString rootDir);
    void debugChanged(bool value);

private:
    void removeDirEntries(DirectoryAsset& folder);
    void updateDirectory(const QFileInfo& finfo);
    void updateAsset(const QFileInfo& path);
    bool isFileExcluded(const QFileInfo& f);

    QUrl m_rootDir; // The Project's root fs directory

    QMap<QString, std::shared_ptr<Asset> > m_assets; /// project asset's URLs with their associated loaders
    QMap<QString, std::shared_ptr<DirectoryAsset> > m_directories;
    QSettings* m_projectSettings {nullptr};

    ProjectMonitor* m_watcher {nullptr};
    bool m_debug {false};
    SofaBaseScene* m_currentScene;
};

}  // namespace sofaqtquick


