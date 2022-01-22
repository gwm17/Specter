#ifndef RENDERER_H
#define RENDERER_H

#include "Navigator/NavCore.h"
#include "RendererAPI.h"
#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"

namespace Navigator {

	class NAV_API Renderer
	{
	public:

		static void BeginScene();
		static void EndScene();
		static void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& data);

		static inline RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

	};
}

#endif