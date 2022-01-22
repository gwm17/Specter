#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "Navigator/NavCore.h"
#include "RendererAPI.h"

namespace Navigator {

	class NAV_API RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color_array) { s_api->SetClearColor(color_array); }
		inline static void Clear() { s_api->Clear(); }
		inline static void DrawIndexed(const std::shared_ptr<VertexArray>& array) { s_api->DrawIndexed(array); }
	private:
		static RendererAPI* s_api;
	};

}

#endif