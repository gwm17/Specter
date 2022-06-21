add_executable(SpecProject)

target_include_directories(SpecProject
    SYSTEM PUBLIC ../Specter/vendor/asio/asio/include
    PUBLIC ../Specter/vendor/imgui
    ../Specter/vendor/implot
    ../Specter/vendor/glad/include
    ../Specter/vendor/glfw/include
    ../Specter/vendor/glm
    ../Specter/vendor/IconFontCppHeaders
    ../Specter/vendor/spdlog/include
    ../Specter/src/
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

set(THREADS_PREFER_PTHREAD_FLAG On)
find_package(Threads REQUIRED)
target_link_libraries(SpecProject PRIVATE Specter Threads::Threads glad glfw imgui)

if(APPLE)
    target_link_libraries(SpecProject PRIVATE "-framework Cocoa" "-framework CoreVideo" "-framework IOKit" "-framework OpenGL" "-framework Carbon" ${CMAKE_DL_LIBS})
    target_compile_definitions(SpecProject PRIVATE SPEC_APPLE)
elseif(UNIX)
    target_link_libraries(SpecProject PRIVATE libGL.so libX11.so ${CMAKE_DL_LIBS})
    target_compile_definitions(SpecProject PRIVATE SPEC_LINUX)
elseif(MSVC)
    target_link_libraries(SpecProject PRIVATE opengl32.lib)
    target_compile_definitions(SpecProject PRIVATE SPEC_WINDOWS)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(SpecProject PRIVATE SPEC_RELEASE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(SpecProject PRIVATE SPEC_DEBUG)
endif()

set_target_properties(SpecProject PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${SPECTER_BINARY_DIR})

add_custom_command(TARGET SpecProject POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/Assets ${SPECTER_BINARY_DIR}/Assets)
