#ifndef APPLICATION_H
#define APPLICATION_H

#include "Navigator/NavCore.h"
#include "Events/Event.h"
#include "Navigator/Events/AppEvent.h"
#include "Navigator/Events/PhysicsEvent.h"
#include "Navigator/LayerStack.h"
#include "Navigator/Layer.h"
#include "Navigator/Window.h"
#include "Navigator/ImGui/ImGuiLayer.h"
#include "Navigator/Physics/PhysicsEventBuilder.h"
#include "Navigator/HistogramMap.h"
#include <thread>

namespace Navigator {

	class NAV_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& event);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);
		void SetParameterList();  

		inline void AttachHistogramMap() { PhysicsEventBuilder::Get().AttachHistogramMap(&m_histMap); }
		inline const std::vector<std::string>& GetParameterList() { return m_parameterList; } //Thread-safe way to access a list of the available parameters (i.e. for the editor)

		inline static Application& Get() { return *s_instance; }
		inline static void LinkHistogramMap() { s_instance->AttachHistogramMap(); } //IMPORTANT: Only use BEFORE calling Run(). NO guarantee of thread safety.
		inline static void LinkParameterList() { s_instance->SetParameterList(); } //IMPORTANT: Only use BEFORE calling Run(). NO guarantee of thread safety.

		inline Window& GetWindow() { return *m_window; }

	private:
		bool OnWindowCloseEvent(WindowCloseEvent& event);
		bool OnPhysicsStartEvent(PhysicsStartEvent& event);
		bool OnPhysicsStopEvent(PhysicsStopEvent& event);

		void DestroyPhysThread();

		std::thread* m_physThread;
		
		LayerStack m_stack;
		HistogramMap m_histMap;
		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_imgui_layer;
		std::vector<std::string> m_parameterList;
		bool m_runFlag;

		float m_bckgnd_color[4] = {0.1, 0.1, 0.1, 1.0};
		
		static Application* s_instance;
	};

	NAV_API Application* CreateApplication();
}

#endif