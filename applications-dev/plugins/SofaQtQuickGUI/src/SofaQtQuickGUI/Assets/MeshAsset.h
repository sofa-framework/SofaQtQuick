#pragma once

#include "Asset.h"
#include <SofaGeneralLoader/MeshGmshLoader.h>
#include <SofaGeneralLoader/MeshSTLLoader.h>
#include <SofaGeneralLoader/MeshXspLoader.h>
#include <SofaLoader/MeshObjLoader.h>
#include <SofaLoader/MeshVTKLoader.h>
#include <sofa/core/loader/MeshLoader.h>
using namespace sofa::component::loader;

namespace sofaqtquick
{

template <class T>
struct MeshAssetLoader : public TBaseAssetLoader<T>{};


class MeshAsset : public Asset
{
    Q_OBJECT
  public:
    MeshAsset(std::string path, std::string extension);
    virtual sofaqtquick::bindings::SofaNode* create(sofaqtquick::bindings::SofaNode* parent, const QString& assetName = "") override;
    virtual void getDetails() override;
    virtual QUrl getAssetInspectorWidget() override;
    virtual void openThirdParty(); // Opens a 3rd party program to edit / visualize the asset

    Q_PROPERTY(int vertices READ vertices NOTIFY verticesChanged)
    Q_PROPERTY(int faces READ faces NOTIFY facesChanged)
    Q_PROPERTY(int materials READ materials NOTIFY materialsChanged)
    Q_PROPERTY(int meshes READ meshes NOTIFY meshesChanged)
    Q_PROPERTY(QString primitiveType READ primitiveType NOTIFY primitiveTypeChanged)
//    Q_PROPERTY(int minPoint READ minPoint NOTIFY minPointChanged)
//    Q_PROPERTY(int maxPoint READ maxPoint NOTIFY maxPointChanged)
//    Q_PROPERTY(int centerPoint READ centerPoint NOTIFY centerPointChanged)
//    Q_PROPERTY(bool isLoaded READ isLoaded NOTIFY isLoadedChanged)

protected:
    int m_vertices = 0;
    int m_faces = 0;
    int m_materials = 0;
    int m_meshes = 0;
    QString m_primitiveType = "";
    QList<double> m_minPoint = QList<double>();
    QList<double> m_maxPoint = QList<double>();
    QList<double> m_centerPoint = QList<double>();

    static const LoaderMap _loaders;

    static LoaderMap createLoaders();

protected:
    Q_INVOKABLE virtual QUrl getIconPath() override { return QUrl("qrc:/icon/ICON_MESH.png"); }
    Q_INVOKABLE virtual QString getTypeString() override { return "3D mesh file"; }
    Q_INVOKABLE virtual bool getIsSofaContent() override { return true; }

    Q_INVOKABLE int vertices() { getDetails(); return m_vertices; }
    Q_SIGNAL void verticesChanged();

    Q_INVOKABLE int faces() { getDetails(); return m_faces; }
    Q_SIGNAL void facesChanged();

    Q_INVOKABLE int materials() { getDetails(); return m_materials; }
    Q_SIGNAL void materialsChanged();

    Q_INVOKABLE int meshes() { getDetails(); return m_meshes; }
    Q_SIGNAL void meshesChanged();

    Q_INVOKABLE QString primitiveType() { getDetails(); return m_primitiveType; }
    Q_SIGNAL void primitiveTypeChanged();
};

} // namespace sofaqtquick

