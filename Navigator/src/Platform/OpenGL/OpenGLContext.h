#ifndef OPEGL_CONTEXT_H
#define OPEGL_CONTEXT_H

#include "Navigator/NavCore.h"
#include "Navigator/GraphicsContext.h"

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