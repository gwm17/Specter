#ifndef RENDERER_API_H
#define RENDERER_API_H

namespace Navigator {

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1
		};

		virtual void Clear() = 0;
		virtual void SetClearColor(const float* color_array) = 0;

		inline static API GetAPI() { return s_api; }

	private:
		static API s_api;
	};

}

#endif