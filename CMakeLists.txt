cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_BUILD_TYPE "Debug")
    message("Building debug")
else()
    message("Building release")
endif()

project(Specter)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Specter)