/*
	OpenGLWindow.h
	Implementation of a window with OpenGL context. Not really OpenGL specific, other than in creation of GraphicsContext.
	Bulk of creation can be used in any api/context (glfw compatible with Cocoa, X11, or Windows). Based entirely upon the
	work of @TheCherno in his game engine series.

	GWM -- Feb 2022
*/
#include "OpenGLWindow.h"
#include "OpenGLContext.h"
#include "Navigator/NavCore.h"
#include "Navigator/Events/AppEvent.h"
#include "Navigator/Events/KeyEvent.h"
#include "Navigator/Events/MouseEvent.h"

namespace Navigator {

	static bool s_glfwInitialized = false;
	static void GLFWErrorCallback(int error, const char* description)
	{
		NAV_ERROR("GLFW Error Code {0}: {1}", error, description);
	}

	Window* Window::Create(const WindowProperties& props) { return new OpenGLWindow(props); }

	OpenGLWindow::OpenGLWindow(const WindowProperties& props)
	{
		Init(props);
	} 

	OpenGLWindow::~OpenGLWindow()
	{
		Shutdown();
	}

	void OpenGLWindow::Init(const WindowProperties& props)
	{
		m_data.width = props.width;
		m_data.height = props.height;
		m_data.name = props.name;

		NAV_INFO("Creating OpenGLWindow with height {0} width {1} and name {2}", m_data.height, m_data.width, m_data.name);

		if(!s_glfwInitialized)
		{
			int passed = glfwInit();
			NAV_INFO("Initializing GLFW ... Returned value {0}", passed);
			glfwSetErrorCallback(GLFWErrorCallback);
		}
        
		//Apple specific. OpenGL is technically deprecated, so a little extra work to force the correct version
#ifdef __APPLE__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,true);
#endif

		m_window = glfwCreateWindow((int)m_data.width, (int)m_data.height, m_data.name.c_str(), nullptr, nullptr);

		m_context = new OpenGLContext(m_window); //This is the only seriously OpenGL specific code
		m_context->Init();

		glfwSetWindowUserPointer(m_window, &m_data);
		SetVSync(true);

		//Set all of the callback functions for the window. 
		glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
			{
				Data& data = *(Data*) glfwGetWindowUserPointer(window);
				data.width = width;
				data.height = height;
				WindowResizeEvent event(width, height);
				data.event_callback_func(event);
			});

		glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window)
			{
				Data& data = *(Data*) glfwGetWindowUserPointer(window);
				WindowCloseEvent event;
				data.event_callback_func(event);
			});

		glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				Data& data = *(Data*) glfwGetWindowUserPointer(window);
				switch(action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.event_callback_func(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.event_callback_func(event);
						break;
					}
					case GLFW_REPEAT:
					{
						//GLFW doesnt have a hold count, so here we just pass 1 to indicate a hold is happening.
						KeyPressedEvent event(key, 1);
						data.event_callback_func(event);
						break;
					}
				}
			});

		glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int character)
			{
				Data& data = *(Data*) glfwGetWindowUserPointer(window);
				KeyTypedEvent event(character);
				data.event_callback_func(event);
			});

		glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods)
			{
				Data& data = *(Data*) glfwGetWindowUserPointer(window);
				switch(action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.event_callback_func(event);
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.event_callback_func(event);
					}
				}
			});

		glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset)
			{
				Data& data = *(Data*) glfwGetWindowUserPointer(window);
				MouseScrolledEvent event((float)xoffset, (float)yoffset);
				data.event_callback_func(event);
			});

		glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos)
			{
				Data& data = *(Data*) glfwGetWindowUserPointer(window);
				MouseMovedEvent event((float)xpos, (float)ypos);
				data.event_callback_func(event);
			});
	}

	void OpenGLWindow::Shutdown()
	{
		glfwDestroyWindow(m_window);
	}

	void OpenGLWindow::OnUpdate()
	{
		glfwPollEvents();
		m_context->SwapBuffers();
	}

	void OpenGLWindow::SetVSync(bool enabled)
	{
		if(enabled)
		{
			glfwSwapInterval(1);
		}
		else
		{
			glfwSwapInterval(0);
		}
		m_data.vsyncFlag = enabled;
	}

	bool OpenGLWindow::IsVSync() const
	{
		return m_data.vsyncFlag;
	}
}
