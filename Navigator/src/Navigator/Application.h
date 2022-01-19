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
#include "Navigator/Physics/PhysicsLayer.h"
#include "Navigator/HistogramMap.h"

namespace Navigator {

	class NAV_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& event);
		void PushLayer(Layer* layer);
		inline void PushAnalysisStage(AnalysisStage* stage) { m_physicsLayer->PushStage(stage); }
		void PushOverlay(Layer* layer);
		void SetParameterList();  

		inline const std::vector<std::string>& GetParameterList() { return m_parameterList; }

		inline static Application& Get() { return *s_instance; }
		inline static void LinkParameterList() { s_instance->SetParameterList(); }

		inline Window& GetWindow() { return *m_window; }

	private:
		bool OnWindowCloseEvent(WindowCloseEvent& event);

		LayerStack m_stack;
		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_imgui_layer;
		PhysicsLayer* m_physicsLayer;
		std::vector<std::string> m_parameterList;
		bool m_runFlag;

		float m_bckgnd_color[4] = {0.1, 0.1, 0.1, 1.0};
		
		static Application* s_instance;
	};

	NAV_API Application* CreateApplication();
}

#endif