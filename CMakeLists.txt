# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.27 FATAL_ERROR)
project(lorenz63 VERSION 0.0.0 LANGUAGES CXX DESCRIPTION "Data Assimilation Plugin For Lorenz63")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local/" CACHE PATH "Default install path" FORCE)
endif()

# Define executable name for different platforms
set(DOUKA_PLUGIN_NAME "${PROJECT_NAME}")
set(DOUKA_RUNTIME_NAME "${PROJECT_NAME}-exec")

# CMake Local Module Imports
include(${CMAKE_SOURCE_DIR}/cmake/clang_format.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/cppcheck.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/dependencies.cmake)

set(TARGET ${DOUKA_PLUGIN_NAME})
add_library(${TARGET} SHARED)
target_sources(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/src/${PROJECT_NAME}.cc)
target_link_libraries(${TARGET} PUBLIC douka::plugin_interface)

set(TARGET ${DOUKA_RUNTIME_NAME})
add_executable(${TARGET})
target_sources(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/src/main.cc)
target_link_libraries(${TARGET} PRIVATE ${DOUKA_PLUGIN_NAME})

douka_plugin_install(${DOUKA_PLUGIN_NAME})
douka_plugin_status(${DOUKA_PLUGIN_NAME})
