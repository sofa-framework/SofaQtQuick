cmake_minimum_required(VERSION 2.8.12)
project(SofaQtQuickGUI)

set(SOFAQTQUICKGUI_MAJOR_VERSION 1)
set(SOFAQTQUICKGUI_MINOR_VERSION 0)
set(SOFAQTQUICKGUI_VERSION ${SOFAQTQUICKGUI_MAJOR_VERSION}.${SOFAQTQUICKGUI_MINOR_VERSION})

include(cmake/GeneratePluginConfig.cmake)

if(SOFA_BUILD_TESTS)
    find_package(SofaTest QUIET)
    if(SofaTest_FOUND)
        add_subdirectory(SofaQtQuickGUI_test)
    endif()
endif()

set(SOURCE_FILES
    src/SofaQtQuickGUI/SofaQtQuickGUI.cpp
    src/SofaQtQuickGUI/SofaApplication.cpp
    src/SofaQtQuickGUI/SofaViewer.cpp
    src/SofaQtQuickGUI/Windows/CameraView.cpp
    src/SofaQtQuickGUI/Windows/EditView.cpp
    src/SofaQtQuickGUI/Camera.cpp
    src/SofaQtQuickGUI/SofaScene.cpp
    src/SofaQtQuickGUI/SofaComponentList.cpp
    src/SofaQtQuickGUI/Selectable.cpp
    src/SofaQtQuickGUI/SelectableManipulator.cpp
    src/SofaQtQuickGUI/SelectableSofaComponent.cpp
    src/SofaQtQuickGUI/SelectableSofaParticle.cpp
    src/SofaQtQuickGUI/SofaPythonInteractor.cpp
    src/SofaQtQuickGUI/Console.cpp
    src/SofaQtQuickGUI/PythonConsole.cpp
    src/SofaQtQuickGUI/SofaParticleInteractor.cpp
    src/SofaQtQuickGUI/Bindings/SofaFactory.cpp
    src/SofaQtQuickGUI/Bindings/SofaComponent.cpp
    src/SofaQtQuickGUI/Bindings/SofaData.cpp
    src/SofaQtQuickGUI/Bindings/SofaLink.cpp
    src/SofaQtQuickGUI/Manipulators/Manipulator.cpp
    src/SofaQtQuickGUI/Manipulators/Manipulator2D_Translation.cpp
    src/SofaQtQuickGUI/Manipulators/Manipulator2D_Rotation.cpp
    src/SofaQtQuickGUI/Manipulators/Manipulator3D_Translation.cpp
    src/SofaQtQuickGUI/Manipulators/Manipulator3D_Rotation.cpp
    src/SofaQtQuickGUI/Models/SofaDataListModel.cpp
    src/SofaQtQuickGUI/Models/SofaDisplayFlagsTreeModel.cpp
    src/SofaQtQuickGUI/Models/SofaInspectorDataListModel.cpp
    src/SofaQtQuickGUI/Models/SofaSceneListModel.cpp
    src/SofaQtQuickGUI/Models/SofaSceneListProxy.cpp
    src/SofaQtQuickGUI/Models/SofaSceneItemModel.cpp
    src/SofaQtQuickGUI/Models/SofaSceneItemProxy.cpp
    src/SofaQtQuickGUI/Models/SofaViewListModel.cpp
    src/SofaQtQuickGUI/SofaCamera.cpp
    src/SofaQtQuickGUI/Rendering/ObjectRenderer.cpp
    src/SofaQtQuickGUI/Rendering/SceneRenderer.cpp

    src/SofaQtQuickGUI/RuntimeViewer.cpp
    src/SofaQtQuickGUI/LiveFileMonitor.cpp

)

set(HEADER_FILES
    src/SofaQtQuickGUI/SofaQtQuickGUI.h
    src/SofaQtQuickGUI/SofaApplication.h
    src/SofaQtQuickGUI/SofaCamera.h
    src/SofaQtQuickGUI/ProcessState.h
    src/SofaQtQuickGUI/SofaViewer.h
    src/SofaQtQuickGUI/Windows/CameraView.h
    src/SofaQtQuickGUI/Windows/EditView.h
    src/SofaQtQuickGUI/Camera.h
    src/SofaQtQuickGUI/SofaScene.h
    src/SofaQtQuickGUI/SofaComponentList.h
    src/SofaQtQuickGUI/Selectable.h
    src/SofaQtQuickGUI/SelectableManipulator.h
    src/SofaQtQuickGUI/SelectableSofaComponent.h
    src/SofaQtQuickGUI/SelectableSofaParticle.h
    src/SofaQtQuickGUI/SofaPythonInteractor.h
    src/SofaQtQuickGUI/Console.h
    src/SofaQtQuickGUI/PythonConsole.h
    src/SofaQtQuickGUI/SofaParticleInteractor.h
    src/SofaQtQuickGUI/Bindings/SofaFactory.h
    src/SofaQtQuickGUI/Bindings/SofaComponent.h
    src/SofaQtQuickGUI/Bindings/SofaData.h
    src/SofaQtQuickGUI/Bindings/SofaLink.h
    src/SofaQtQuickGUI/Manipulators/Manipulator.h
    src/SofaQtQuickGUI/Manipulators/Manipulator2D_Translation.h
    src/SofaQtQuickGUI/Manipulators/Manipulator2D_Rotation.h
    src/SofaQtQuickGUI/Manipulators/Manipulator3D_Translation.h
    src/SofaQtQuickGUI/Manipulators/Manipulator3D_Rotation.h
    src/SofaQtQuickGUI/Models/SofaDataListModel.h
    src/SofaQtQuickGUI/Models/SofaDisplayFlagsTreeModel.h
    src/SofaQtQuickGUI/Models/SofaInspectorDataListModel.h
    src/SofaQtQuickGUI/Models/SofaSceneListModel.h
    src/SofaQtQuickGUI/Models/SofaSceneListProxy.h
    src/SofaQtQuickGUI/Models/SofaSceneItemModel.h
    src/SofaQtQuickGUI/Models/SofaSceneItemProxy.h
    src/SofaQtQuickGUI/Models/SofaViewListModel.h
    src/SofaQtQuickGUI/Rendering/ObjectRenderer.h
    src/SofaQtQuickGUI/Rendering/SceneRenderer.h
    src/SofaQtQuickGUI/Helper/QMathExtensions.h
    src/SofaQtQuickGUI/RuntimeViewer.h
    src/SofaQtQuickGUI/LiveFileMonitor.h
)


set(QML_FILES
    data/qml/SofaDataTypes/qmldir
    data/qml/SofaDataTypes/SofaDataType_notimplementedyet.qml
    data/qml/SofaDataTypes/SofaDataType_boolean.qml
    data/qml/SofaDataTypes/SofaDataType_0to1RatioWidget.notimplementedyet.qml
    data/qml/SofaDataTypes/SofaDataType_array.qml
    data/qml/SofaDataTypes/SofaDataType_boolean.qml
    data/qml/SofaDataTypes/SofaDataType_BoundingBox.qml
    data/qml/SofaDataTypes/SofaDataType_Material.qml
    data/qml/SofaDataTypes/SofaDataType_OptionsGroup.qml
    data/qml/SofaDataTypes/SofaDataType_FileName.qml
    data/qml/SofaDataTypes/SofaDataType_graph_linear.notimplementedyet.qml
    data/qml/SofaDataTypes/SofaDataType_graph.notimplementedyet.qml
    data/qml/SofaDataTypes/SofaDataType_number.qml
    data/qml/SofaDataTypes/SofaDataType_poissonRatio.notimplementedyet.qml
    data/qml/SofaDataTypes/SofaDataType_string.qml
    data/qml/SofaDataTypes/SofaDataType_RGBAColor.qml
    data/qml/SofaDataTypes/SofaDataType_widget_displayFlags.qml
    data/qml/SofaApplication/qmldir
    data/qml/SofaApplication/SofaApplication.qml
    data/qml/SofaApplication/MainStyle.qml
    data/qml/SofaBasics/qmldir
    data/qml/SofaBasics/IconButton.qml
    data/qml/SofaBasics/ToolTip.qml
    data/qml/SofaBasics/BubbleToolTip.qml
    data/qml/SofaBasics/DefaultStatusBar.qml
    data/qml/SofaBasics/DefaultToolBar.qml
    data/qml/SofaBasics/DefaultMenuBar.qml
    data/qml/SofaBasics/SofaScene.qml
    data/qml/SofaBasics/SofaSceneInterface.qml
    data/qml/SofaBasics/SofaDataItem.qml
    data/qml/SofaBasics/SofaPythonInteractor.qml
    data/qml/SofaBasics/UserInteractor.qml
    data/qml/SofaBasics/Fit.qml
    data/qml/SofaBasics/Camera.qml
    data/qml/SofaBasics/SofaCamera.qml
    data/qml/SofaBasics/FPSItem.qml
    data/qml/SofaBasics/SofaTimeItem.qml
    data/qml/SofaBasics/ContentItem.qml
    data/qml/SofaBasics/ExplicitSwitch.qml
    data/qml/SofaBasics/RangeSlider.qml
    data/qml/SofaBasics/CollapsibleGroupBox.qml
    data/qml/SofaBasics/Vector2DSpinBox.qml
    data/qml/SofaBasics/Vector3DSpinBox.qml
    data/qml/SofaBasics/Vector4DSpinBox.qml
    data/qml/SofaBasics/DynamicSplitView.qml
    data/qml/SofaBasics/DynamicContent.qml
    data/qml/SofaBasics/MementoBar.qml
    data/qml/SofaBasics/MementoObject.qml
    data/qml/SofaBasics/MementoManager.qml
    data/qml/SofaBasics/WindowResizer.qml
    data/qml/SofaBasics/ClipPlane.qml
    data/qml/SofaBasics/ColorChooser.qml
    data/qml/SofaWidgets/SofaSearchBar.qml
    data/qml/SofaWidgets/SofaNodeMenu.qml
    data/qml/SofaWidgets/SofaObjectMenu.qml
    data/qml/SofaWidgets/SofaWindowDataListView.qml
    data/qml/SofaWidgets/SofaWindowComponentHelp.qml
    data/qml/SofaWidgets/SofaWindowComponentMessages.qml
    data/qml/SofaWidgets/PopupWindowCreateComponent.qml
    data/qml/SofaWidgets/qmldir
    data/qml/SofaInteractors/qmldir
    data/qml/SofaInteractors/UserInteractor_MoveCamera.qml
    data/qml/SofaInteractors/UserInteractor_Selection.qml
    data/qml/SofaInteractors/UserInteractor_SofaParticleInteractor.qml
    data/qml/SofaManipulators/qmldir
    data/qml/SofaManipulators/Manipulator.qml
    data/qml/SofaManipulators/Manipulator2D_Translation.qml
    data/qml/SofaManipulators/Manipulator2D_Rotation.qml
    data/qml/SofaManipulators/Manipulator3D_Translation.qml
    data/qml/SofaManipulators/Manipulator3D_Rotation.qml
    data/qml/SofaViews/qmldir
    data/qml/SofaViews/QML_LiveCoding.qml
    data/qml/SofaViews/SofaViewer.qml
    data/qml/SofaViews/SofaDataView.qml
    data/qml/SofaViews/ImageViewer.qml
    data/qml/SofaViews/PythonConsole.qml
    data/qml/SofaViews/SofaDataListView.qml
    data/qml/SofaViews/SofaSceneGraphView.qml
    data/qml/SofaViews/SofaSceneToolPanel.qml
    data/qml/SofaViews/SofaSimulationControl.qml
    data/qml/SofaViews/SofaVisualizationControl.qml
    data/qml/SofaViews/AffineTransformationController.qml
    data/qml/SofaViews/ColorSelector.qml
    data/qml/SofaViews/SofaSceneViewer.qml
    data/qml/SofaViews/CameraView.qml
    data/qml/SofaViews/EditView.qml
    data/qml/SofaViews/Inspector.qml
    data/qml/SofaViews/Message.qml
    data/qml/SofaViews/SceneGraph.qml
    data/qml/SofaViews/Hierarchy.qml
    data/qml/SofaViews/ExampleView.qml
    data/qml/SofaViews/WebBrowserView.qml
    data/qml/SofaViews/LiveInterfaceEditor.qml
    data/qml/SofaWidgets/RuntimeViewer.qml
    data/qml/SofaWidgets/BaseWidgetExample.qml
    data/qml/SofaWidgets/SceneToolsPanel.qml
)

add_definitions("-DSOFA_SOFAQTQUICKGUI_SRC_DIR=\"${CMAKE_CURRENT_LIST_DIR}/data/qml/\"")

list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaDataTypes/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaApplication/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaBasics/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaInteractors/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaManipulators/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaWidgets/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaViews/")
set(QML_IMPORT_PATH "${QML_DIRS}" CACHE STRING "Qt Creator extra qml import paths" FORCE)

set(CONFIG_FILES
    data/config/default.ini
    data/config/classic.ini
    data/config/fullview.ini
    data/config/quadview.ini
)

set(PYTHON_FILES
    data/python/BaseHeader.py
    data/python/PythonConsole.py
)

set(QRC_FILES
    data/sofaQtQuickGUI_resources.qrc
    data/qml/sofaQtQuickGUI_qml.qrc
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# on Window, Qt packages need the glu32 lib full path, but they do not find it without help
if(WIN32)
    if(CMAKE_CL_64)
        set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um/x64")
    else()
        set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um/x86")
    endif()
endif()

find_package(Qt5 COMPONENTS Core Gui Widgets Quick QuickControls2 Qml WebEngine  REQUIRED PATHS "${SOFA-EXTERNAL_QT5_PATH}")


include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5QuickControls2_INCLUDE_DIRS})
include_directories(${Qt5Qml_INCLUDE_DIRS})
include_directories(${Qt5Quick_INCLUDE_DIRS})
include_directories(${Qt5Quick_INCLUDE_DIRS})
include_directories(${Qt5QmlLive_INCLUDE_DIRS})

add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5Gui_DEFINITIONS})
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5QuickControls2_DEFINITIONS})
add_definitions(${Qt5Qml_DEFINITIONS})
add_definitions(${Qt5Quick_DEFINITIONS})


if (WIN32)
    # Does not work with Visual Studio config generation
    #if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG")
endif()

set(_configPluginFileName plugin_list.conf)
set(_defaultConfigPluginFileName "${_configPluginFileName}.default")
set(_defaultConfigPluginFilePath "${CMAKE_BINARY_DIR}/${_pluginLocation}/${_defaultConfigPluginFileName}")
sofa_generate_plugin_config_qtquick(${_defaultConfigPluginFilePath})
message("Write Plugin list at ${_defaultConfigPluginFilePath}")

qt5_wrap_cpp(MOC_FILES ${HEADER_FILES}) # do not use "set(CMAKE_AUTOMOC ON)" since all the mocced files will be compiled by a single compilation unit leading to a "out of heap space" issue on MSVC
qt5_add_resources(RESOURCE_FILES ${QRC_FILES})

# We need to put the .inl after the call of qt5_wrap_cpp to avoid having the inline mocced.
set(HEADER_FILES ${HEADER_FILES}
    src/SofaQtQuickGUI/SofaParticleInteractor.inl
)


add_definitions(-DCONFIG_PLUGIN_FILENAME=${_configPluginFileName})
add_definitions(-DDEFAULT_CONFIG_PLUGIN_FILENAME=${_defaultConfigPluginFileName})

find_package(SofaFramework REQUIRED)
find_package(SofaPython REQUIRED)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${MOC_FILES} ${SOURCE_FILES} ${QRC_FILES} ${RESOURCE_FILES} ${QML_FILES} ${CONFIG_FILES} ${PYTHON_FILES})
target_link_libraries(${PROJECT_NAME} SofaPython SofaSimulationGraph SofaComponentBase Qt5::Core Qt5::Gui Qt5::Widgets Qt5::QuickControls2 Qt5::Qml Qt5::Quick Qt5::WebEngine)

target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/>")
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${SOFAQTQUICKGUI_VERSION})

add_definitions("-DSOFA_BUILD_SOFAQTQUICKGUI")
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DQT_QML_DEBUG")
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DQMLJSDEBUGGER")

##TODO(dmarchal) A virer et remplacer par des vrai resources.
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DSOFAQTQUICK_DIRECTORY_VIEW=\"${CMAKE_CURRENT_SOURCE_DIR}/data/qml/SofaViews\"")


if(NOT MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC "$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:${CXX11_FLAG}>")
endif()


## Install rules for the library; CMake package configurations files
sofa_create_package(${PROJECT_NAME} ${SOFAQTQUICKGUI_VERSION} ${PROJECT_NAME} ${PROJECT_NAME})

## Install rules for the resources
install(DIRECTORY examples/ DESTINATION share/sofa/plugins/${PROJECT_NAME})

## Hand-crafted install rules for the headers, to preserve the directory structure
foreach(HEADER ${HEADER_FILES})
    string(REGEX MATCH "(.*)[/\\]" DIR ${HEADER})
    install(FILES ${HEADER} DESTINATION "include/${PROJECT_NAME}/${DIR}")
endforeach(HEADER HEADERS)
