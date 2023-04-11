/*
	Application.h
	This is the main application class, master and controller of program flow. Contains a layer stack, where each layer represets an
	aspect of the application. Based on the Application class written by @TheCherno during his tutorial series on making a game engine.
	Check out his work to learn more!

	Note that Application is a singleton. Should only ever be one application ever created in the project.

	GWM -- Feb 2022
*/
#ifndef APPLICATION_H
#define APPLICATION_H

#include "Specter/Core/SpecCore.h"
#include "Specter/Events/Event.h"
#include "Specter/Events/AppEvent.h"
#include "Specter/Events/PhysicsEvent.h"
#include "Specter/Core/LayerStack.h"
#include "Specter/Core/Layer.h"
#include "Specter/Core/Window.h"
#include "Specter/ImGui/ImGuiLayer.h"
#include "Specter/Physics/PhysicsLayer.h"
#include "SpectrumManager.h"
#include "glm/vec4.hpp"

namespace Specter {

	struct ApplicationArgs
	{
		std::string name = "";
		std::filesystem::path runtimePath = "";
	};

	class Application
	{
	public:
		Application(const ApplicationArgs& args);
		virtual ~Application();

		void Run();
		void Close() { m_runFlag = false; }

		void OnEvent(Event& event);
		void PushLayer(Layer* layer);
		void PushAnalysisStage(AnalysisStage* stage) { m_physicsLayer->PushStage(stage); }
		void PushOverlay(Layer* layer);


		static Application& Get() { return *s_instance; }

		Window& GetWindow() { return *m_window; }

		const ApplicationArgs& GetArgs() { return m_args; }

	private:
		bool OnWindowCloseEvent(WindowCloseEvent& event);

		ApplicationArgs m_args;

		LayerStack m_stack;
		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_imgui_layer;
		PhysicsLayer* m_physicsLayer;
		bool m_runFlag;

		//Dark grey background
		glm::vec4 m_bckgnd_color = {0.1f, 0.1f, 0.1f, 1.0f};
		
		static Application* s_instance;

	protected:
		SpectrumManager::Ref m_manager; //manager needs to be available to derived classes
	};


	/*
		This function must be defined by the user. It is called in by the entry point function in EntryPoint.h
	*/
	Application* CreateApplication(const ApplicationArgs& args);
}

#endif