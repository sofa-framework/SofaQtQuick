#pragma once

#include "SofaQtQuickGUI/Assets/AssetFactory.h"
#include "SofaQtQuickGUI/config.h"
#include "SofaQtQuickGUI/Bindings/SofaBase.h"
using sofaqtquick::bindings::SofaBase;


#include <QObject>
#include <QDir>
#include <QUrl>


namespace sofa {
namespace qtquick {

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

    Q_PROPERTY(AssetFactory* assetFactory READ getAssetFactory WRITE setAssetFactory NOTIFY assetFactoryChanged)
    AssetFactory* getAssetFactory();
    void setAssetFactory(AssetFactory* ) {}
    Q_SIGNAL void assetFactoryChanged(AssetFactory* );

    Q_INVOKABLE QString createProject(const QUrl& dir);
    Q_INVOKABLE bool createProjectTree(const QUrl& dir);
    Q_INVOKABLE QString importProject(const QUrl& archive);
    Q_INVOKABLE bool exportProject(const QUrl& projectName);
    Q_INVOKABLE void scanProject(const QDir& folder);
    Q_INVOKABLE void scanProject(const QUrl& url);
    Q_INVOKABLE const QString getFileCount(const QUrl& url);

    Q_INVOKABLE sofaqtquick::bindings::SofaNode* getAsset(const QUrl& url, const QString& assetName = "");
    Q_INVOKABLE QList<QObject*> getAssetMetaInfo(const QUrl& url);

    Q_INVOKABLE bool createPrefab(SofaBase* node);

private:
    QUrl m_rootDir; // The Project's root fs directory
    AssetFactory m_assetFactory; /// The factory to create SofaComponents & query meta data

    using assetMapPair = std::pair<QString, Asset::SPtr >;
    using assetMapIterator = std::map<QString, Asset::SPtr >::iterator;
    std::map<QString, Asset::SPtr > m_assets; /// project asset's URLs with their associated loaders

    void _scanProject(const QDir& folder);
};

}  // namespace qtquick
}  // namespace sofa


