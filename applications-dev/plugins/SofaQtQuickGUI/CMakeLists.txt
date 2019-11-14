cmake_minimum_required(VERSION VERSION 3.14)
project(SofaQtQuickGUI VERSION 1.0)

set (CMAKE_CXX_STANDARD 17)

find_package(SofaFramework REQUIRED)
find_package(SofaOpenglVisual REQUIRED)
find_package(SofaGeneral REQUIRED)
find_package(SofaCommon REQUIRED)
find_package(QmlUI REQUIRED)

find_package(SofaPython3 QUIET)
if (SofaPython3_FOUND)
    set (SOFAPYTHON_LIBRARY SofaPython3)
    include_directories(${SofaPython3_INCLUDE_DIRS})
    #  add_definitions("-DSOFAQTQUICK_WITH_SOFAPYTHON3")
else()
    message(FATAL_ERROR "SofaPython3 is required to compile SofaQtQuick")
endif ()

#NodeEditor
#option(SOFAGUIQT_ENABLE_NODEGRAPH "Build the Node graph representation. NodeEditor library is needed." OFF)
#if (SOFAGUIQT_ENABLE_NODEGRAPH)
set(NODEEDITOR_LIBRARY "/usr/local/lib/libnodes.so")
set(NODEEDITOR_INCLUDE_PATH "/usr/local/include")
find_package(NodeEditor REQUIRED)
#endif()
#sofa_set_01(SOFAGUIQT_HAVE_NODEEDITOR VALUE ${SOFAGUIQT_ENABLE_NODEGRAPH} BOTH_SCOPES)


set(SOURCE_FILES
    src/SofaQtQuickGUI/DataHelper.cpp
    src/SofaQtQuickGUI/SofaQtQuickQmlModule.cpp
    src/SofaQtQuickGUI/SofaBaseApplication.cpp
    src/SofaQtQuickGUI/SofaViewer.cpp
    src/SofaQtQuickGUI/Windows/CameraView.cpp
    src/SofaQtQuickGUI/Windows/EditView.cpp
    src/SofaQtQuickGUI/Windows/AssetView.cpp
    src/SofaQtQuickGUI/Camera.cpp
    src/SofaQtQuickGUI/SofaBaseScene.cpp
    src/SofaQtQuickGUI/SofaComponentList.cpp
    src/SofaQtQuickGUI/Selectable.cpp
    src/SofaQtQuickGUI/SelectableManipulator.cpp
    src/SofaQtQuickGUI/SelectableSofaComponent.cpp
    src/SofaQtQuickGUI/SelectableSofaParticle.cpp
    src/SofaQtQuickGUI/Console.cpp
    src/SofaQtQuickGUI/SofaParticleInteractor.cpp
    src/SofaQtQuickGUI/SyntaxHighlight/QSyntaxHighlighterPython.cpp
    src/SofaQtQuickGUI/SyntaxHighlight/QSyntaxHighlighterCpp.cpp
    src/SofaQtQuickGUI/SyntaxHighlight/HighlightComponent.cpp
    src/SofaQtQuickGUI/Bindings/SofaFactory.cpp
    src/SofaQtQuickGUI/Bindings/SofaCamera.cpp
    src/SofaQtQuickGUI/Bindings/SofaComponent.cpp
    src/SofaQtQuickGUI/Bindings/SofaBase.cpp
    src/SofaQtQuickGUI/Bindings/SofaBaseObject.cpp
    src/SofaQtQuickGUI/Bindings/SofaNode.cpp
    src/SofaQtQuickGUI/Bindings/SofaData.cpp
    src/SofaQtQuickGUI/Bindings/SofaLink.cpp
    src/SofaQtQuickGUI/Bindings/SofaCoreBindingContext.cpp
    src/SofaQtQuickGUI/Bindings/SofaCoreBindingFactory.cpp
    src/SofaQtQuickGUI/Helper/QGTest.cpp
    src/SofaQtQuickGUI/Helper/QMathExtensions.cpp
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
    src/SofaQtQuickGUI/Models/SofaDataContainerListModel.cpp
    src/SofaQtQuickGUI/Models/SofaLinkCompletionModel.cpp
    #    src/SofaQtQuickGUI/Models/CustomInspectorModel.cpp
    src/SofaQtQuickGUI/Modules/SofaRuntime/SofaRuntimeModule.cpp
    src/SofaQtQuickGUI/QMLUI/QMLUI.cpp
    src/SofaQtQuickGUI/QMLUI/View.cpp
    src/SofaQtQuickGUI/Rendering/ObjectRenderer.cpp
    src/SofaQtQuickGUI/Rendering/SceneRenderer.cpp
    src/SofaQtQuickGUI/LiveFileMonitor.cpp
    src/SofaQtQuickGUI/LiveQMLFileMonitor.cpp
    src/SofaQtQuickGUI/SofaProject.cpp
    src/SofaQtQuickGUI/Assets/Asset.cpp
    src/SofaQtQuickGUI/Assets/DirectoryAsset.cpp
    src/SofaQtQuickGUI/Assets/FileAsset.cpp
    src/SofaQtQuickGUI/Assets/MeshAsset.cpp
    src/SofaQtQuickGUI/Assets/PythonAsset.cpp
    src/SofaQtQuickGUI/Assets/SceneAsset.cpp
    src/SofaQtQuickGUI/Assets/TextureAsset.cpp
    src/SofaQtQuickGUI/Assets/CanvasAsset.cpp
    src/SofaQtQuickGUI/Assets/AssetFactory.cpp
    )

if (SofaPython_FOUND)
    list(APPEND SOURCE_FILES
        src/SofaQtQuickGUI/PythonConsole.cpp
        # src/SofaQtQuickGUI/SofaPythonInteractor.cpp
        )
else ()
    list(APPEND SOURCE_FILES
        src/SofaQtQuickGUI/Python3Console.cpp
        src/SofaQtQuickGUI/SofaQtQuick_PythonEnvironment.cpp
        # src/SofaQtQuickGUI/SofaPython3Interactor.cpp
        )
endif ()

set(HEADER_FILES
    src/SofaQtQuickGUI/config.h
    src/SofaQtQuickGUI/DataHelper.h
    src/SofaQtQuickGUI/SofaQtQuickQmlModule.h
    src/SofaQtQuickGUI/SofaBaseApplication.h
    src/SofaQtQuickGUI/ProcessState.h
    src/SofaQtQuickGUI/SofaViewer.h
    src/SofaQtQuickGUI/Windows/CameraView.h
    src/SofaQtQuickGUI/Windows/EditView.h
    src/SofaQtQuickGUI/Windows/AssetView.h
    src/SofaQtQuickGUI/Camera.h
    src/SofaQtQuickGUI/SofaBaseScene.h
    src/SofaQtQuickGUI/SofaComponentList.h
    src/SofaQtQuickGUI/Selectable.h
    src/SofaQtQuickGUI/SelectableManipulator.h
    src/SofaQtQuickGUI/SelectableSofaComponent.h
    src/SofaQtQuickGUI/SelectableSofaParticle.h
    # src/SofaQtQuickGUI/SofaPythonInteractor.h
    src/SofaQtQuickGUI/Console.h
    src/SofaQtQuickGUI/PythonConsole.h
    src/SofaQtQuickGUI/SofaParticleInteractor.h
    src/SofaQtQuickGUI/SyntaxHighlight/QSyntaxHighlighterPython.h
    src/SofaQtQuickGUI/SyntaxHighlight/QSyntaxHighlighterCpp.h
    src/SofaQtQuickGUI/SyntaxHighlight/HighlightComponent.h
    src/SofaQtQuickGUI/Bindings/SofaFactory.h
    src/SofaQtQuickGUI/Bindings/SofaComponent.h
    src/SofaQtQuickGUI/Bindings/SofaCamera.h
    src/SofaQtQuickGUI/Bindings/SofaBase.h
    src/SofaQtQuickGUI/Bindings/SofaBaseObject.h
    src/SofaQtQuickGUI/Bindings/SofaNode.h
    src/SofaQtQuickGUI/Bindings/SofaData.h
    src/SofaQtQuickGUI/Bindings/SofaLink.h
    src/SofaQtQuickGUI/Bindings/SofaCoreBindingContext.h
    src/SofaQtQuickGUI/Bindings/SofaCoreBindingFactory.h
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
    src/SofaQtQuickGUI/Models/SofaDataContainerListModel.h
    src/SofaQtQuickGUI/Models/SofaLinkCompletionModel.h
    #    src/SofaQtQuickGUI/Models/CustomInspectorModel.h
    src/SofaQtQuickGUI/Modules/SofaRuntime/SofaRuntimeModule.h
    src/SofaQtQuickGUI/QMLUI/QMLUI.h
    src/SofaQtQuickGUI/QMLUI/View.h
    src/SofaQtQuickGUI/Rendering/ObjectRenderer.h
    src/SofaQtQuickGUI/Rendering/SceneRenderer.h
    src/SofaQtQuickGUI/Helper/QMathExtensions.h
    src/SofaQtQuickGUI/Helper/QGTest.h
    src/SofaQtQuickGUI/Helper/QtStl/QtStl.h
    src/SofaQtQuickGUI/LiveFileMonitor.h
    src/SofaQtQuickGUI/LiveQMLFileMonitor.h
    src/SofaQtQuickGUI/SofaProject.h
    src/SofaQtQuickGUI/Assets/Asset.h
    src/SofaQtQuickGUI/Assets/DirectoryAsset.h
    src/SofaQtQuickGUI/Assets/FileAsset.h
    src/SofaQtQuickGUI/Assets/MeshAsset.h
    src/SofaQtQuickGUI/Assets/PythonAsset.h
    src/SofaQtQuickGUI/Assets/TextureAsset.h
    src/SofaQtQuickGUI/Assets/SceneAsset.h
    src/SofaQtQuickGUI/Assets/CanvasAsset.h
    src/SofaQtQuickGUI/Assets/AssetFactory.h
    src/SofaQtQuickGUI/Assets/AssetFactory.inl
    )

if (SofaPython3_FOUND)
    list(APPEND HEADER_FILES src/SofaQtQuickGUI/SofaQtQuick_PythonEnvironment.h)
endif ()

list(APPEND HEADER_FILES
    src/SofaQtQuickGUI/Windows/GraphView.h
    src/SofaQtQuickGUI/Windows/ProfilerView.h
    src/SofaQtQuickGUI/Models/SofaComponentGraphModel.h
    )

list(APPEND SOURCE_FILES
    src/SofaQtQuickGUI/Windows/GraphView.cpp
    src/SofaQtQuickGUI/Windows/ProfilerView.cpp
    src/SofaQtQuickGUI/Models/SofaComponentGraphModel.cpp
    )

set(ASSET_TEMPLATES
    data/config/templates/MeshAsset.py
    data/config/templates/PythonAsset.py
    data/python/templates/emptyController.py
    data/python/templates/emptyDataEngine.py
    data/python/templates/emptyForceField.py
    )



add_definitions("-DSOFAQTQUICKGUI_QML_DIR=\"${CMAKE_CURRENT_LIST_DIR}/data/qml/\"")

list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/GTest/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaDataTypes/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaApplication/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaBasics/")
list(APPEND QML_DIRS "${CMAKE_CURRENT_LIST_DIR}/data/qml/SofaColorScheme/")
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

set(EXTRA_FILES
    src/SofaQtQuickGUI/QMLUI/Readme.md
    )

set(PYTHON_FILES
    data/python/BaseHeader.py
    data/python/PythonConsole.py
    data/python/SofaQtQuick.py
    )
sofa_install_pythonscripts(PLUGIN_NAME ${PROJECT_NAME} PYTHONSCRIPTS_SOURCE_DIR "data/python")

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

find_package(Qt5 COMPONENTS Core Gui Widgets Quick QuickControls2 Qml WebEngine  Charts REQUIRED PATHS "${SOFA-EXTERNAL_QT5_PATH}")


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

#find_package(ASSIMP REQUIRED)
#message(STATUS "Found ASSIMP in ${ASSIMP_INCLUDE_DIR}")
#include_directories( ${ASSIMP_INCLUDE_DIR})

if (WIN32)
    # Does not work with Visual Studio config generation
    #if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DQT_NO_DEBUG")
endif()

qt5_wrap_cpp(MOC_FILES ${HEADER_FILES}) # do not use "set(CMAKE_AUTOMOC ON)" since all the mocced files will be compiled by a single compilation unit leading to a "out of heap space" issue on MSVC
qt5_add_resources(RESOURCE_FILES ${QRC_FILES})

# We need to put the .inl after the call of qt5_wrap_cpp to avoid having the inline mocced.
set(HEADER_FILES ${HEADER_FILES}
    src/SofaQtQuickGUI/SofaParticleInteractor.inl
    )

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${MOC_FILES} ${SOURCE_FILES} ${QRC_FILES} ${RESOURCE_FILES} ${QML_FILES} ${CONFIG_FILES} ${PYTHON_FILES} ${EXTRA_FILES} ${ASSET_TEMPLATES})
target_link_libraries(${PROJECT_NAME} PUBLIC ${SOFAPYTHON_LIBRARY} SofaOpenglVisual SofaGeneralLoader SofaLoader SofaSimulationGraph SofaGraphComponent SofaBoundaryCondition SofaComponentBase SofaMeshCollision
    Qt5::Core Qt5::Gui Qt5::Widgets Qt5::QuickControls2 Qt5::Qml Qt5::Quick Qt5::WebEngine Qt5::Charts stdc++fs QmlUI)
#target_link_libraries(${PROJECT_NAME} PRIVATE assimp)

target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")
target_include_directories(${PROJECT_NAME} PRIVATE "$<BUILD_INTERFACE:${ASSIMP_INCLUDE_DIR}>")


target_link_libraries(${PROJECT_NAME} PUBLIC ${NODEEDITOR_LIBRARY})
sofa_install_libraries(LIBRARIES ${NODEEDITOR_LIBRARY})
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${NODEEDITOR_INCLUDE_PATH}>")

file(COPY ${ASSET_TEMPLATES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/templates)
install(FILES ${ASSET_TEMPLATES} DESTINATION bin/config/templates)

target_compile_definitions(${PROJECT_NAME} PRIVATE "-DSOFA_BUILD_SOFAQTQUICKGUI")
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DQT_QML_DEBUG")
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DQMLJSDEBUGGER")

##TODO(dmarchal) A virer et remplacer par des vrai resources.
target_compile_definitions(${PROJECT_NAME} PUBLIC "-DSOFAQTQUICK_DIRECTORY_VIEW=\"${CMAKE_CURRENT_SOURCE_DIR}/data/qml/SofaViews\"")

if(NOT MSVC)
    target_compile_options(${PROJECT_NAME} PUBLIC "$<$<STREQUAL:$<TARGET_PROPERTY:LINKER_LANGUAGE>,CXX>:${CXX11_FLAG}>")
endif()
#if(WIN32)
#        sofa_copy_libraries(${ASSIMP_DLL})
#endif()
#sofa_install_libraries(${ASSIMP_DLL})

set(SOFAQTQUICKGUI_VERSION 1.0)
## Install rules for the library; CMake package configurations files
sofa_generate_package(NAME ${PROJECT_NAME} VERSION ${SOFAQTQUICKGUI_VERSION} TARGETS ${PROJECT_NAME} INCLUDE_INSTALL_DIR ${PROJECT_NAME} INCLUDE_SOURCE_DIR "src/${PROJECT_NAME}")

if(SOFA_BUILD_TESTS)
    find_package(SofaTest QUIET)
    if(SofaTest_FOUND)
        add_subdirectory(SofaQtQuickGUI_test)
    endif()
endif()
