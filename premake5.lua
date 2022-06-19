workspace "Specter"
	architecture "x64"
	configurations {
		"Debug",
		"Release"
	}
	startproject "SpecProject"



outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
-- External directories to be included
IncludeDirs ={}
IncludeDirs["glfw"] = "Specter/vendor/glfw/include"
IncludeDirs["ImGui"] = "Specter/vendor/imgui"
IncludeDirs["glad"] = "Specter/vendor/glad/include"
IncludeDirs["ImPlot"] = "Specter/vendor/implot"
IncludeDirs["glm"] = "Specter/vendor/glm"
IncludeDirs["asio"] = "Specter/vendor/asio/asio/include"
IncludeDirs["IconFonts"] = "Specter/vendor/IconFontCppHeaders"

include "Specter/vendor/glfw"
include "Specter/vendor/imgui"
include "Specter/vendor/glad"
project "Specter"
	location "Specter"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	targetdir ("lib/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	filter "system:windows"
		defines "SPEC_WINDOWS"
		pchheader "specpch.h"
		pchsource "Specter/src/specpch.cpp"
		forceincludes {"specpch.h"}
		
	filter "system:linux or macosx"
		pchheader "%{prj.name}/src/specpch.h"
	filter "system:macosx"
		sysincludedirs {
			"%{prj.name}/vendor/spdlog/include/",
			"%{IncludeDirs.glfw}",
			"%{IncludeDirs.ImGui}",
			"%{IncludeDirs.glad}",
			"%{IncludeDirs.ImPlot}",
			"%{IncludeDirs.glm}",
			"%{IncludeDirs.asio}",
			"%{IncludeDirs.IconFonts}"
		}
	filter {}

	includedirs {
		"%{prj.name}/src/",
		"%{prj.name}/vendor/spdlog/include/",
		"%{IncludeDirs.glfw}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glad}",
		"%{IncludeDirs.ImPlot}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.asio}",
		"%{IncludeDirs.IconFonts}"
	}
	

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/vendor/implot/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/implot/**.cpp"
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

	links {
		"GLFW",
		"GLAD",
		"ImGui"
	}

	systemversion "latest"
	defines {
		"GLFW_INCLUDE_NONE",
		"IMGUI_IMPL_OPENGL_LOADER_GLAD",
		"IMPLOT_BACKEND_ENABLE_OPENGL3"
	}


	filter "system:linux"
		defines "SPEC_LINUX"
		links {
			"GL",
			"X11",
			"dl"
		}
		linkoptions {
			"-pthread"
		}
	filter "system:macosx"
		defines "SPEC_APPLE"
		links {
			"Cocoa.framework",
			"CoreVideo.framework",
			"IOKit.framework",
			"OpenGL.framework",
			"Carbon.framework",
			"dl"
		}
		linkoptions{
			"-pthread",
			"-undefined dynamic_lookup"
		}
	filter "system:windows"
		links {
			"opengl32.lib"
		}

	filter "configurations:Debug"
		defines "SPEC_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SPEC_RELEASE"
		runtime "Release"
		optimize "on"

project "SpecProject"
	location "SpecProject"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"SpecProject/src/*.h",
		"SpecProject/src/*.cpp"
	}

	includedirs {
		"Specter/src",
		"Specter/src/Specter",
		"Specter/vendor/spdlog/include/",
		"%{IncludeDirs.ImPlot}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glad}",
		"%{IncludeDirs.glfw}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.asio}",
		"%{IncludeDirs.IconFonts}"
	}

	links {
		"Specter"
	}

	systemversion "latest"

	filter "system:macosx"
		defines "SPEC_APPLE"
		sysincludedirs {
			"Specter/vendor/spdlog/include/",
			"%{IncludeDirs.glfw}",
			"%{IncludeDirs.ImGui}",
			"%{IncludeDirs.glad}",
			"%{IncludeDirs.ImPlot}",
			"%{IncludeDirs.glm}",
			"%{IncludeDirs.asio}",
			"%{IncludeDirs.IconFonts}"
		}
		links {
			"Cocoa.framework",
			"CoreVideo.framework",
			"IOKit.framework",
			"OpenGL.framework",
			"Carbon.framework",
			"GLFW",
			"GLAD",
			"ImGui",
			"dl"
		}
		linkoptions {
			"-pthread"
		}
		postbuildcommands {
			(" {COPYDIR} Assets %{cfg.targetdir} ")
		}
	filter "system:windows"
		defines "SPEC_WINDOWS"
		postbuildcommands {
			(" {COPYDIR} Assets %{cfg.targetdir}/Assets ")
		}
	filter "system:linux"
		defines "SPEC_LINUX"
		links {
			"GL",
			"X11",
			"GLFW",
			"GLAD",
			"ImGui",
			"dl"
		}
		linkoptions {
			"-pthread"
		}
		postbuildcommands {
			(" {COPYDIR} Assets %{cfg.targetdir} ")
		}

	

	filter "configurations:Debug"
		defines "SPEC_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "SPEC_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SPEC_DIST"
		runtime "Release"
		optimize "on"
