#ifndef OPENGL_WINDOW_H
#define OPENGL_WINDOW_H

#include "Navigator/NavCore.h"
#include "Navigator/Window.h"
#include "Navigator/GraphicsContext.h"
#include "GLFW/glfw3.h"

namespace Navigator {

	class NAV_API OpenGLWindow : public Window
	{

	public:
		OpenGLWindow(const WindowProperties& props);
		virtual ~OpenGLWindow();

		void OnUpdate() override;

		inline void SetEventCallback(const EventCallbackFunc& function) override { m_data.event_callback_func = function; }
		inline unsigned int GetWidth() const override { return m_data.width; }
		inline unsigned int GetHeight() const override { return m_data.height; }
		inline std::string GetName() const override { return m_data.name; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		inline virtual void* GetNativeWindow() const override { return m_window; }
	private:
		virtual void Init(const WindowProperties& props);
		virtual void Shutdown();

		GraphicsContext* m_context;

		GLFWwindow* m_window;

		struct Data 
		{
			int height, width;
			std::string name;
			bool vsyncFlag;
			EventCallbackFunc event_callback_func;
		};

		Data m_data;
	};
}

#endif