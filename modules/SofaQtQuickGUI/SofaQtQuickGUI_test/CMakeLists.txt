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

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME} SofaTest SofaQtQuickGUI SofaGTestMain Qt5::Test)
set_target_properties(${PROJECT_NAME} PROPERTIES AUTOMOC TRUE)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
