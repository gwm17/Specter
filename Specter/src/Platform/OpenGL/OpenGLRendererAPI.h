/*
	OpenGLRendererAPI.h
	Implementation of OpenGL API in Specter context. Note here only a few things exist. We don't need to implement much ourselves,
	ImGui handles a lot of the heavy lifting for us. Based entirely upon @TheCherno's work in his game engine series. 

	GWM -- Feb 2022
*/
#ifndef OPENGL_RENDERER_API_H
#define OPENGL_RENDERER_API_H

#include "Specter/Core/SpecCore.h"
#include "Specter/Renderer/RendererAPI.h"

namespace Specter {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Clear() override;
		virtual void SetClearColor(const glm::vec4& color_array) override;
		virtual float GetFrameTime() override;
	};

}

#endif