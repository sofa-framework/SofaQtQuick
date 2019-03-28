#pragma once

#include <sofa/core/objectmodel/BaseObject.h>
#include "SofaQtQuickGUI/Bindings/SofaComponent.h"

using sofa::core::objectmodel::BaseObject;

#include <QUrl>

namespace sofa {
namespace qtquick {


struct BaseSofaLoaderCreator
{
    virtual ~BaseSofaLoaderCreator();
    virtual SofaComponent* createInstance() = 0;
    virtual const std::string& getClassName() = 0;
};

template <class T>
struct TBaseSofaLoaderCreator : public BaseSofaLoaderCreator
{
    virtual ~TBaseSofaLoaderCreator();
    virtual const std::string& getClassName() override;

protected:
    sofa::core::objectmodel::Base::SPtr m_sofaLoader;
};

class AssetLoader
{
public:
    AssetLoader(std::string path, std::string extension);
    virtual ~AssetLoader();

    virtual SofaComponent* createSofaLoader();

    static const QUrl iconPath;
    static const QString typeString;

protected:
    const std::string m_path;
    const std::string m_extension;

    std::map<std::string, BaseSofaLoaderCreator*> m_loaders;
};

template <class T>
TBaseSofaLoaderCreator<T>::~TBaseSofaLoaderCreator() {}

template <class T>
const std::string& TBaseSofaLoaderCreator<T>::getClassName() { return T::GetClass()->className; }


} // namespace qtquick
} // namespace sofa
