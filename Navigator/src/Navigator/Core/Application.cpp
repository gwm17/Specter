/*
	Application.cpp
	This is the main application class, master and controller of program flow. Contains a layer stack, where each layer represets an
	aspect of the application. Based on the Application class written by @TheCherno during his tutorial series on making a game engine.
	Check out his work to learn more!

	Note that Application is a singleton. Should only ever be one application ever created in the project.

	GWM -- Feb 2022
*/
#include "Application.h"
#include "Navigator/Renderer/RenderCommand.h"
#include "Navigator/Editor/EditorLayer.h"

namespace Navigator {

	Application* Application::s_instance = nullptr;

	Application::Application() :
		m_runFlag(true)
	{
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FUNCTION(Application::OnEvent)); //Allow window to pass events back

		m_physicsLayer = new PhysicsLayer();
		PushLayer(m_physicsLayer);
		EditorLayer* editor = new EditorLayer(); //memory handled by layer stack
		editor->SetEventCallbackFunc(BIND_EVENT_FUNCTION(Application::OnEvent)); //Allow editor to pass events back
		PushLayer(editor);
		m_imgui_layer = new ImGuiLayer();
		PushOverlay(m_imgui_layer);
	}

	Application::~Application()
	{
	}

	void Application::OnEvent(Event& event) 
	{
		EventDispatcher dispatch(event);
		dispatch.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(Application::OnWindowCloseEvent));
		for(auto iter = m_stack.end(); iter != m_stack.begin(); )
		{
			(*(--iter))->OnEvent(event);
			if(event.handledFlag)
				break;
		}
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
			RenderCommand::SetClearColor(m_bckgnd_color);
			RenderCommand::Clear();

			for(auto layer : m_stack)
				layer->OnUpdate();

			m_imgui_layer->Begin();
			for(auto layer : m_stack)
				layer->OnImGuiRender();
			m_imgui_layer->End();
			m_window->OnUpdate();
		}
	}
}
