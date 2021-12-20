#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "RendererAPI.h"

namespace Navigator {

	class RenderCommand
	{
	public:
		inline static void SetClearColor(const float* color_array) { s_api->SetClearColor(color_array); }
		inline static void Clear() { s_api->Clear(); }
	private:
		static RendererAPI* s_api;
	};

}

#endif