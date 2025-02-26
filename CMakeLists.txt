# Copyright (C) 2021 Intel Corporation
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.12 FATAL_ERROR)
project(Dml VERSION 0.1.9 LANGUAGES C CXX)

set(PROJECT_SOVERSION 0)

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    option(DML_RECORD_SWITCHES "Enables -frecord-gcc-switches flag" OFF)
else()
    set(DML_RECORD_SWITCHES OFF CACHE BOOL "Disable recording of switches for non-gnu compiler" FORCE)
endif()

# TODO: Remove all options below
option(SANITIZE_MEMORY "Enables memory sanitizing" OFF)
option(SANITIZE_THREADS "Enables threads sanitizing" OFF)
option(LOG_HW_INIT "Enables HW initialization log" OFF)
option(EFFICIENT_WAIT "Enables usage of umonitor/umwait" OFF)

if (SANITIZE_MEMORY)
    message(STATUS "Memory sanitizing build is enabled")

    if (WIN32)
        message(FATAL_ERROR "Memory sanitizing is not supported in Windows")
    else ()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -fsanitize=leak -g")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=leak -g")
    endif ()
endif ()

if (SANITIZE_THREADS)
    message(STATUS "Threads sanitizing build is enabled")

    if (WIN32)
        message(FATAL_ERROR "Threads sanitizing is not supported in Windows")
    else ()
        set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
        set(THREADS_PREFER_PTHREAD_FLAG TRUE)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=thread -g")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread -g")
    endif ()
endif ()

include(cmake/tests.cmake)
include(cmake/CompileOptions.cmake)
include(cmake/utils.cmake)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "CMAKE_BUILD_TYPE is unset, defaulting to Release")
    set(CMAKE_BUILD_TYPE Release)
endif ()

if (CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    include(cmake/msvc.cmake)
elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    include(cmake/gnu.cmake)
else ()
    message(FATAL_ERROR "Unknown compiler ID: ${CMAKE_C_COMPILER_ID}")
endif ()

# Project-wide settings
add_compile_options(
    ${DML_SECURITY_OPTIONS}
    $<$<BOOL:${DML_RECORD_SWITCHES}>:-frecord-gcc-switches>)
add_compile_definitions(${DML_SECURITY_DEFINITIONS})

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Hardware initialization logging: ${LOG_HW_INIT}")

# TODO: Remove
get_git_revision()

add_subdirectory(sources)

# Dependencies
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "Suppress gmock build" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "Suppress gtest installation" FORCE)
add_subdirectory(google-test)

# Testing
add_subdirectory(tests)
add_subdirectory(examples)

# Install rules
install(EXPORT ${PROJECT_NAME}Targets
        NAMESPACE Dml::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
    "include(\${CMAKE_CURRENT_LIST_DIR}/${PROJECT_NAME}Targets.cmake)\n")

write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN *.h PATTERN *.hpp)
