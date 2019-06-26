cmake_minimum_required(VERSION 2.8.12)
project(SofaQtQuickGUI)

set(SOFAQTQUICKGUI_MAJOR_VERSION 0)
set(SOFAQTQUICKGUI_MINOR_VERSION 1)
set(SOFAQTQUICKGUI_VERSION ${SOFAQTQUICKGUI_MAJOR_VERSION}.${SOFAQTQUICKGUI_MINOR_VERSION})

include(cmake/GeneratePluginConfig.cmake)

if(SOFA_BUILD_TESTS)
    find_package(SofaTest QUIET)
    if(SofaTest_FOUND)
        add_subdirectory(SofaQtQuickGUI_test)
    endif()
endif()

set(SOURCE_FILES
    SofaQtQuickGUI.cpp
    SofaApplication.cpp
    SofaViewer.cpp
    ControlledSofaViewer.cpp
    Camera.cpp
    SofaScene.cpp
    SofaComponent.cpp
    SofaComponentList.cpp
    SofaData.cpp
    SofaSceneListModel.cpp
    Selectable.cpp
    SelectableManipulator.cpp
    SelectableSofaComponent.cpp
    SelectableSofaParticle.cpp
    SofaDataListModel.cpp
    SofaDisplayFlagsTreeModel.cpp
    SofaPythonInteractor.cpp
    SofaInspectorDataListModel.cpp
    Console.cpp
    PythonConsole.cpp
    SofaParticleInteractor.cpp
    Manipulator.cpp
    Manipulator2D_Translation.cpp
    Manipulator2D_Rotation.cpp
    Manipulator3D_Translation.cpp
    Manipulator3D_Rotation.cpp
    SofaCamera.cpp
)

set(HEADER_FILES
    SofaQtQuickGUI.h
    SofaApplication.h
    ProcessState.h
    SofaViewer.h
    ControlledSofaViewer.h
    Camera.h
    SofaScene.h
    SofaComponent.h
    SofaComponentList.h
    SofaData.h
    SofaSceneListModel.h
    Selectable.h
    SelectableManipulator.h
    SelectableSofaComponent.h
    SelectableSofaParticle.h
    SofaDataListModel.h
    SofaDisplayFlagsTreeModel.h
    SofaPythonInteractor.h
    SofaInspectorDataListModel.h
    Console.h
    PythonConsole.h
    SofaParticleInteractor.h
    Manipulator.h
    Manipulator2D_Translation.h
    Manipulator2D_Rotation.h
    Manipulator3D_Translation.h
    Manipulator3D_Rotation.h
    SofaCamera.h
    helper/QMathExtensions.h
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
    data/qml/SofaWidgets/qmldir
    data/qml/SofaWidgets/QML_LiveCoding.qml
    data/qml/SofaWidgets/SofaViewer.qml
    data/qml/SofaWidgets/ControlledSofaViewer.qml
    data/qml/SofaWidgets/SofaDataView.qml
    data/qml/SofaWidgets/ImageViewer.qml
    data/qml/SofaWidgets/PythonConsole.qml
    data/qml/SofaWidgets/SofaDataListView.qml
    data/qml/SofaWidgets/SofaSceneGraphView.qml
    data/qml/SofaWidgets/SofaSceneToolPanel.qml
    data/qml/SofaWidgets/SofaSimulationControl.qml
    data/qml/SofaWidgets/SofaVisualizationControl.qml
    data/qml/SofaWidgets/AffineTransformationController.qml
    data/qml/SofaWidgets/ColorSelector.qml
    data/qml/SofaWidgets/SofaSceneViewer.qml

    data/qml/SofaWidgets/Inspector.qml
    data/qml/SofaWidgets/Message.qml
)


list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaDataTypes/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaApplication/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaBasics/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaInteractors/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaManipulators/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaWidgets/")
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

find_package(Qt5 COMPONENTS Core Gui Widgets Quick QuickControls2 Qml REQUIRED PATHS "${SOFA-EXTERNAL_QT5_PATH}")

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5QuickControls2_INCLUDE_DIRS})
include_directories(${Qt5Qml_INCLUDE_DIRS})
include_directories(${Qt5Quick_INCLUDE_DIRS})

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

set(HEADER_FILES ${HEADER_FILES}
    SofaParticleInteractor.inl
)

add_definitions(-DCONFIG_PLUGIN_FILENAME=${_configPluginFileName})
add_definitions(-DDEFAULT_CONFIG_PLUGIN_FILENAME=${_defaultConfigPluginFileName})

find_package(SofaFramework REQUIRED)
find_package(SofaPython REQUIRED)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${MOC_FILES} ${SOURCE_FILES} ${QRC_FILES} ${RESOURCE_FILES} ${QML_FILES} ${CONFIG_FILES} ${PYTHON_FILES})
target_link_libraries(${PROJECT_NAME} SofaPython SofaSimulationGraph SofaComponentBase SofaOpenglVisual Qt5::Core Qt5::Gui Qt5::Widgets Qt5::QuickControls2 Qt5::Qml Qt5::Quick)

target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>")
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${SOFAQTQUICKGUI_VERSION})

add_definitions("-DSOFA_BUILD_SOFAQTQUICKGUI")


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
