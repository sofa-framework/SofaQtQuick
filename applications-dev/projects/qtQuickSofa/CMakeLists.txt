cmake_minimum_required(VERSION 2.8.12)
project(qtQuickSofa)

set(SOURCE_FILES
    Main.cpp
)

set(HEADER_FILES
    ""
)

set(QML_FILES
    data/qml/Main.qml
)

set(CONFIG_FILES
    data/config/default.ini
)

set(QRC_FILES
    data/resource.qrc
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

find_package(SofaFramework REQUIRED)
find_package(SofaQtQuickGUI REQUIRED)

qt5_wrap_cpp(MOC_FILES ${HEADER_FILES}) # do not use "set(CMAKE_AUTOMOC ON)" since all the mocced files will be compiled by a single compilation unit leading to a "out of heap space" issue on MSVC
qt5_add_resources(RESOURCE_FILES ${QRC_FILES})

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${MOC_FILES} ${SOURCE_FILES} ${QRC_FILES} ${RESOURCE_FILES} ${QML_FILES} ${CONFIG_FILES})

target_link_libraries(${PROJECT_NAME} SofaQtQuickGUI Qt5::Core Qt5::Gui Qt5::Widgets Qt5::QuickControls2 Qt5::Qml Qt5::Quick)

sofa_install_targets(SofaGui ${PROJECT_NAME} "")

