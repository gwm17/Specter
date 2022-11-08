cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_BUILD_TYPE "Debug")
    message("Building debug")
else()
    message("Building release")
endif()

project(Specter)

option(BUILD_SPECPROJECT "Build SpecProject template" Off)

if(NOT DEFINED SPECTER_BINARY_DIR)
    set(SPECTER_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)
endif()
if(NOT DEFINED SPECTER_LIBRARY_DIR) # optional block, can be repeated
    set(SPECTER_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
endif()

add_subdirectory(Specter)
if(BUILD_SPECPROJECT)
    add_subdirectory(SpecProject)
endif()