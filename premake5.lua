workspace "Navigator"
	architecture "x64"
	configurations {
		"Debug",
		"Release"
	}
	startproject "NavProject"



outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
-- External directories to be included
IncludeDirs ={}
IncludeDirs["glfw"] = "Navigator/vendor/glfw/include"
IncludeDirs["ImGui"] = "Navigator/vendor/imgui"
IncludeDirs["glad"] = "Navigator/vendor/glad/include"
IncludeDirs["ImPlot"] = "Navigator/vendor/implot"
IncludeDirs["glm"] = "Navigator/vendor/glm"
IncludeDirs["asio"] = "Navigator/vendor/asio/asio/include"

include "Navigator/vendor/glfw"
include "Navigator/vendor/imgui"
include "Navigator/vendor/glad"
project "Navigator"
	location "Navigator"
	kind "SharedLib"
	language "C++"
	cppdialect "C++17"
	targetdir ("lib/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	filter "system:windows"
		defines "NAV_WINDOWS"
		pchheader "navpch.h"
		pchsource "Navigator/src/navpch.cpp"
		forceincludes {"navpch.h"}
		
	filter "system:linux or macosx"
		pchheader "%{prj.name}/src/navpch.h"
	filter "system:macosx"
		pchheader "src/navpch.h"
		sysincludedirs {
			"%{prj.name}/vendor/spdlog/include/",
			"%{IncludeDirs.glfw}",
			"%{IncludeDirs.ImGui}",
			"%{IncludeDirs.glad}",
			"%{IncludeDirs.ImPlot}",
			"%{IncludeDirs.glm}",
			"%{IncludeDirs.asio}"
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
		"%{IncludeDirs.asio}"
	}
	

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/vendor/implot/*.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/implot/*.cpp"
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
		"GLFW_INCLUDE_NONE"
	}


	filter "system:linux"
		defines "NAV_LINUX"
		links {
			"GL",
			"X11",
			"dl"
		}
		linkoptions {
			"-pthread"
		}
	filter "system:macosx"
		defines "NAV_APPLE"
		links {
			"Cocoa.framework",
			"CoreVideo.framework",
			"IOKit.framework",
			"OpenGL.framework",
			"Carbon.framework",
			"dl",
		}
		linkoptions{
			"-pthread",
			"-undefined dynamic_lookup"
		}
	filter "system:windows"
		defines "NAV_EXPORT"
		postbuildcommands {
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/NavProject/\"")
		}
		links {
			"opengl32.lib"
		}

	filter "configurations:Debug"
		defines "NAV_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "NAV_RELEASE"
		runtime "Release"
		optimize "on"

project "NavProject"
	location "NavProject"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"NavProject/main.cpp"
	}


	includedirs {
		"Navigator/src",
		"Navigator/src/Navigator",
		"Navigator/vendor/spdlog/include/",
		"Navigator/vendor/implot/",
		"Navigator/vendor",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.asio}"
	}

	links {
		"Navigator"
	}


	systemversion "latest"

	filter "system:macosx"
		defines "NAV_APPLE"
		sysincludedirs {
			"Navigator/vendor/spdlog/include/",
			"%{IncludeDirs.glfw}",
			"%{IncludeDirs.ImGui}",
			"%{IncludeDirs.glad}",
			"%{IncludeDirs.ImPlot}",
			"%{IncludeDirs.glm}"
		}
	filter "system:windows"
		defines "NAV_WINDOWS"
	filter "system:linux"
		defines "NAV_LINUX"

	

	filter "configurations:Debug"
		defines "NAV_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "NAV_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "NAV_DIST"
		runtime "Release"
		optimize "on"
