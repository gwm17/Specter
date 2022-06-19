/*
	RenderCommand.h
	Render call abstraction. Exists to allow Specter the felxibility to be backend api agnostic, though currently
	only OpenGL is implemented. Entirely based upon the work done by @TheCherno in his game engine series. See his content for more details.

	GWM -- Feb 2022
*/
#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "Specter/Core/SpecCore.h"
#include "RendererAPI.h"

namespace Specter {

	class RenderCommand
	{
	public:
		inline static void SetClearColor(const glm::vec4& color_array) { s_api->SetClearColor(color_array); }
		inline static void Clear() { s_api->Clear(); }
		inline static float GetFrameTime() { return s_api->GetFrameTime(); }

	private:
		static RendererAPI* s_api;
	};

}

#endif