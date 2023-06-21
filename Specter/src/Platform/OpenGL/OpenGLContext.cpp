/*
	OpenGLContext.h
	Implementation of OpenGL rendering context. Entirely based upon the work done by @TheCherno in his game engine series. See his content for more details.

	GWM -- Feb 2022
*/
#include "OpenGLContext.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"

namespace Specter {

	OpenGLContext::OpenGLContext(GLFWwindow* window) :
		m_windowHandle(window)
	{
	}

	OpenGLContext::~OpenGLContext() {}

	void OpenGLContext::Init()
	{
		SPEC_PROFILE_FUNCTION();
		glfwMakeContextCurrent(m_windowHandle);

		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

		//Report graphics status
		SPEC_INFO("Loaded OpenGL with glad Init status {0}", status);

		SPEC_INFO("Loaded OpenGL renderer");
		SPEC_INFO("Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		SPEC_INFO("Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		SPEC_INFO("Version: {0}", (const char*)glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_windowHandle);
	}
}