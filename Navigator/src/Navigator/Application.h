#ifndef APPLICATION_H
#define APPLICATION_H


namespace Navigator {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

		void OnEvent();
		void PushLayer();
		void PushOverlay();

		inline static Application& Get() { return *s_instance; }

		inline void GetWindow() { return; }

	private:

		bool m_runFlag;
		
		static Application* s_instance;
	};

	Application* CreateApplication();
}

#endif