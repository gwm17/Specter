#ifndef OPENGL_RENDERER_API_H
#define OPENGL_RENDERER_API_H

#include "Navigator/NavCore.h"
#include "Navigator/Renderer/RendererAPI.h"

namespace Navigator {

	class NAV_API OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Clear() override;
		virtual void SetClearColor(const glm::vec4& color_array) override;
	};

}

#endif