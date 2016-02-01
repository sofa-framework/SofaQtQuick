cmake_minimum_required(VERSION 2.8.12)

find_package(SofaFramework REQUIRED)

sofa_add_plugin(applications-dev/plugins/SofaQtQuickGUI SofaQtQuickGUI)

sofa_add_application(applications-dev/projects/qtQuickSofa qtQuickSofa)


find_package(image QUIET)
find_package(SofaQtQuickGUI QUIET)
if(image_FOUND AND SofaQtQuickGUI_FOUND)
    message("Adding automatically image_qtquickgui (image and SofaQtQuickGUI are activated)")
    add_subdirectory(applications-dev/plugins/image_qtquickgui)
endif()

sofa_install_git_version( "sofaqtquick" ${CMAKE_CURRENT_SOURCE_DIR} )
