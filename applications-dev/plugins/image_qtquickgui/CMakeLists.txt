cmake_minimum_required(VERSION 2.8.12)
project(image_qtquickgui)

set(SOURCE_FILES
    ImagePlaneModel.cpp
    ImagePlaneView.cpp
    ImageQtQuickGUI.cpp
)

set(MOC_FILES
    ImagePlaneModel.h
    ImagePlaneView.h
)

set(HEADER_FILES
    ImageQtQuickGUI.h
)

set(QML_FILES
    data/qml/SofaDataTypes/qmldir
    data/qml/SofaDataTypes/SofaDataType_imageplane.qml
    data/qml/SofaInteractors/qmldir
    data/qml/SofaInteractors/UserInteractor_Image.qml
)

set(QRC_FILES
    data/qml/qml.qrc
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# on Window, Qt packages need the glu32 lib full path, but they do not find it without help
if(WIN32)
    if(CMAKE_CL_64)
        set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um/x64")
    else()
        set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "C:/Program Files (x86)/Windows Kits/8.0/Lib/win8/um/x86")
    endif()

    # to fix a bug when one is compiling a debug version of the code
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-DQT_NO_DEBUG)
    endif()
endif()

if(NOT MSVC)
    add_definitions(-std=gnu++11)
endif()

find_package(Qt5 COMPONENTS Core Gui Widgets Quick Qml REQUIRED PATHS "${SOFA-EXTERNAL_QT5_PATH}")
# make sure to use QT > 5.0

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

find_package(SofaFramework REQUIRED)
find_package(image REQUIRED)
find_package(SofaQtQuickGUI REQUIRED)

qt5_wrap_cpp(MOC_FILES ${MOC_FILES}) # do not use "set(CMAKE_AUTOMOC ON)" since all the mocced files will be compiled by a single compilation unit leading to a "out of heap space" issue on MSVC
qt5_add_resources(RESOURCE_FILES ${QRC_FILES})

add_definitions(-DQT_PLUGIN)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${MOC_FILES} ${SOURCE_FILES} ${QRC_FILES} ${RESOURCE_FILES} ${QML_FILES} ${CONFIG_FILES})

find_package(CImg REQUIRED)
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CImg_INCLUDE_DIRS}>")

if(APPLE)
    find_package(X11)
    if(NOT X11_FOUND)
        message(FATAL_ERROR "Failed to find X11 which is required to build plugin 'image'")
    endif()
    target_link_libraries(${PROJECT_NAME} ${X11_X11_LIB})
    target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${X11_X11_INCLUDE_PATH}>")
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-DSOFA_BUILD_IMAGE_QTQUICKGUI")

target_link_libraries(${PROJECT_NAME} image SofaQtQuickGUI)

sofa_install_targets(SofaGui ${PROJECT_NAME} "")
