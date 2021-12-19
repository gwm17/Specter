workspace "Nova"
	architecture "x64"
	configurations {
		"Debug",
		"Release"
	}
	startproject "NavProject"



outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
-- External directories to be included
IncludeDirs ={}
IncludeDirs["glfw"] = "src/vendor/glfw/include"
IncludeDirs["ImGui"] = "src/vendor/imgui"

include "src/vendor/glfw"
include "src/vendor/imgui"
project "Navigator"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	targetdir ("lib/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	pchheader "navpch.h"
	pchsource "navpch.cpp"

	files {
		"src/%{prj.name}/**.h",
		"src/%{prj.name}/**.cpp"
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs {
		"src/",
		"src/vendor/spdlog/include/",
		"src/%{prj.name}/",
		"%{IncludeDirs.glfw}",
		"%{IncludeDirs.ImGui}",
	}

	links {
		"GLFW",
		"ImGui",
		"opengl32.lib"
	}

	systemversion "latest"

	defines {
		"GLFW_INCLUDE_NONE"
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
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"src/main.cpp"
	}


	includedirs {
		"src/Navigator",
		"src/vendor/spdlog/include/",
		"src/vendor"
	}

	links {
		"Navigator"
	}


	systemversion "latest"

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