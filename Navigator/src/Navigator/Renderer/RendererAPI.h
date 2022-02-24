#ifndef RENDERER_API_H
#define RENDERER_API_H

#include "Navigator/NavCore.h"
#include "glm/vec4.hpp"

namespace Navigator {

	class NAV_API RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1
		};

		virtual void Clear() = 0;
		virtual void SetClearColor(const glm::vec4& color) = 0;

		inline static API GetAPI() { return s_api; }

	private:
		static API s_api;
	};

}

#endif