cmake_minimum_required(VERSION 2.8.12)

find_package(SofaFramework REQUIRED)

list(APPEND SOFAQTQUICKGUI_TARGETS SofaQtQuickGUI)
list(APPEND SOFAQTQUICKGUI_TARGETS qtQuickSofa)

sofa_add_plugin(applications-dev/plugins/SofaQtQuickGUI SofaQtQuickGUI)
sofa_add_application(applications-dev/projects/qtQuickSofa qtQuickSofa)

set(SOFAQTQUICKGUI_TARGETS "")

find_package(image QUIET)
find_package(SofaQtQuickGUI QUIET)
# if(PLUGIN_IMAGE AND PLUGIN_SOFAQTQUICKGUI)
if(image_FOUND AND SofaQtQuickGUI_FOUND)
    list(APPEND SOFAQTQUICKGUI_TARGETS image_qtquickgui)
    message("Adding automatically image_qtquickgui (image and SofaQtQuickGUI are activated)")
    add_subdirectory(applications-dev/plugins/image_qtquickgui)
endif()

## Version
set(SOFAQTQUICKGUI_VERSION "1.0")

## IDE folder
foreach(TARGET ${SOFAQTQUICKGUI_TARGETS})
    set_target_properties(${TARGET} PROPERTIES FOLDER "SofaQtQuickGUI")
    target_include_directories(${TARGET} PUBLIC "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>")
    target_include_directories(${TARGET} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../applications>")
    # target_include_directories(${TARGET} PUBLIC "$<BUILD_INTERFACE:${QT_INCLUDES}>")
    set_target_properties(${TARGET} PROPERTIES DEBUG_POSTFIX "_d")
    set_target_properties(${TARGET} PROPERTIES VERSION ${SOFAQTQUICKGUI_VERSION})
endforeach()

sofa_install_git_version( "sofaqtquick" ${CMAKE_CURRENT_SOURCE_DIR} )
