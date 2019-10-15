#pragma once

#include <QObject>
#include <QDir>
#include <QUrl>

#include "SofaQtQuickGUI/Assets/AssetFactory.h"
#include "SofaQtQuickGUI/config.h"
#include "SofaQtQuickGUI/Bindings/SofaBase.h"
using sofaqtquick::bindings::SofaBase;

#include "LiveFileMonitor.h"

namespace sofa {
namespace qtquick {

class ProjectFileMonitor;

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

    void updatePath(const std::string& file);

private slots:
    void onFilesChanged();
signals:
    void filesChanged();

private:
    QUrl m_rootDir; // The Project's root fs directory

    using assetMapPair = std::pair<QString, std::shared_ptr<Asset> >;
    using assetMapIterator = std::map<QString, std::shared_ptr<Asset> >::iterator;
    std::map<QString, std::shared_ptr<Asset> > m_assets; /// project asset's URLs with their associated loaders

    void _scanProject(const QFileInfo &file);

    ProjectFileMonitor* m_fileMonitor {nullptr};
};

}  // namespace qtquick
}  // namespace sofa


