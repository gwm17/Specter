/*
	OpenGLContext.h
	Implementation of OpenGL rendering context. Entirely based upon the work done by @TheCherno in his game engine series. See his content for more details.

	GWM -- Feb 2022
*/
#ifndef OPEGL_CONTEXT_H
#define OPEGL_CONTEXT_H

#include "Navigator/Core/NavCore.h"
#include "Navigator/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Navigator {

	class NAV_API OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(GLFWwindow* window);
		~OpenGLContext();

		virtual void Init() override;
		virtual void SwapBuffers() override;
	private:
		GLFWwindow* m_windowHandle;
	};

}

#endif