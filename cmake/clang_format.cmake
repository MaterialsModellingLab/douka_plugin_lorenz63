# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

find_program(CLANG_FORMAT_BIN NAMES clang-format)

if (CLANG_FORMAT_BIN)
  message(STATUS "Found: ${CLANG_FORMAT_BIN}")
  file(GLOB_RECURSE CPP_FILES
    ${PROJECT_SOURCE_DIR}/*.cc
    ${PROJECT_SOURCE_DIR}/*.hh)

  add_custom_target(
    format
    COMMENT "Running clang-format..."
    COMMAND ${CLANG_FORMAT_BIN} --style=file -i ${CPP_FILES})
endif()

