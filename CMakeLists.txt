cmake_minimum_required(VERSION 3.0.0)

find_package(SofaFramework REQUIRED)

add_subdirectory(modules/SofaQtQuickGUI SofaQtQuickGUI)

# TODO(@bmarques): move image & compliant code to their respective plugins
find_package(image QUIET)
find_package(Compliant QUIET)
if(image_FOUND AND SofaQtQuickGUI_FOUND)
    message("Adding (auto) image_qtquickgui (image and SofaQtQuickGUI are activated)")
    add_subdirectory(modules/image_qtquickgui)
endif()

if(Compliant_FOUND AND SofaQtQuickGUI_FOUND)
    message("Adding (auto) compliant_qtquickgui (since Compliant and SofaQtQuickGUI are activated)")
    add_subdirectory(modules/compliant_qtquickgui)
endif()

#optional application runSofa2
sofa_add_application(applications/runSofa2 runSofa2)
