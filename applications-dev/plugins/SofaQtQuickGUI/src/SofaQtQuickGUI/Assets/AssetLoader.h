#pragma once

#include <sofa/core/objectmodel/BaseObject.h>
using sofa::core::objectmodel::BaseObject;

#include <QUrl>

namespace sofa {
namespace qtquick {


struct BaseSofaLoaderCreator
{
    virtual ~BaseSofaLoaderCreator();
    virtual core::objectmodel::BaseObject::SPtr createInstance() = 0;
    virtual const std::string& getClassName() = 0;
};

template <class T>
struct TBaseSofaLoaderCreator : public BaseSofaLoaderCreator
{
    virtual ~TBaseSofaLoaderCreator();
    virtual const std::string& getClassName();
};

class AssetLoader
{
public:
    AssetLoader(std::string path, std::string extension);
    virtual ~AssetLoader();

    virtual BaseObject::SPtr createSofaLoader();

    static const QUrl iconPath;

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
