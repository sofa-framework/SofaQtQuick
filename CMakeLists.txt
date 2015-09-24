cmake_minimum_required(VERSION 2.8.12)

find_package(SofaFramework REQUIRED)
sofa_add_plugin(applications-dev/plugins/SofaQtQuickGUI SofaQtQuickGUI)

## GUI Applications
option(SOFAGUI_QTQUICKSOFA "Build the qtQuickSofa application" OFF)
if(SOFAGUI_QTQUICKSOFA)
    add_subdirectory(applications-dev/projects/qtQuickSofa ${CMAKE_CURRENT_BINARY_DIR}/qtQuickSofa)
    list(APPEND SOFAGUI_TARGETS qtQuickSofa)
endif()
