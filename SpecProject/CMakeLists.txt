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

target_link_libraries(SpecProject PRIVATE Specter)

set_target_properties(SpecProject PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${SPECTER_BINARY_DIR})

add_custom_command(TARGET SpecProject
    POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink
    ${CMAKE_CURRENT_SOURCE_DIR}/Assets
    ${SPECTER_BINARY_DIR}/Assets
)
