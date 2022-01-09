#ifndef RENDERER_H
#define RENDERER_H

#include "Navigator/NavCore.h"
#include "RendererAPI.h"
#include "RenderCommand.h"

namespace Navigator {

	class NAV_API Renderer
	{
	public:

		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	};
}

#endif