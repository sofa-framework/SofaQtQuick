cmake_minimum_required(VERSION 3.14)
project(QmlUI)

set(QMLUI_MAJOR_VERSION 1)
set(QMLUI_MINOR_VERSION 0)
set(QMLUI_VERSION ${QMLUI_MAJOR_VERSION}.${QMLUI_MINOR_VERSION})

set (CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES
    src/QmlUI/Canvas.cpp
    src/QmlUI/initQmlUI.cpp
)

set(HEADER_FILES
    src/QmlUI/Canvas.h
    src/QmlUI/initQmlUI.h
)

set(QML_TEMPLATES
    data/qml/emptyCanvas.qml
)

find_package(SofaFramework REQUIRED)
find_package(SofaBase REQUIRED)

add_library(${PROJECT_NAME} SHARED ${HEADER_FILES} ${SOURCE_FILES} ${QML_TEMPLATES})

target_link_libraries(${PROJECT_NAME} PUBLIC SofaCore SofaBaseVisual)

target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>")
target_include_directories(${PROJECT_NAME} PUBLIC "$<INSTALL_INTERFACE:include/${PROJECT_NAME}>")

file(COPY ${QML_TEMPLATES} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/config/templates)
install(FILES ${QML_TEMPLATES} DESTINATION bin/config/templates)

target_compile_definitions(${PROJECT_NAME} PRIVATE "-DSOFA_BUILD_QMLUI")

## Install rules for the library; CMake package configurations files
sofa_generate_package(NAME ${PROJECT_NAME} VERSION ${QMLUI_VERSION} TARGETS ${PROJECT_NAME} INCLUDE_INSTALL_DIR ${PROJECT_NAME} INCLUDE_SOURCE_DIR src/${PROJECT_NAME})
