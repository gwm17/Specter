/*
	RenderAPI.h
	Render API abstraction. Exists to allow Specter the felxibility to be backend api agnostic, though currently
	only OpenGL is implemented. Entirely based upon the work done by @TheCherno in his game engine series. See his content for more details.

	GWM -- Feb 2022
*/
#ifndef RENDERER_API_H
#define RENDERER_API_H

#include "Specter/Core/SpecCore.h"
#include "glm/vec4.hpp"

namespace Specter {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1 //Currently only have OpenGL
		};

		virtual void Clear() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual float GetFrameTime() = 0;

		static API GetAPI() { return s_api; }

	private:
		static API s_api;
	};

}

#endif