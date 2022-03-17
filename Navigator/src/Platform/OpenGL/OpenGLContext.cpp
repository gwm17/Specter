/*
	OpenGLContext.h
	Implementation of OpenGL rendering context. Entirely based upon the work done by @TheCherno in his game engine series. See his content for more details.

	GWM -- Feb 2022
*/
#include "OpenGLContext.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"

namespace Navigator {

	OpenGLContext::OpenGLContext(GLFWwindow* window) :
		m_windowHandle(window)
	{
	}

	OpenGLContext::~OpenGLContext() {}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_windowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		//Report graphics status
		NAV_TRACE("Loaded OpenGL with glad Init status {0}", status);

		NAV_INFO("Loaded OpenGL renderer");
		NAV_INFO("Vendor: {0}", glGetString(GL_VENDOR));
		NAV_INFO("Renderer: {0}", glGetString(GL_RENDERER));
		NAV_INFO("Version: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_windowHandle);
	}
}