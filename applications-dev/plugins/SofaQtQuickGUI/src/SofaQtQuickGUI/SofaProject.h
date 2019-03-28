#pragma once

#include "SofaQtQuickGUI/Assets/AssetLoaderFactory.h"
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

    Q_PROPERTY(AssetLoaderFactory* assetFactory READ getAssetFactory WRITE setAssetFactory NOTIFY assetFactoryChanged)
    AssetLoaderFactory* getAssetFactory();
    void setAssetFactory(AssetLoaderFactory* ) {}
    Q_SIGNAL void assetFactoryChanged(AssetLoaderFactory* );

    Q_INVOKABLE void scanProject(const QDir& folder);
    Q_INVOKABLE void scanProject(const QUrl& url);
    Q_INVOKABLE const QString getFileCount(const QUrl& url);

private:
    QUrl m_rootDir; // The Project's root fs directory
    AssetLoaderFactory m_assetFactory; /// The factory to create SofaComponents & query meta data

    using assetMapPair = std::pair<QString, std::shared_ptr<AssetLoader> >;
    std::map<QString,std::shared_ptr<AssetLoader> > m_assets; /// project asset's URLs with their associated loaders
};

}  // namespace qtquick
}  // namespace sofa


