#pragma once

#include "SofaQtQuickGUI/Assets/AssetFactory.h"
#include "SofaQtQuickGUI/config.h"


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

    Q_INVOKABLE void scanProject(const QDir& folder);
    Q_INVOKABLE void scanProject(const QUrl& url);
    Q_INVOKABLE const QString getFileCount(const QUrl& url);

    Q_INVOKABLE sofa::qtquick::SofaComponent* getAsset(const QUrl& url);
    Q_INVOKABLE void getAssetMetaInfo(const QUrl& url);


private:
    QUrl m_rootDir; // The Project's root fs directory
    AssetFactory m_assetFactory; /// The factory to create SofaComponents & query meta data
//    QProgressDialog* m_progress;

    using assetMapPair = std::pair<QString, Asset::SPtr >;
    using assetMapIterator = std::map<QString, Asset::SPtr >::iterator;
    std::map<QString, Asset::SPtr > m_assets; /// project asset's URLs with their associated loaders

    void _scanProject(const QDir& folder);
};

}  // namespace qtquick
}  // namespace sofa


