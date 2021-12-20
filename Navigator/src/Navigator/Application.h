#ifndef APPLICATION_H
#define APPLICATION_H

#include "NavCore.h"
#include "Events/Event.h"
#include "Events/AppEvent.h"
#include "LayerStack.h"
#include "Layer.h"
#include "Window.h"

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

		inline void GetWindow() { return; }

	private:
		bool OnWindowCloseEvent(WindowCloseEvent& event);
		
		LayerStack m_stack;
		std::unique_ptr<Window> m_window;
		bool m_runFlag;
		
		static Application* s_instance;
	};

	Application* CreateApplication();
}

#endif