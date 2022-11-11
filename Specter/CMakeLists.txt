add_subdirectory(vendor/glad)
set(GLFW_INSTALL Off CACHE BOOL "Generate installation target" )
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/imgui)

set(YAML_CPP_BUILD_TOOLS Off CACHE BOOL "Enable parse tools")
add_subdirectory(vendor/yaml-cpp)

set(GRIMOIRE_ASIO_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vendor/asio/asio/include)
add_subdirectory(vendor/DaqGrimoire)

add_subdirectory(src)
