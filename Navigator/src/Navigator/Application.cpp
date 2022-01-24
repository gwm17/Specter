#include "Application.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Editor/EditorLayer.h"

//temp
#include "CutMap.h"

namespace Navigator {

	Application* Application::s_instance = nullptr;

	Application::Application() :
		m_runFlag(true)
	{
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FUNCTION(Application::OnEvent));

		m_physicsLayer = new PhysicsLayer();
		PushLayer(m_physicsLayer);
		EditorLayer* editor = new EditorLayer(); //memory handled by layer stack
		editor->SetEventCallbackFunc(BIND_EVENT_FUNCTION(Application::OnEvent));
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
		//PhysicsStartEvent junk("/media/gordon/GordonData/gwm17/NavTests/data/", DataSource::SourceType::CompassOffline, 2000000);
		//OnEvent(junk);
		while(m_runFlag)
		{
			HistogramMap::GetInstance().UpdateHistograms();

			RenderCommand::SetClearColor(m_bckgnd_color);
			RenderCommand::Clear();

			for(auto layer : m_stack)
				layer->OnUpdate();

			m_imgui_layer->Begin();
			for(auto layer : m_stack)
				layer->OnImGuiRender();
			m_imgui_layer->End();
			m_window->OnUpdate();

			ParameterMap::GetInstance().find("joseph")->second->validFlag = true;
			ParameterMap::GetInstance().find("joseph")->second->value = 8.0;
		}
	}
}
