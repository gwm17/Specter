/*
	Application.cpp
	This is the main application class, master and controller of program flow. Contains a layer stack, where each layer represets an
	aspect of the application. Based on the Application class written by @TheCherno during his tutorial series on making a game engine.
	Check out his work to learn more!

	Note that Application is a singleton. Should only ever be one application ever created in the project.

	GWM -- Feb 2022
*/
#include "Application.h"
#include "Specter/Renderer/RenderCommand.h"
#include "Specter/Editor/EditorLayer.h"
#include "Timestep.h"

namespace Specter {


	Application* Application::s_instance = nullptr;

	Application::Application(const ApplicationArgs& args) :
		m_args(args), m_runFlag(true)
	{
		SPEC_PROFILE_FUNCTION();

		s_instance = this;

		//Set the runtime path so that we can find our assets
		if(!m_args.runtimePath.empty())
			std::filesystem::current_path(m_args.runtimePath);

		SPEC_INFO("Runtime Directory: {0}", std::filesystem::current_path().string());

		m_window = std::unique_ptr<Window>(Window::Create({m_args.name, 1280, 720}));
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
		SPEC_PROFILE_FUNCTION();
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
		SPEC_WARN("Closing the window!");
		return true;
	}

	void Application::PushLayer(Layer* layer)
	{
		SPEC_PROFILE_FUNCTION();
		m_stack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		SPEC_PROFILE_FUNCTION();
		m_stack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Run()
	{
		SPEC_PROFILE_FUNCTION();

		float lastFrameTime = 0;
		float time;
		Timestep step;

		while(m_runFlag)
		{
			RenderCommand::SetClearColor(m_bckgnd_color);
			RenderCommand::Clear();

			time = RenderCommand::GetFrameTime();
			step.SetTime(time - lastFrameTime);
			lastFrameTime = time;

			for(auto layer : m_stack)
				layer->OnUpdate(step);

			m_imgui_layer->Begin();
			for(auto layer : m_stack)
				layer->OnImGuiRender();
			m_imgui_layer->End();
			m_window->OnUpdate();
		}
	}
}
