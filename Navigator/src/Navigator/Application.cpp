#include "Application.h"

namespace Navigator {

	Application* Application::s_instance = nullptr;

	Application::Application() :
		m_runFlag(true)
	{
		s_instance = this;

		PushLayer(new Layer());
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

	void Application::PushLayer(Layer* layer)
	{
		m_stack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_stack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Run()
	{
		while(m_runFlag)
		{
			NAV_TRACE("Doing a run.");
			for(auto layer : m_stack)
				NAV_TRACE("Layer with name {0} found!", layer->GetName());
			WindowCloseEvent event;
			OnEvent(event);
		}
	}
}