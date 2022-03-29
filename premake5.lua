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
IncludeDirs["IconFonts"] = "Navigator/vendor/IconFontCppHeaders"

include "Navigator/vendor/glfw"
include "Navigator/vendor/imgui"
include "Navigator/vendor/glad"
project "Navigator"
	location "Navigator"
	kind "StaticLib"
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
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"NavProject/src/*.h",
		"NavProject/src/*.cpp"
	}

	includedirs {
		"Navigator/src",
		"Navigator/src/Navigator",
		"Navigator/vendor/spdlog/include/",
		"%{IncludeDirs.ImPlot}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glad}",
		"%{IncludeDirs.glfw}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.asio}",
		"%{IncludeDirs.IconFonts}"
	}

	links {
		"Navigator"
	}

	systemversion "latest"

	postbuildcommands {
		(" {COPYDIR} Resources %{cfg.targetdir} ")
	}

	filter "system:macosx"
		defines "NAV_APPLE"
		sysincludedirs {
			"Navigator/vendor/spdlog/include/",
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
	filter "system:windows"
		defines "NAV_WINDOWS"
	filter "system:linux"
		defines "NAV_LINUX"
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
