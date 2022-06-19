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
#include "glm/vec4.hpp"

namespace Specter {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();
		inline void Close() { m_runFlag = false; }

		void OnEvent(Event& event);
		void PushLayer(Layer* layer);
		inline void PushAnalysisStage(AnalysisStage* stage) { m_physicsLayer->PushStage(stage); }
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_instance; }

		inline Window& GetWindow() { return *m_window; }

	private:
		bool OnWindowCloseEvent(WindowCloseEvent& event);

		LayerStack m_stack;
		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_imgui_layer;
		PhysicsLayer* m_physicsLayer;
		bool m_runFlag;

		//Dark grey background
		glm::vec4 m_bckgnd_color = {0.1f, 0.1f, 0.1f, 1.0f};
		
		static Application* s_instance;
	};


	/*
		This function is left to be defined by the user. In principle we don't need to do this, as the Specter library doesn't handle creation of the application,
		but I like it and might be useful for changing to a system with a pre-defined entry point.
	*/
	Application* CreateApplication();
}

#endif