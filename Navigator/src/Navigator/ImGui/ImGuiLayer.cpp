/*
	ImGuiLayer.h
	The layer containing all of the ImGui related setup and calls. Based on the work by @TheCherno in his game engine series.
	Should always exist as an overlay in the Application LayerStack. Note that it currently is OpenGL specific based on
	ImGui implementation/backends.

	GWM -- Feb 2022
*/
#include "ImGuiLayer.h"
#include "Navigator/Core/Application.h"
#include "Navigator/Core/NavCore.h"

#include "imgui.h"
#include "implot.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "IconsFontAwesome5.h"

namespace Navigator {

	ImGuiLayer::ImGuiLayer() :
		Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();

		NAV_INFO("Creating ImGui Context...");

		ImGui::CreateContext();
		ImPlot::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//Viewports are real wonky on Linux and sometimes on MacOS
		//Can currently cause assertion failure on checking number of monitors in ImGui sanity checks.
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark(); //Hacker mode
		ImPlot::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();

		if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		//Setup our fonts. We have Roboto for text and FontAwesome for our icons.
		//Note the .ttf files are found in NavProject, or in the bin dir. This is because
		//the actual program (NavProject) is launched from either the bin/ ... /NaProject or the NavProject directory
		//io.Fonts->AddFontDefault();
		ImFontConfig latin_config;
		latin_config.RasterizerMultiply = 1.3f;
		ImFontConfig config;
		config.MergeMode = true;
		//config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
		config.PixelSnapH = true;
		static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		io.Fonts->AddFontFromFileTTF("Assets/fonts/Roboto-Regular.ttf", 16.0f, &latin_config, io.Fonts->GetGlyphRangesDefault());
		io.Fonts->AddFontFromFileTTF("Assets/fonts/fa-solid-900.ttf", 16.0f, &config, icon_ranges);

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410"); //GLSL version

		NAV_INFO("ImGui Finished initializing.");
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		Application& app = Application::Get();
		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiLayer::OnImGuiRender()
	{
		//Demo's used to figure out how to do things.
		//Should not be on for actual NavProject for 
		//real use
		//static bool show = true;
		//ImGui::ShowDemoWindow(&show);
		//ImPlot::ShowDemoWindow();
	}
}
