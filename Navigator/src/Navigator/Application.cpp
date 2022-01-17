#include "Application.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"
#include "Editor/EditorLayer.h"

//temp
#include "CutMap.h"

namespace Navigator {

	Application* Application::s_instance = nullptr;

	Application::Application() :
		m_runFlag(true), m_physThread(nullptr)
	{
		s_instance = this;

		m_window = std::unique_ptr<Window>(Window::Create());
		m_window->SetEventCallback(BIND_EVENT_FUNCTION(Application::OnEvent));

		PushLayer(new EditorLayer(&m_histMap));

		m_histMap.AddHistogram("myHisto", "joseph", 100, 0, 10);
		m_histMap.AddHistogram("myHisto2D", "joseph", "joseph", 100, 0, 10, 100, 0, 10);
        
        CutMap::GetInstance().AddCut("joe_cut","joseph",0.0, 7.0);
		CutMap::GetInstance().AddCut("joe2D_cut", "joseph", "joseph", { 1.0, 3.0, 3.0, 1.0, 1.0}, { 1.0, 1.0, 3.0, 3.0, 1.0});
        
        m_histMap.AddCutToHistogramDraw("joe_cut", "myHisto");
		m_histMap.AddCutToHistogramDraw("joe2D_cut", "myHisto2D");

		m_imgui_layer = new ImGuiLayer();
		PushOverlay(m_imgui_layer);
	}

	Application::~Application()
	{
		NAV_INFO("Detaching PhysicsEventBuilder at shutdown");
		PhysicsEventBuilder::Get().DetachDataSource();
		DestroyPhysThread();
	}

	void Application::DestroyPhysThread()
	{
		if(m_physThread != nullptr && m_physThread->joinable())
			m_physThread->join();

		if(m_physThread != nullptr)
		{
			delete m_physThread;
			m_physThread = nullptr;
		}
	}

	void Application::SetParameterList()
	{
		m_parameterList = ParameterMap::GetInstance().GetListOfParameters();
		PhysicsParamEvent event;
		OnEvent(event);
	}

	void Application::OnEvent(Event& event) 
	{
		EventDispatcher dispatch(event);
		dispatch.Dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(Application::OnWindowCloseEvent));
		dispatch.Dispatch<PhysicsStartEvent>(BIND_EVENT_FUNCTION(Application::OnPhysicsStartEvent));
		dispatch.Dispatch<PhysicsStopEvent>(BIND_EVENT_FUNCTION(Application::OnPhysicsStopEvent));
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

	bool Application::OnPhysicsStartEvent(PhysicsStartEvent& event)
	{
		if(PhysicsEventBuilder::Get().IsRunning())
		{
			PhysicsEventBuilder::Get().DetachDataSource();
			NAV_INFO("Stopping the event builder...");
			DestroyPhysThread();
		}
		PhysicsEventBuilder::Get().AttachDataSource(event.GetSourceLocation(), event.GetSourceType());
		PhysicsEventBuilder::Get().SetCoincidenceWindow(event.GetCoincidenceWindow());
		if(PhysicsEventBuilder::Get().IsRunning())
		{
			NAV_INFO("Starting the event builder...");
			m_physThread = new std::thread(&PhysicsEventBuilder::Run, std::ref(PhysicsEventBuilder::Get()));
		}
		return true;
	}

	bool Application::OnPhysicsStopEvent(PhysicsStopEvent& event)
	{
		PhysicsEventBuilder::Get().DetachDataSource();
		NAV_INFO("Stopping the event builder...");
		DestroyPhysThread();
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
		m_histMap.UpdateHistograms();
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
