cmake_minimum_required(VERSION 2.8.12)

find_package(SofaFramework REQUIRED)

sofa_add_plugin(applications-dev/plugins/SofaQtQuickGUI SofaQtQuickGUI)

sofa_add_plugin(applications-dev/plugins/image_qtquickgui image_qtquickgui)

sofa_add_application(applications-dev/projects/qtQuickSofa qtQuickSofa)

sofa_install_git_version( "sofaqtquick" ${CMAKE_CURRENT_SOURCE_DIR} )
