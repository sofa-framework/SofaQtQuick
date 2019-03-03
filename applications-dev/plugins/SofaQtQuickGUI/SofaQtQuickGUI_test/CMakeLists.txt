cmake_minimum_required(VERSION 3.1)

project(SofaQtQuickGUI_test)
find_package(SofaFramework REQUIRED)
find_package(SofaTest REQUIRED)
find_package(SofaGTestMain REQUIRED)
find_package(Qt5Test REQUIRED)
find_package(Qt5QuickTest REQUIRED)


set(HEADER_FILES
    modeltest.h
    QmlGTest.h
    SofaBase_test.h
    SofaScene_test.h
    SofaSceneModel_test.h
)

set(SOURCE_FILES
    main.cpp
    QmlGTest.cpp
    modeltest.cpp
    SofaBase_test.cpp
    SofaApplication_test.cpp
    SofaScene_test.cpp
    SofaSceneModel_test.cpp
)

set(QMLTEST_FILES
    data/qmltests/SofaBase_test.qml
    data/qmltests/SofaNode_test.qml
    data/qmltests/SofaData_test.qml
    )

add_definitions("-DSOFAQTQUICK_TEST_DIR=\"${CMAKE_CURRENT_SOURCE_DIR}/data\"")

set(MOC_FILES)
qt5_wrap_cpp(MOC_FILES ${HEADER_FILES}) # do not use "set(CMAKE_AUTOMOC ON)" since all the mocced files will be compiled by a single compilation unit leading to a "out of heap space" issue on MSVC

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${MOC_FILES} ${QMLTEST_FILES})
target_link_libraries(${PROJECT_NAME} SofaCore SofaQtQuickGUI Qt5::Test Qt5::QuickTest)

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})
