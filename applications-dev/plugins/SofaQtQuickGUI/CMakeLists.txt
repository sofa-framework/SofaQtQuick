cmake_minimum_required(VERSION 2.8.12)
project(SofaQtQuickGUI)

set(SOFAQTQUICKGUI_MAJOR_VERSION 0)
set(SOFAQTQUICKGUI_MINOR_VERSION 1)
set(SOFAQTQUICKGUI_VERSION ${SOFAQTQUICKGUI_MAJOR_VERSION}.${SOFAQTQUICKGUI_MINOR_VERSION})

set(SOURCE_FILES
    SofaQtQuickGUI.cpp
    SofaApplication.cpp
    Tools.cpp
    Viewer.cpp
    Camera.cpp
    Scene.cpp
    SceneComponent.cpp
    SceneData.cpp
    SceneListModel.cpp
    Selectable.cpp
    SelectableManipulator.cpp
    SelectableSceneComponent.cpp
    SelectableSceneParticle.cpp
    DataListModel.cpp
    PythonInteractor.cpp
    PythonConsole.cpp
    ParticleInteractor.cpp
    Manipulator.cpp
    Manipulator2D_Translation.cpp
    Manipulator2D_Rotation.cpp
    Manipulator3D_Translation.cpp
    Manipulator3D_Rotation.cpp
)

set(HEADER_FILES
    SofaQtQuickGUI.h
    SofaApplication.h
    Tools.h
    Viewer.h
    Camera.h
    Scene.h
    SceneComponent.h
    SceneData.h
    SceneListModel.h
    Selectable.h
    SelectableManipulator.h
    SelectableSceneComponent.h
    SelectableSceneParticle.h
    DataListModel.h
    PythonInteractor.h
    PythonConsole.h
    ParticleInteractor.h
    Manipulator.h
    Manipulator2D_Translation.h
    Manipulator2D_Rotation.h
    Manipulator3D_Translation.h
    Manipulator3D_Rotation.h
)

set(QML_FILES
    data/qml/SofaDataTypes/qmldir
    data/qml/SofaDataTypes/DataType_boolean.qml
    data/qml/SofaDataTypes/DataType_0to1RatioWidget.qml
    data/qml/SofaDataTypes/DataType_array.qml
    data/qml/SofaDataTypes/DataType_boolean.qml
    data/qml/SofaDataTypes/DataType_BoundingBox.qml
    data/qml/SofaDataTypes/DataType_Material.qml
    data/qml/SofaDataTypes/DataType_OptionsGroup.qml
    data/qml/SofaDataTypes/DataType_graph_linear.qml
    data/qml/SofaDataTypes/DataType_graph.qml
    data/qml/SofaDataTypes/DataType_number.qml
    data/qml/SofaDataTypes/DataType_poissonRatio.qml
    data/qml/SofaDataTypes/DataType_string.qml
    data/qml/SofaDataTypes/DataType_widget_displayFlags.qml
    data/qml/SofaCommon/SofaCommonScript.js
    data/qml/SofaCommon/SofaToolsScript.js
    data/qml/SofaCommon/SofaSettingsScript.js
    data/qml/SofaApplication/qmldir
    data/qml/SofaApplication/SofaApplication.qml
    data/qml/SofaBasics/qmldir
    data/qml/SofaBasics/RecentSettings.qml
    data/qml/SofaBasics/UISettings.qml
    data/qml/SofaBasics/Tools.qml
    data/qml/SofaBasics/ToolTip.qml
    data/qml/SofaBasics/DefaultStatusBar.qml
    data/qml/SofaBasics/DefaultToolBar.qml
    data/qml/SofaBasics/DefaultMenuBar.qml
    data/qml/SofaBasics/Scene.qml
    data/qml/SofaBasics/SceneInterface.qml
    data/qml/SofaBasics/DataItem.qml
    data/qml/SofaBasics/DataListModel.qml
    data/qml/SofaBasics/PythonInteractor.qml
    data/qml/SofaBasics/UserInteractor.qml
    data/qml/SofaBasics/Camera.qml
    data/qml/SofaBasics/FPSDisplay.qml
    data/qml/SofaBasics/ContentItem.qml
    data/qml/SofaBasics/CollapsibleGroupBox.qml
    data/qml/SofaBasics/Vector3DSpinBox.qml
    data/qml/SofaBasics/DynamicSplitView.qml
    data/qml/SofaBasics/DynamicContent.qml
    data/qml/SofaInteractors/qmldir
    data/qml/SofaInteractors/UserInteractor_MoveCamera.qml
    data/qml/SofaInteractors/UserInteractor_Selection.qml
    data/qml/SofaInteractors/UserInteractor_ParticleInteractor.qml
    data/qml/SofaManipulators/qmldir
    data/qml/SofaManipulators/Manipulator.qml
    data/qml/SofaManipulators/Manipulator2D_Translation.qml
    data/qml/SofaManipulators/Manipulator2D_Rotation.qml
    data/qml/SofaManipulators/Manipulator3D_Translation.qml
    data/qml/SofaManipulators/Manipulator3D_Rotation.qml
    data/qml/SofaTools/qmldir
    data/qml/SofaTools/SimulationControl.qml
    data/qml/SofaTools/SceneGraphView.qml
    data/qml/SofaTools/DataListView.qml
    data/qml/SofaTools/SceneToolPanel.qml
    data/qml/SofaWidgets/qmldir
    data/qml/SofaWidgets/Viewer.qml
    data/qml/SofaWidgets/DataView.qml
    data/qml/SofaWidgets/ImageViewer.qml
    data/qml/SofaWidgets/ToolPanel.qml
    data/qml/SofaWidgets/PythonConsole.qml
)

set(CONFIG_FILES
    data/config/default.ini
)

set(PYTHON_FILES
    data/python/BaseHeader.py
    data/python/PythonConsole.py
)

set(QRC_FILES
    data/resources.qrc
    data/qml/qml.qrc
)

# on Window, Qt packages need the glu32 lib full path, but they do not find it without help
if(WIN32)
    if(CMAKE_CL_64)
        set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um/x64")
    else()
        set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um/x86")
    endif()
endif()

if(NOT MSVC)
    add_definitions(-std=gnu++11)
endif()

find_package(Qt5 COMPONENTS Core Gui Widgets Quick Qml REQUIRED PATHS "${SOFA-EXTERNAL_QT5_PATH}")
# make sure to use QT > 5.2

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Gui_INCLUDE_DIRS})
include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${Qt5Qml_INCLUDE_DIRS})
include_directories(${Qt5Quick_INCLUDE_DIRS})

add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5Gui_DEFINITIONS})
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5Qml_DEFINITIONS})
add_definitions(${Qt5Quick_DEFINITIONS})


qt5_wrap_cpp(MOC_FILES ${HEADER_FILES}) # do not use "set(CMAKE_AUTOMOC ON)" since all the mocced files will be compiled by a single compilation unit leading to a "out of heap space" issue on MSVC
qt5_add_resources(RESOURCE_FILES ${QRC_FILES})

add_definitions(-DQT_PLUGIN)

find_package(SofaFramework REQUIRED)
find_package(SofaPython REQUIRED)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${MOC_FILES} ${SOURCE_FILES} ${QRC_FILES} ${RESOURCE_FILES} ${QML_FILES} ${CONFIG_FILES} ${PYTHON_FILES})
target_link_libraries(${PROJECT_NAME} SofaPython SofaSimulationGraph SofaComponentBase Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Qml Qt5::Quick)


target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>")
set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${SOFAQTQUICKGUI_VERSION})

add_definitions("-DSOFA_BUILD_SOFAQTQUICKGUI")

## Install rules for the library; CMake package configurations files
sofa_create_package(${PROJECT_NAME} ${SOFAQTQUICKGUI_VERSION} ${PROJECT_NAME} ${PROJECT_NAME})

## Install rules for the resources
install(DIRECTORY examples/ DESTINATION share/sofa/plugins/${PROJECT_NAME})

## Hand-crafted install rules for the headers, to preserve the directory structure
foreach(HEADER ${HEADER_FILES})
    string(REGEX MATCH "(.*)[/\\]" DIR ${HEADER})
    install(FILES ${HEADER} DESTINATION "include/${PROJECT_NAME}/${DIR}")
endforeach(HEADER HEADERS)
