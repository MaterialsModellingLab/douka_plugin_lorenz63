# Copyright (c) 2025 Materials Modelling Lab, The University of Tokyo
# SPDX-License-Identifier: Apache-2.0

find_program(CPPCHECK_BIN NAMES cppcheck)

if (CPPCHECK_BIN)
  message(STATUS "Found: ${CPPCHECK_BIN}")
  list( APPEND CMAKE_CXX_CPPCHECK
      "${CPPCHECK_BIN}"
      "--enable=warning"
      "--check-level=exhaustive"
      "--std=c++17"
      "--suppress=unusedFunction"
      "--suppress=missingIncludeSystem")
endif ()