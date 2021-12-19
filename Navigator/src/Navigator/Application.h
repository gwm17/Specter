#ifndef APPLICATION_H
#define APPLICATION_H

#include "NavCore.h"
#include "Events/Event.h"
#include "Events/AppEvent.h"

namespace Navigator {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& event);
		void PushLayer();
		void PushOverlay();

		inline static Application& Get() { return *s_instance; }

		inline void GetWindow() { return; }

	private:
		bool OnWindowCloseEvent(WindowCloseEvent& event);
		
		bool m_runFlag;
		
		static Application* s_instance;
	};

	Application* CreateApplication();
}

#endif