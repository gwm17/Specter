/*
	RenderCommand.h
	Render call abstraction. Exists to allow Navigator the felxibility to be backend api agnostic, though currently
	only OpenGL is implemented. Entirely based upon the work done by @TheCherno in his game engine series. See his content for more details.

	GWM -- Feb 2022
*/
#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "Navigator/Core/NavCore.h"
#include "RendererAPI.h"

namespace Navigator {

	class NAV_API RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color_array) { s_api->SetClearColor(color_array); }
		inline static void Clear() { s_api->Clear(); }

	private:
		static RendererAPI* s_api;
	};

}

#endif