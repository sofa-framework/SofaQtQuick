cmake_minimum_required(VERSION 3.1)

project(SofaQtQuickGUI_test)
find_package(Qt5Test REQUIRED)

set(HEADER_FILES
    modeltest.h
    SofaScene_test.h
    SofaSceneModel_test.h
)

set(SOURCE_FILES
    modeltest.cpp
    SofaApplication_test.cpp
    SofaScene_test.cpp
    SofaSceneModel_test.cpp
)

add_definitions("-DSOFAQTQUICK_TEST_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")

qt5_wrap_cpp(MOC_FILES ${HEADER_FILES}) # do not use "set(CMAKE_AUTOMOC ON)" since all the mocced files will be compiled by a single compilation unit leading to a "out of heap space" issue on MSVC

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${MOC_FILES})
target_link_libraries(${PROJECT_NAME} SofaTest SofaQtQuickGUI SofaGTestMain Qt5::Test)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
