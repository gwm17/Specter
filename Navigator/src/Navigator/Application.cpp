#include <navpch.h>
#include "Application.h"

namespace Navigator {

	Application* Application::s_instance = nullptr;

	Application::Application() :
		m_runFlag(true)
	{
		s_instance = this;
	}

	Application::~Application() {}


	void Application::OnEvent() {}

	void Application::PushLayer() {}

	void Application::PushOverlay() {}

	void Application::Run()
	{
		while(m_runFlag)
		{
			NAV_TRACE("Doing a run.");
			m_runFlag = false;
		}
	}
}