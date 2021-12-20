#ifndef OPENGL_RENDERER_API_H
#define OPENGL_RENDERER_API_H

#include "Navigator/Renderer/RendererAPI.h"

namespace Navigator {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Clear() override;
		virtual void SetClearColor(const float* color_array) override;
	};

}

#endif