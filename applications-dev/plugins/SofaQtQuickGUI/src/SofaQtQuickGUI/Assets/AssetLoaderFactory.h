#pragma once

#include "MeshLoader.h"
#include "TextureLoader.h"
#include <iostream>
#include <map>
#include <QObject>
#include <SofaQtQuickGUI/config.h>

namespace sofa {
namespace qtquick {


/**
 *  \brief interface for the AssetLoader's factory creators
 */
class SOFA_SOFAQTQUICKGUI_API BaseAssetLoaderCreator
{
public:
    virtual ~BaseAssetLoaderCreator();
    virtual std::shared_ptr<AssetLoader> createInstance(std::string path, std::string extension) = 0;
    virtual const QUrl& getIcon() = 0;
    virtual const QString& getTypeString() = 0;
};

/**
 *  \brief AssetLoader's factory creators.
 *
 *  Can intantiate or query meta data on its templated type
 */
template<class T>
class SOFA_SOFAQTQUICKGUI_API AssetLoaderCreator : public BaseAssetLoaderCreator
{
public:
    AssetLoaderCreator();
    virtual ~AssetLoaderCreator();
    virtual std::shared_ptr<AssetLoader> createInstance(std::string path, std::string extension);
    virtual const QUrl& getIcon();
    virtual const QString& getTypeString();
};

/**
 *  \brief Factory holding all possible asset loader types.
 *
 *   - Can instantiate an assetLoader according to a file's extension & path
 *   - Can query an asset's Icon from its extension
 */
class SOFA_SOFAQTQUICKGUI_API AssetLoaderFactory : public QObject
{
    Q_OBJECT
public:
    explicit AssetLoaderFactory(QObject *parent = nullptr);
    ~AssetLoaderFactory();

    Q_INVOKABLE QUrl getIcon(QString extension) const;
    Q_INVOKABLE QString getTypeString(QString extension) const;
    std::shared_ptr<AssetLoader> createInstance(const QString& path, const QString& extension) const;
private:
    std::map<std::string, BaseAssetLoaderCreator*> m_loaders;
};




template <class T>
AssetLoaderCreator<T>::AssetLoaderCreator() {}

template <class T>
AssetLoaderCreator<T>::~AssetLoaderCreator() {}

template <class T>
std::shared_ptr<AssetLoader> AssetLoaderCreator<T>::createInstance(std::string path, std::string extension)
{
    return std::shared_ptr<AssetLoader>(new T(path, extension));
}

template <class T>
const QUrl& AssetLoaderCreator<T>::getIcon()
{
    return T::iconPath;
}

template <class T>
const QString& AssetLoaderCreator<T>::getTypeString()
{
    return T::typeString;
}



} // namespace qtquick
} // namespace sofa
