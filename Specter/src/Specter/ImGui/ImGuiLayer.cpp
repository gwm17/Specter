/*
	ImGuiLayer.h
	The layer containing all of the ImGui related setup and calls. Based on the work by @TheCherno in his game engine series.
	Should always exist as an overlay in the Application LayerStack. Note that it currently is OpenGL specific based on
	ImGui implementation/backends.

	GWM -- Feb 2022
*/
#include "ImGuiLayer.h"
#include "Specter/Core/Application.h"
#include "Specter/Core/SpecCore.h"

#include "imgui.h"
#include "implot.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
//Use embedded fonts
#include "Roboto-Regular.h"
#include "FA-Solid.h"
#include "IconsFontAwesome5.h"

namespace Specter {

	ImGuiLayer::ImGuiLayer() :
		Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		SPEC_PROFILE_FUNCTION();
		IMGUI_CHECKVERSION();

		SPEC_INFO("Creating ImGui Context...");

		ImGui::CreateContext();
		ImPlot::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark(); //Hacker mode
		ImPlot::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();

		if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		//Setup our fonts. We have Roboto for text and FontAwesome for our icons.
		//These fonts are embedded in the application for better resource management. Found in FA-Solid.h, Roboto-Regular.h
		ImFontConfig latin_config;
		latin_config.RasterizerMultiply = 1.3f;
		ImFontConfig config;
		config.MergeMode = true;
		//config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
		config.PixelSnapH = true;
		static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		io.Fonts->AddFontFromMemoryTTF(Roboto_Regular_ttf, Roboto_Regular_ttf_len, 16.0f, &latin_config, io.Fonts->GetGlyphRangesDefault());
		io.Fonts->AddFontFromMemoryTTF(fa_solid_900_ttf, fa_solid_900_ttf_len, 16.0f, &config, icon_ranges);

		//ImPlot styling
		ImPlot::GetStyle().FillAlpha = 0.75;

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410"); //GLSL version

		SPEC_INFO("ImGui Finished initializing.");
	}

	void ImGuiLayer::OnDetach()
	{
		SPEC_PROFILE_FUNCTION();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		SPEC_PROFILE_FUNCTION();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		SPEC_PROFILE_FUNCTION();
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
		//Should not be on for actual SpecProject for 
		//real use
		//static bool show = true;
		//ImGui::ShowDemoWindow(&show);
		//ImPlot::ShowDemoWindow();
	}
}
