cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_BUILD_TYPE "Debug")
    message("Building debug")
else()
    message("Building release")
endif()

project(SpecProject)

set(SPECPROJECT_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../bin)

#Use this to find Specter and its libraries. Specify the path to the Specter/lib dir after the PATHS keyword
find_package(Specter REQUIRED PATHS ../lib)

add_executable(SpecProject)

target_include_directories(SpecProject PUBLIC
    ./src
)

target_sources(SpecProject PRIVATE
    ./src/main.cpp
    ./src/MassMap.cpp
    ./src/MassMap.h
    ./src/SPSAnalysisStage.cpp
    ./src/SPSAnalysisStage.h
    ./src/SPSInputLayer.cpp
    ./src/SPSInputLayer.h
)

target_link_libraries(SpecProject PRIVATE Specter::Specter)

set_target_properties(SpecProject PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${SPECPROJECT_BINARY_DIR})

if(NOT EXISTS "${SPECPROJECT_BINARY_DIR}/Assets")
    add_custom_command(TARGET SpecProject
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/Assets
        ${SPECPROJECT_BINARY_DIR}/Assets
    )
endif()
