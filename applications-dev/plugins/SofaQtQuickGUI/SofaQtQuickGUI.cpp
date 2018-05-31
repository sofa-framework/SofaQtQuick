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

#include "SofaQtQuickGUI.h"
#include "SofaApplication.h"
#include "ProcessState.h"
#include "Camera.h"
#include "SofaCamera.h"
#include "SofaParticleInteractor.h"
#include "SofaPythonInteractor.h"
#include "Manipulator.h"
#include "Manipulator2D_Translation.h"
#include "Manipulator2D_Rotation.h"
#include "Manipulator3D_Translation.h"
#include "Manipulator3D_Rotation.h"
#include "SofaScene.h"
#include "SofaComponent.h"
#include "SofaData.h"
#include "Selectable.h"
#include "SelectableManipulator.h"
#include "SelectableSofaComponent.h"
#include "SelectableSofaParticle.h"
#include "SofaSceneListModel.h"
#include "SofaDataListModel.h"
#include "SofaInspectorDataListModel.h"
#include "SofaDisplayFlagsTreeModel.h"
#include "SofaViewer.h"
#include "CameraView.h"
#include "PythonConsole.h"
using namespace sofa::qtquick;

#include "Console.h"
using sofa::qtquick::console::Console ;

#include <sofa/helper/system/PluginManager.h>

const int versionMajor = 1;
const int versionMinor = 0;

SofaQtQuickGUI::SofaQtQuickGUI(QObject *parent) : QQmlExtensionPlugin(parent)
{
    init();
}

void SofaQtQuickGUI::init()
{
    Q_INIT_RESOURCE(sofaQtQuickGUI_qml);
    Q_INIT_RESOURCE(sofaQtQuickGUI_resources);

    registerTypes("SofaQtQuickGUI");

    sofa::helper::system::PluginManager::s_gui_postfix = "qtquickgui";
}

// Following the doc on creating a singleton component
// we need to have function that return the singleton instance.
// see: http://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterSingletonType
static QObject* createConsole(QQmlEngine *engine,
                              QJSEngine *scriptEngine){
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new Console() ;
}

void SofaQtQuickGUI::registerTypes(const char* /*uri*/)
{
    //Q_ASSERT(QLatin1String(uri) == QLatin1String("SofaQtQuickGUI"));

    qRegisterMetaType<SofaScene::Status>("Status");
    qRegisterMetaType<SofaSceneListModel::Visibility>("Visibility");

    qmlRegisterType<SofaApplication>                                ("SofaApplicationSingleton"             , versionMajor, versionMinor, "SofaApplication");
    qmlRegisterType<Camera>                                         ("Camera"                               , versionMajor, versionMinor, "Camera");
    qmlRegisterType<SofaCamera>                                     ("SofaCamera"                           , versionMajor, versionMinor, "SofaCamera");
    qmlRegisterType<SofaParticleInteractor>                         ("SofaParticleInteractor"               , versionMajor, versionMinor, "SofaParticleInteractor");
    qmlRegisterType<SofaPythonInteractor>                           ("SofaPythonInteractor"                 , versionMajor, versionMinor, "SofaPythonInteractor");
    qmlRegisterType<Manipulator>                                    ("Manipulator"                          , versionMajor, versionMinor, "Manipulator");
    qmlRegisterType<Manipulator2D_Translation>                      ("Manipulator2D_Translation"            , versionMajor, versionMinor, "Manipulator2D_Translation");
    qmlRegisterType<Manipulator2D_Rotation>                         ("Manipulator2D_Rotation"               , versionMajor, versionMinor, "Manipulator2D_Rotation");
    qmlRegisterType<Manipulator3D_Translation>                      ("Manipulator3D_Translation"            , versionMajor, versionMinor, "Manipulator3D_Translation");
    qmlRegisterType<Manipulator3D_Rotation>                         ("Manipulator3D_Rotation"               , versionMajor, versionMinor, "Manipulator3D_Rotation");
    qmlRegisterType<SofaScene>                                      ("SofaScene"                            , versionMajor, versionMinor, "SofaScene");
    qmlRegisterUncreatableType<SofaComponent> 	                    ("SofaComponent"                        , versionMajor, versionMinor, "SofaComponent", "SofaComponent is not instantiable");
    qmlRegisterUncreatableType<SofaComponentList>                   ("SofaComponentList"                    , versionMajor, versionMinor, "SofaComponentList", "SofaComponentList is not instantiable");
    qmlRegisterUncreatableType<SofaData>                            ("SofaData"                             , versionMajor, versionMinor, "SofaData", "SofaData is not instantiable");
    qmlRegisterUncreatableType<Selectable>                          ("Selectable"                           , versionMajor, versionMinor, "Selectable", "Selectable is not instantiable");
    qmlRegisterUncreatableType<SelectableManipulator>               ("SelectableManipulator"                , versionMajor, versionMinor, "SelectableManipulator", "SelectableManipulator is not instantiable");
    qmlRegisterUncreatableType<SelectableSofaComponent>             ("SelectableSofaComponent"              , versionMajor, versionMinor, "SelectableSofaComponent", "SelectableSofaComponent is not instantiable");
    qmlRegisterUncreatableType<SelectableSofaParticle>              ("SelectableSofaParticle"               , versionMajor, versionMinor, "SelectableSofaParticle", "SelectableSofaParticle is not instantiable");
    qmlRegisterUncreatableType<ProcessState>                        ("ProcessState"                         , versionMajor, versionMinor, "ProcessState", "ProcessState is not instantiable");
    qmlRegisterType<SofaSceneListModel>                             ("SofaSceneListModel"                   , versionMajor, versionMinor, "SofaSceneListModel");
    qmlRegisterType<SofaDataListModel>                              ("SofaDataListModel"                    , versionMajor, versionMinor, "SofaDataListModel");
    qmlRegisterType<SofaInspectorDataListModel>                     ("SofaInspectorDataListModel"           , versionMajor, versionMinor, "SofaInspectorDataListModel");
    qmlRegisterType<SofaDisplayFlagsTreeModel>                      ("SofaDisplayFlagsTreeModel"            , versionMajor, versionMinor, "SofaDisplayFlagsTreeModel");
    qmlRegisterType<SofaViewer>                                     ("SofaViewer"                           , versionMajor, versionMinor, "SofaViewer");
    qmlRegisterType<CameraView>                                     ("CameraView"                           , versionMajor, versionMinor, "CameraView");
    qmlRegisterType<PythonConsole>                                  ("PythonConsole"                        , versionMajor, versionMinor, "PythonConsole");

    // registers the C++ type in the QML system with the name "Console",
    qmlRegisterSingletonType<Console>("SofaMessageList",                    // char* uri
                                       versionMajor, versionMinor,          // int majorVersion
                                      "SofaMessageList",
                                       createConsole );        // exported Name.
}
