#ifndef APPLICATION_H
#define APPLICATION_H

#include "Navigator/NavCore.h"
#include "Events/Event.h"
#include "Navigator/Events/AppEvent.h"
#include "Navigator/LayerStack.h"
#include "Navigator/Layer.h"
#include "Navigator/Window.h"
#include "Navigator/ImGui/ImGuiLayer.h"

namespace Navigator {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& event);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		inline static Application& Get() { return *s_instance; }

		inline Window& GetWindow() { return *m_window; }

	private:
		bool OnWindowCloseEvent(WindowCloseEvent& event);
		
		LayerStack m_stack;
		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_imgui_layer;
		bool m_runFlag;
		
		static Application* s_instance;
	};

	Application* CreateApplication();
}

#endif