#include "Application.h"

namespace Navigator {

	Application* Application::s_instance = nullptr;

	Application::Application() :
		m_runFlag(true)
	{
		s_instance = this;
	}

	Application::~Application() {}


	void Application::OnEvent(Event& event) 
	{
		EventDispatcher dispatch(event);
		dispatch.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(Application::OnWindowCloseEvent));
	}

	bool Application::OnWindowCloseEvent(WindowCloseEvent& event)
	{
		m_runFlag = false;
		NAV_WARN("Closing the window!");
		return true;
	}

	void Application::PushLayer() {}

	void Application::PushOverlay() {}

	void Application::Run()
	{
		while(m_runFlag)
		{
			NAV_TRACE("Doing a run.");
			WindowCloseEvent event;
			OnEvent(event);
		}
	}
}