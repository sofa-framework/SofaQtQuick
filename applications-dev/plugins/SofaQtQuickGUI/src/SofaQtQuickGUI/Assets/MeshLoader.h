#pragma once

#include "AssetLoader.h"
#include <sofa/core/loader/MeshLoader.h>
#include <SofaLoader/MeshObjLoader.h>
#include <SofaLoader/MeshVTKLoader.h>
#include <SofaGeneralLoader/MeshSTLLoader.h>
#include <SofaGeneralLoader/MeshGmshLoader.h>
#include <SofaGeneralLoader/MeshXspLoader.h>
using namespace sofa::component::loader;


namespace sofa
{
namespace qtquick
{

template<class T>
struct MeshLoaderCreator : public TBaseSofaLoaderCreator<T>
{
    MeshLoaderCreator(const std::string& path) : m_path(path) {}

    SofaComponent* createInstance() override {
        return nullptr;
    }

private:
    std::string m_path;
};


//template<>
//SofaComponent* MeshLoaderCreator<MeshSTLLoader>::createInstance()
//{
//    m_sofaLoader = core::objectmodel::New<MeshSTLLoader>();

//    return new SofaComponent(nullptr, m_sofaLoader.get());
//}


class MeshLoader : public AssetLoader
{
public:
    MeshLoader(std::string path, std::string extension);

    static const QUrl iconPath;
    static const QString typeString;
};

} // namespace qtquick
} // namespace sofa
