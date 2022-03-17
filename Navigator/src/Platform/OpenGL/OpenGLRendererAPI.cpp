/*
	OpenGLRendererAPI.h
	Implementation of OpenGL API in Navigator context. Note here only a few things exist. We don't need to implement much ourselves,
	ImGui handles a lot of the heavy lifting for us. Based entirely upon @TheCherno's work in his game engine series.

	GWM -- Feb 2022
*/
#include "OpenGLRendererAPI.h"

#include "glad/glad.h"

namespace Navigator {

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color_array)
	{
		glClearColor(color_array[0], color_array[1], color_array[2], color_array[3]);
	}
}