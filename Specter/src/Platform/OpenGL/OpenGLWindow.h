/*
	OpenGLWindow.h
	Implementation of a window with OpenGL context. Not really OpenGL specific, other than in creation of GraphicsContext.
	Bulk of creation can be used in any api/context (glfw compatible with Cocoa, X11, or Windows). Based entirely upon the 
	work of @TheCherno in his game engine series.

	GWM -- Feb 2022
*/
#ifndef OPENGL_WINDOW_H
#define OPENGL_WINDOW_H

#include "Specter/Core/SpecCore.h"
#include "Specter/Core/Window.h"
#include "Specter/Renderer/GraphicsContext.h"
#include "GLFW/glfw3.h"

namespace Specter {

	class OpenGLWindow : public Window
	{

	public:
		OpenGLWindow(const WindowProperties& props);
		virtual ~OpenGLWindow();

		void OnUpdate() override;

		void SetEventCallback(const EventCallbackFunc& function) override { m_data.event_callback_func = function; }
		unsigned int GetWidth() const override { return m_data.width; }
		unsigned int GetHeight() const override { return m_data.height; }
		std::string GetName() const override { return m_data.name; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		virtual void* GetNativeWindow() const override { return m_window; }
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