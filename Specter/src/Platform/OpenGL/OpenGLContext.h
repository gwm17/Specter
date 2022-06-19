/*
	OpenGLContext.h
	Implementation of OpenGL rendering context. Entirely based upon the work done by @TheCherno in his game engine series. See his content for more details.

	GWM -- Feb 2022
*/
#ifndef OPEGL_CONTEXT_H
#define OPEGL_CONTEXT_H

#include "Specter/Core/SpecCore.h"
#include "Specter/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Specter {

	class OpenGLContext : public GraphicsContext
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