#ifndef RENDERER_H
#define RENDERER_H

#include "RendererAPI.h"
#include "RenderCommand.h"

namespace Navigator {

	class Renderer
	{
	public:

		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	};
}

#endif