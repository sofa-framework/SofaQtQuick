/*
Copyright 2015, Anatoscope

This file is part of sofaqtquick.

sofaqtquick is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

sofaqtquick is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with sofaqtquick. If not, see <http://www.gnu.org/licenses/>.
*/

#include <SofaQtQuickGUI/SofaQtQuickQmlModule.h>
#include <SofaQtQuickGUI/SofaApplication.h>
#include <SofaQtQuickGUI/ProcessState.h>
#include <SofaQtQuickGUI/Camera.h>
#include <SofaQtQuickGUI/SofaParticleInteractor.h>
//#include <SofaQtQuickGUI/SofaPythonInteractor.h>
#include <SofaQtQuickGUI/Manipulators/Manipulator.h>
#include <SofaQtQuickGUI/Manipulators/Manipulator2D_Translation.h>
#include <SofaQtQuickGUI/Manipulators/Manipulator2D_Rotation.h>
#include <SofaQtQuickGUI/Manipulators/Manipulator3D_Translation.h>
#include <SofaQtQuickGUI/Manipulators/Manipulator3D_Rotation.h>
#include <SofaQtQuickGUI/SofaScene.h>

/// Bindings from c++ to QML/Javascript
#include <SofaQtQuickGUI/Bindings/SofaCamera.h>
#include <SofaQtQuickGUI/Bindings/SofaComponent.h>
#include <SofaQtQuickGUI/Bindings/SofaData.h>
#include <SofaQtQuickGUI/Bindings/SofaLink.h>
#include <SofaQtQuickGUI/Bindings/SofaBase.h>
using sofaqtquick::bindings::SofaBase;

#include <SofaQtQuickGUI/Bindings/SofaNode.h>
using sofaqtquick::bindings::SofaNode;
using sofaqtquick::bindings::SofaNodeFactory;

#include <SofaQtQuickGUI/Bindings/SofaBaseObject.h>
using sofaqtquick::bindings::SofaBaseObject;

#include <SofaQtQuickGUI/SofaViewer.h>
#include <SofaQtQuickGUI/Selectable.h>
#include <SofaQtQuickGUI/SelectableManipulator.h>
#include <SofaQtQuickGUI/SelectableSofaComponent.h>
#include <SofaQtQuickGUI/SelectableSofaParticle.h>
#include <SofaQtQuickGUI/Models/SofaSceneListModel.h>
#include <SofaQtQuickGUI/Models/SofaSceneListProxy.h>
#include <SofaQtQuickGUI/Models/SofaSceneItemModel.h>
#include <SofaQtQuickGUI/Models/SofaSceneItemProxy.h>
#include <SofaQtQuickGUI/Models/SofaDataListModel.h>
#include <SofaQtQuickGUI/Models/SofaViewListModel.h>
#include <SofaQtQuickGUI/Models/SofaDataContainerListModel.h>
#include <SofaQtQuickGUI/Models/SofaInspectorDataListModel.h>
#include <SofaQtQuickGUI/Models/SofaDisplayFlagsTreeModel.h>
#include <SofaQtQuickGUI/Windows/CameraView.h>
#include <SofaQtQuickGUI/Windows/EditView.h>
#include <SofaQtQuickGUI/Windows/AssetView.h>
#include <SofaQtQuickGUI/Bindings/SofaCoreBindingContext.h>
#include <SofaQtQuickGUI/Bindings/SofaCoreBindingFactory.h>
using sofaqtquick::bindings::SofaCoreBindingFactory;

#include <SofaQtQuickGUI/PythonConsole.h>
using namespace sofa::qtquick;

#include <SofaQtQuickGUI/Console.h>
using sofa::qtquick::console::Console ;

#include <SofaQtQuickGUI/SofaProject.h>
using sofa::qtquick::SofaProject;

#include <SofaQtQuickGUI/Assets/Asset.h>
using sofa::qtquick::Asset;
#include <SofaQtQuickGUI/Assets/PythonAsset.h>
using sofa::qtquick::PythonAsset;

#include <SofaQtQuickGUI/Assets/MeshAsset.h>
using sofa::qtquick::MeshAsset;
#include <SofaQtQuickGUI/Assets/TextureAsset.h>
using sofa::qtquick::TextureAsset;

#include <SofaQtQuickGUI/Bindings/SofaFactory.h>
using sofaqtquick::bindings::SofaFactory;

#include <SofaQtQuickGUI/LiveQMLFileMonitor.h>
using sofa::qtquick::LiveQMLFileMonitor;

#include <SofaQtQuickGUI/SyntaxHighlight/HighlightComponent.h>
using sofa::qtquick::HighlightComponent;

#include <sofa/helper/system/PluginManager.h>

#include <QQmlPropertyMap>
#include <QQmlContext>

void initRessources()
{
    Q_INIT_RESOURCE(sofaQtQuickGUI_qml);
    Q_INIT_RESOURCE(sofaQtQuickGUI_resources);
}

namespace sofaqtquick
{
const int versionMajor = 1;
const int versionMinor = 0;

// Following the doc on creating a singleton component
// we need to have function that return the singleton instance.
// see: http://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterSingletonType
static QObject* createConsole(QQmlEngine *engine,
                              QJSEngine *scriptEngine){
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new Console() ;
}

// Following the doc on creating a singleton component
// we need to have function that return the singleton instance.
// see: http://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterSingletonType
static QObject* createAnInstanceOfLiveFileMonitor(QQmlEngine *engine,
                                                  QJSEngine *scriptEngine){
    Q_UNUSED(scriptEngine)
    return new LiveQMLFileMonitor(engine) ;
}

// Following the doc on creating a singleton component
// we need to have function that return the singleton instance.
// see: http://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterSingletonType
static QObject* createSofaFactory(QQmlEngine *engine,
                                  QJSEngine *scriptEngine){
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new SofaFactory() ;
}

// Following the doc on creating a singleton component
// we need to have function that return the singleton instance.
// see: http://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterSingletonType
static QObject* createSofaViewListModel(QQmlEngine *engine,
                                        QJSEngine *scriptEngine){
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new SofaViewListModel() ;
}



void registerSofaTypesToQml(const char* /*uri*/)
{
    qRegisterMetaType<SofaScene::Status>("Status");
    qRegisterMetaType<size_t>("const size_t");
    qRegisterMetaType<QVariantMap*>("QVariantMap*");
    qRegisterMetaType<SofaScene*>("const SofaScene*");
    qRegisterMetaType<Asset*>("Asset*");
    qRegisterMetaType<QUrlList>("QUrlList");
    qRegisterMetaType<sofaqtquick::bindings::SofaBaseObject*>("sofaqtquick::bindings::SofaBaseObject*");
    qRegisterMetaType<sofaqtquick::bindings::SofaBaseObject*>("SofaBaseObject*");
    qRegisterMetaType<sofaqtquick::bindings::SofaBaseObjectList*>("SofaBaseObjectList*");

    qRegisterMetaType<sofaqtquick::bindings::SofaBase*>("sofaqtquick::bindings::SofaBase*");
    qRegisterMetaType<sofaqtquick::bindings::SofaBase*>("SofaBase*");

    qRegisterMetaType<sofaqtquick::bindings::SofaData*>("sofaqtquick::bindings::SofaData*");
    qRegisterMetaType<sofaqtquick::bindings::SofaData*>("SofaData*");

    qRegisterMetaType<sofaqtquick::bindings::SofaNode*>("sofaqtquick::bindings::SofaNode*");
    qRegisterMetaType<sofaqtquick::bindings::SofaNode*>("SofaNode*");
    qRegisterMetaType<sofaqtquick::bindings::SofaNodeList*>("SofaNodeList*");

    qmlRegisterType<SofaApplication>                                ("SofaApplicationSingleton"             , versionMajor, versionMinor, "SofaApplicationSingleton");
    qmlRegisterType<Camera>                                         ("Camera"                               , versionMajor, versionMinor, "Camera");
    qmlRegisterType<SofaCamera>                                     ("SofaCamera"                           , versionMajor, versionMinor, "SofaCamera");
    qmlRegisterType<SofaParticleInteractor>                         ("SofaParticleInteractor"               , versionMajor, versionMinor, "SofaParticleInteractor");
//    qmlRegisterType<SofaPythonInteractor>                           ("SofaPythonInteractor"                 , versionMajor, versionMinor, "SofaPythonInteractor");
    qmlRegisterType<Manipulator>                                    ("Manipulator"                          , versionMajor, versionMinor, "Manipulator");
    qmlRegisterType<Manipulator2D_Translation>                      ("Manipulator2D_Translation"            , versionMajor, versionMinor, "Manipulator2D_Translation");
    qmlRegisterType<Manipulator2D_Rotation>                         ("Manipulator2D_Rotation"               , versionMajor, versionMinor, "Manipulator2D_Rotation");
    qmlRegisterType<Manipulator3D_Translation>                      ("Manipulator3D_Translation"            , versionMajor, versionMinor, "Manipulator3D_Translation");
    qmlRegisterType<Manipulator3D_Rotation>                         ("Manipulator3D_Rotation"               , versionMajor, versionMinor, "Manipulator3D_Rotation");
    qmlRegisterType<SofaScene>                                      ("SofaScene"                            , versionMajor, versionMinor, "SofaScene");
    qmlRegisterUncreatableType<SofaComponent> 	                    ("SofaComponent"                        , versionMajor, versionMinor, "SofaComponent", "SofaComponent is not instantiable");
    qmlRegisterUncreatableType<SofaComponentList>                   ("SofaComponentList"                    , versionMajor, versionMinor, "SofaComponentList", "SofaComponentList is not instantiable");
    qmlRegisterUncreatableType<Selectable>                          ("Selectable"                           , versionMajor, versionMinor, "Selectable", "Selectable is not instantiable");
    qmlRegisterUncreatableType<SelectableManipulator>               ("SelectableManipulator"                , versionMajor, versionMinor, "SelectableManipulator", "SelectableManipulator is not instantiable");
    qmlRegisterUncreatableType<SelectableSofaComponent>             ("SelectableSofaComponent"              , versionMajor, versionMinor, "SelectableSofaComponent", "SelectableSofaComponent is not instantiable");
    qmlRegisterUncreatableType<SelectableSofaParticle>              ("SelectableSofaParticle"               , versionMajor, versionMinor, "SelectableSofaParticle", "SelectableSofaParticle is not instantiable");
    qmlRegisterUncreatableType<ProcessState>                        ("ProcessState"                         , versionMajor, versionMinor, "ProcessState", "ProcessState is not instantiable");
    qmlRegisterType<SofaSceneListProxy>                             ("SofaSceneListModel"                   , versionMajor, versionMinor, "SofaSceneListModel");
    qmlRegisterType<SofaSceneItemModel>                             ("SofaSceneItemModel"                   , versionMajor, versionMinor, "SofaSceneItemModel");
    qmlRegisterType<SofaSceneItemProxy>                             ("SofaSceneItemProxy"                   , versionMajor, versionMinor, "SofaSceneItemProxy");
    qmlRegisterType<SofaDataListModel>                              ("SofaDataListModel"                    , versionMajor, versionMinor, "SofaDataListModel");
    qmlRegisterType<SofaDataContainerListModel>                     ("SofaDataContainerListModel"           , versionMajor, versionMinor, "SofaDataContainerListModel");
    qmlRegisterType<SofaInspectorDataListModel>                     ("SofaInspectorDataListModel"           , versionMajor, versionMinor, "SofaInspectorDataListModel");
    qmlRegisterType<SofaDisplayFlagsTreeModel>                      ("SofaDisplayFlagsTreeModel"            , versionMajor, versionMinor, "SofaDisplayFlagsTreeModel");
    qmlRegisterType<SofaViewer>                                     ("SofaViewer"                           , versionMajor, versionMinor, "SofaViewer");
    qmlRegisterType<CameraView>                                     ("CameraView"                           , versionMajor, versionMinor, "CameraView");
    qmlRegisterType<EditView>                                       ("EditView"                             , versionMajor, versionMinor, "EditView");
    qmlRegisterType<AssetView>                                      ("AssetView"                            , versionMajor, versionMinor, "AssetView");
    qmlRegisterType<PythonConsole>                                  ("PythonConsole"                        , versionMajor, versionMinor, "PythonConsole");
    qmlRegisterType<SofaProject>                                    ("SofaProject"                          , versionMajor, versionMinor, "SofaProject");
    qmlRegisterUncreatableType<Asset>                               ("Asset"                                , versionMajor, versionMinor, "Asset", QString("It is not possible to create an Asset"));
    qmlRegisterType<PythonAsset>                                    ("PythonAsset"                          , versionMajor, versionMinor, "PythonAsset");
    qmlRegisterType<HighlightComponent>                             ("HighlightComponent"                   , versionMajor, versionMinor, "HighlightComponent");
    qmlRegisterUncreatableType<MeshAsset>                           ("MeshAsset"                            , versionMajor, versionMinor, "MeshAsset", QString("It is not possible to create a MeshAsset"));
    qmlRegisterUncreatableType<TextureAsset>                        ("TextureAsset"                         , versionMajor, versionMinor, "TextureAsset", QString("It is not possible to create a TextureAsset"));

    qmlRegisterUncreatableType<sofaqtquick::bindings::SofaLink> ("Sofa.Core.SofaLink",
                                                                 versionMajor, versionMinor,
                                                                 "SofaLink",
                                                                 QString("It is not possible to create a Sofa.Core.SofaLink object"));

    qmlRegisterUncreatableType<sofaqtquick::bindings::SofaBase> ("Sofa.Core.SofaBase",
                                                                 versionMajor, versionMinor,
                                                                 "SofaBase",
                                                                 QString("It is not possible to create a Sofa.Core.SofaBase object"));

    qmlRegisterUncreatableType<sofaqtquick::bindings::SofaData> ("Sofa.Core.SofaData",
                                                                 versionMajor, versionMinor,
                                                                 "SofaData",
                                                                 QString("It is not possible to create a Sofa.Core.SofaBaseData object"));


    qmlRegisterType<SofaNode> ("Sofa.Core.SofaNode",
                               versionMajor, versionMinor,
                               "SofaNode");

    qmlRegisterType<SofaBaseObject> ("Sofa.Core.SofaBaseObject",
                                     versionMajor, versionMinor,
                                     "BaseObject");


    /// registers the C++ type in the QML system with the name "Console",
    qmlRegisterSingletonType<Console>("SofaMessageList",                  /// char* uri
                                      versionMajor, versionMinor,         /// int majorVersion
                                      "SofaMessageList",
                                      createConsole );                    /// exported Name.

    /// registers the C++ type in the QML system with the name "SofaFactory",
    qmlRegisterSingletonType<SofaFactory>("Sofa.Core.SofaFactory",                  /// char* uri
                                          versionMajor, versionMinor,     /// int majorVersion
                                          "SofaFactory",
                                          createSofaFactory );            /// exported Name.

    /// registers the C++ type in the QML system with the name "SofaFactory",
    qmlRegisterSingletonType<SofaViewListModel>("SofaViewListModel",                  /// char* uri
                                                versionMajor, versionMinor,     /// int majorVersion
                                                "SofaViewListModel",
                                                createSofaViewListModel );            /// exported Name.

    qmlRegisterSingletonType<LiveQMLFileMonitor>("LiveFileMonitorSingleton",            // char* uri
                                              versionMajor, versionMinor,   // minor/major version number
                                              "LiveFileMonitorSingleton",       // exported name
                                              createAnInstanceOfLiveFileMonitor // the function used to create the singleton instance
                                              );

    SofaCoreBindingFactory::registerType("DAGNode", [](sofa::core::objectmodel::Base* obj)
    {
        return SofaNode::createFrom(obj);
    });

    SofaCoreBindingFactory::registerType("Prefab", [](sofa::core::objectmodel::Base* obj)
    {
        return SofaNode::createFrom(obj);
    });


    sofa::Data<sofa::defaulttype::Vec3d> d(sofa::defaulttype::Vec3d(3,2,1),"dummy helpmsg");

    SofaDataContainerListModel* model = new SofaDataContainerListModel();
    model->setSofaData(new SofaData(&d));
}

void SofaQtQuickQmlModule::RegisterTypes(QQmlEngine* engine)
{
    static bool inited=false;
    if(!inited){
        initRessources();
        registerSofaTypesToQml("SofaQtQuickGUI");
        sofa::helper::system::PluginManager::s_gui_postfix = "qtquickgui";
        inited=true;
    }
    if(engine != sofaqtquick::bindings::SofaCoreBindingContext::getQQmlEngine())
    {
        sofaqtquick::bindings::SofaCoreBindingContext::setQQmlEngine(engine);
        QJSValue factoryObj = engine->newQObject(new SofaNodeFactory());
        engine->globalObject().setProperty("_SofaNodeFactory", factoryObj);
        auto a= engine->evaluate(
            "function as_SofaNode(o) {"
            "    return _SofaNodeFactory.createInstance(o)"
            "}");
    }
}

} /// namespace sofaqtquick
