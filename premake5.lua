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

include "Navigator/vendor/glfw"
include "Navigator/vendor/imgui"
project "Navigator"
	location "Navigator"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	targetdir ("lib/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")
	pchheader "navpch.h"
	pchsource "Navigator/src/navpch.cpp"

	filter "system:windows"
		defines "NAV_WINDOWS"
		forceincludes {"navpch.h"}
		includedirs {
			"%{prj.name}/src/",
			"%{prj.name}/vendor/spdlog/include/",
			"%{IncludeDirs.glfw}",
			"%{IncludeDirs.ImGui}",
		}
	filter "system:linux or macosx"
		defines "NAV_UNIX"
		defines "NAV_PCH"
		includedirs {
			"%{prj.name}/vendor/spdlog/include/",
			"%{IncludeDirs.glfw}",
			"%{IncludeDirs.ImGui}",
		}
	filter {}

	files {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	defines {
		"_CRT_SECURE_NO_WARNINGS"
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
		"Navigator/vendor/spdlog/include/",
		"Navigator/vendor"
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