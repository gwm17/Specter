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
		PushLayer(new EditorLayer());
		m_imgui_layer = new ImGuiLayer();
		PushOverlay(m_imgui_layer);
		HistogramMap& histMap = HistogramMap::GetInstance();
		histMap.AddHistogram(HistogramParameters("myHisto", "joseph", 100, 0, 10));
		histMap.AddHistogram(HistogramParameters("myHisto2D", "joseph", "joseph", 100, 0, 10, 100, 0, 10));
        
        CutMap::GetInstance().AddCut(CutParams("joe_cut","joseph"),0.0, 7.0);
		CutMap::GetInstance().AddCut(CutParams("joe2D_cut", "joseph", "joseph"), { 1.0, 3.0, 3.0, 1.0, 1.0}, { 1.0, 1.0, 3.0, 3.0, 1.0});
        
        histMap.AddCutToHistogramDraw("joe_cut", "myHisto");
		histMap.AddCutToHistogramDraw("joe2D_cut", "myHisto2D");

		
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
		HistogramMap::GetInstance().UpdateHistograms();
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
