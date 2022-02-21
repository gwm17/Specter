#ifndef APPLICATION_H
#define APPLICATION_H

#include "Navigator/NavCore.h"
#include "Events/Event.h"
#include "Navigator/Events/AppEvent.h"
#include "Navigator/Events/PhysicsEvent.h"
#include "Navigator/LayerStack.h"
#include "Navigator/Layer.h"
#include "Navigator/Window.h"
#include "Navigator/ImGui/ImGuiLayer.h"
#include "Navigator/Physics/PhysicsLayer.h"
#include "glm/vec4.hpp"

namespace Navigator {

	class NAV_API Application
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

		glm::vec4 m_bckgnd_color = {0.1f, 0.1f, 0.1f, 1.0f};
		
		static Application* s_instance;
	};

	Application* CreateApplication();
}

#endif