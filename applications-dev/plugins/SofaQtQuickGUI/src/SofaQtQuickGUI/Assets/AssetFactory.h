#pragma once

#include "MeshAsset.h"
#include "TextureAsset.h"
#include <QObject>
#include <SofaQtQuickGUI/config.h>
#include <iostream>
#include <map>

namespace sofa
{
namespace qtquick
{

/**
 *  \brief interface for the Asset's factory creators
 */
class SOFA_SOFAQTQUICKGUI_API BaseAssetCreator
{
  public:
    virtual ~BaseAssetCreator();
    virtual std::shared_ptr<Asset> createInstance(std::string path,
                                                  std::string extension) = 0;
    virtual const QUrl& getIcon() = 0;
    virtual const QString& getTypeString() = 0;
};

/**
 *  \brief Asset's factory creators.
 *
 *  Can intantiate or query meta data on its templated type
 */
template <class T>
class SOFA_SOFAQTQUICKGUI_API AssetCreator : public BaseAssetCreator
{
  public:
    AssetCreator();
    virtual ~AssetCreator();
    virtual std::shared_ptr<Asset> createInstance(std::string path,
                                                  std::string extension);
    virtual const QUrl& getIcon();
    virtual const QString& getTypeString();
};

/**
 *  \brief Factory holding all possible asset types.
 *
 *   - Can instantiate an Asset according to a file's extension & path
 *   - Can query an asset's Icon from its extension
 */
class SOFA_SOFAQTQUICKGUI_API AssetFactory : public QObject
{
    Q_OBJECT
  public:
    explicit AssetFactory(QObject* parent = nullptr);
    ~AssetFactory();

    Q_INVOKABLE QUrl getIcon(QString extension) const;
    Q_INVOKABLE QString getTypeString(QString extension) const;
    std::shared_ptr<Asset> createInstance(const QString& path,
                                          const QString& extension) const;

  private:
    std::map<std::string, BaseAssetCreator*> m_loaders;
};

template <class T> AssetCreator<T>::AssetCreator() {}

template <class T> AssetCreator<T>::~AssetCreator() {}

template <class T>
std::shared_ptr<Asset> AssetCreator<T>::createInstance(std::string path,
                                                       std::string extension)
{
    return std::shared_ptr<Asset>(new T(path, extension));
}

template <class T> const QUrl& AssetCreator<T>::getIcon()
{
    return T::iconPath;
}

template <class T> const QString& AssetCreator<T>::getTypeString()
{
    return T::typeString;
}

} // namespace qtquick
} // namespace sofa
