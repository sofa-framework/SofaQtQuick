cmake_minimum_required(VERSION 3.1)
project(SofaPython3_SofaApplication)

find_package(SofaPython3 REQUIRED)
unset(PYTHON_EXECUTABLE)
find_package(Python 3.7 COMPONENTS Interpreter Development REQUIRED)
set(PYTHON_INCLUDE_DIRS ${Python_INCLUDE_DIR})
set(PYTHON_EXECUTABLE ${Python_EXECUTABLE})
set(PYBIND11_PYTHON_VERSION ${Python_VERSION})
find_package(pybind11 QUIET)
if(NOT pybind11_FOUND)#
  message("-- Cannot build the python bindings because pybind is missing.")
  return()
endif()


set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Binding_SofaApplication.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Binding_SofaApplication_doc.h
)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Binding_SofaApplication.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Module_SofaApplication.cpp
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(MODULE_NAME SofaApplication)
set(PACKAGE_DIRECTORY  ${SP3_PYTHON_PACKAGES_DIRECTORY})

SP3_add_python_package(
        SOURCE_DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}/package
        TARGET_DIRECTORY
            ${PACKAGE_DIRECTORY}
)

SP3_add_python_module(
    TARGET
        ${PROJECT_NAME}
    MODULE_NAME
        ${MODULE_NAME}
    SOURCES
        ${SOURCE_FILES}
        ${HEADER_FILES}
    DEPENDS
        SofaCore SofaPython3 SofaPython3_Sofa_Core SofaQtQuickGUI
    DESTINATION
        ${PACKAGE_DIRECTORY}
)
