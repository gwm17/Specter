add_library(Specter STATIC)

target_include_directories(Specter
    SYSTEM PUBLIC ../vendor/asio/asio/include
    PUBLIC ../vendor/imgui
    ../vendor/implot
    ../vendor/glad/include
    ../vendor/glfw/include
    ../vendor/glm
    ../vendor/IconFontCppHeaders
    ../vendor/spdlog/include
    ./
)

target_precompile_headers(Specter PRIVATE specpch.h)

#Specter sources
target_sources(Specter PRIVATE
    Specter/Core/Application.h
    Specter/Core/Application.cpp
    Specter/Core/Cut.cpp 
    Specter/Core/Graph.cpp 
    Specter/Core/Histogram.cpp
    Specter/Core/Layer.cpp
    Specter/Core/LayerStack.cpp
    Specter/Core/Logger.cpp
    Specter/Core/Parameter.cpp
    Specter/Core/SpecCore.h
    Specter/Core/SpectrumManager.h
    Specter/Core/SpectrumSerializer.h
    Specter/Core/Window.h
    Specter/Core/Cut.h
    Specter/Core/Graph.h
    Specter/Core/Histogram.h
    Specter/Core/Layer.h
    Specter/Core/LayerStack.h
    Specter/Core/Logger.h
    Specter/Core/Parameter.h
    Specter/Core/SpectrumManager.cpp
    Specter/Core/SpectrumSerializer.cpp
    Specter/Core/Timestep.h
    Specter/Events/AppEvent.h
    Specter/Events/Event.h
    Specter/Events/KeyEvent.h
    Specter/Events/MouseEvent.h
    Specter/Events/PhysicsEvent.h
    Specter/Renderer/GraphicsContext.h
    Specter/Renderer/RenderCommand.cpp
    Specter/Renderer/RenderCommand.h
    Specter/Renderer/RendererAPI.cpp
    Specter/Renderer/RendererAPI.h
    Specter/ImGui/ImGuiBuild.cpp
    Specter/ImGui/ImGuiExtensions.cpp
    Specter/ImGui/ImGuiLayer.cpp
    Specter/ImGui/ImGuiLayer.h
    Specter/Physics/AnalysisStack.cpp
    Specter/Physics/AnalysisStage.cpp
    Specter/Physics/DataSource.h
    Specter/Physics/PhysicsEventBuilder.h
    Specter/Physics/PhysicsLayer.h
    Specter/Physics/ShiftMap.h
    Specter/Physics/AnalysisStack.h
    Specter/Physics/AnalysisStage.h
    Specter/Physics/DataSource.cpp
    Specter/Physics/PhysicsEventBuilder.cpp
    Specter/Physics/PhysicsLayer.cpp
    Specter/Physics/ShiftMap.cpp
    Specter/Physics/SpecData.h
    Specter/Physics/Caen/CompassFile.cpp
    Specter/Physics/Caen/CompassFile.h
    Specter/Physics/Caen/CompassHit.cpp
    Specter/Physics/Caen/CompassHit.h
    Specter/Physics/Caen/CompassOnlineSource.cpp
    Specter/Physics/Caen/CompassOnlineSource.h
    Specter/Physics/Caen/CompassRun.cpp
    Specter/Physics/Caen/CompassRun.h
    Specter/Editor/EditorLayer.cpp
    Specter/Editor/EditorLayer.h
    Specter/Editor/FileDialog.cpp
    Specter/Editor/FileDialog.h
    Specter/Editor/SourceDialog.cpp
    Specter/Editor/SourceDialog.h
    Specter/Editor/SpectrumDialog.cpp
    Specter/Editor/SpectrumDialog.h
    Specter/Editor/SpectrumPanel.cpp
    Specter/Editor/SpectrumPanel.h
    Specter/Utils/Instrumentor.h
    Specter/Utils/TCPClient.cpp
    Specter/Utils/TCPClient.h
    Specter/Utils/TestServerLayer.cpp
    Specter/Utils/TestServerLayer.h
    Specter/Utils/Timer.h
    Platform/OpenGL/OpenGLContext.cpp
    Platform/OpenGL/OpenGLContext.h
    Platform/OpenGL/OpenGLRendererAPI.cpp
    Platform/OpenGL/OpenGLRendererAPI.h
    Platform/OpenGL/OpenGLWindow.cpp
    Platform/OpenGL/OpenGLWindow.h
)

#ImPlot sources
target_sources(Specter PRIVATE
    ../vendor/implot/implot.cpp
    ../vendor/implot/implot.h
    ../vendor/implot/implot_demo.cpp
    ../vendor/implot/implot_internal.h
    ../vendor/implot/implot_items.cpp
    ../vendor/implot/backends/implot_backend.h
    ../vendor/implot/backends/implot_impl_opengl3.cpp
    ../vendor/implot/backends/implot_impl_opengl3.h
)

#Link our accompanying dependencies
target_link_libraries(Specter PUBLIC imgui glfw glad)

#Link OS graphics implementations
set(THREADS_PREFER_PTHREAD_FLAG On)
find_package(Threads REQUIRED)
if(APPLE)
    target_link_libraries(Specter PRIVATE "-framework Cocoa" "-framework CoreVideo" "-framework IOKit" "-framework OpenGL" "-framework Carbon" ${CMAKE_DL_LIBS} Threads::Threads)
    target_compile_definitions(Specter PRIVATE SPEC_APPLE)
elseif(UNIX)
    target_link_libraries(Specter PRIVATE libGL.so libX11.so ${CMAKE_DL_LIBS} Threads::Threads)
    target_compile_definitions(Specter PRIVATE SPEC_LINUX)
elseif(MSVC)
    target_link_libraries(Specter PRIVATE opengl32.lib Threads::Threads)
    target_compile_definitions(Specter PRIVATE SPEC_WINDOWS)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(Specter PRIVATE SPEC_RELEASE)
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Specter PRIVATE SPEC_DEBUG)
endif()

#Some extra defs
target_compile_definitions(Specter PRIVATE GLFW_INCLUDE_NONE IMGUI_IMPL_OPENGL_LOADER_GLAD IMPLOT_BACKEND_ENABLE_OPENGL3)

set_target_properties(Specter PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${SPECTER_LIBRARY_DIR})