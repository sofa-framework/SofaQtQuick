#pragma once
#include "Asset.h"
#include <QObject>
#include <SofaQtQuickGUI/config.h>
#include <iostream>
#include <map>

namespace sofaqtquick
{

/**
 *  \brief interface for the Asset's factory creators
 */
class SOFA_SOFAQTQUICKGUI_API BaseAssetCreator
{
  public:
    virtual ~BaseAssetCreator();
    virtual QSharedPointer<Asset> createInstance(std::string path,
                                                  std::string extension) = 0;
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
    virtual QSharedPointer<Asset> createInstance(std::string path,
                                                 std::string extension);
};

/**
 *  \brief Factory holding all possible asset types.
 *
 *   - Can instantiate an Asset according to a file's extension & path
 *   - Can query an asset's Icon from its extension
 */
class SOFA_SOFAQTQUICKGUI_API AssetFactory
{
  public:
    static QStringList getSupportedTypes();
    static QSharedPointer<Asset> createInstance(const QString& path,
                                                 const QString& extension);
    static bool registerAsset(const std::string& extension,
                              BaseAssetCreator* creator);
    static void clearLoaders();

  private:
    // A local static map (inside a static method) guarantees the order of
    // creation of those static objects in the different translation units
    inline static std::map<std::string, BaseAssetCreator*>& getFactoryCreators() {
        static std::map<std::string, BaseAssetCreator*> _creators;
        return _creators;
    }
};

} // namespace sofaqtquick
