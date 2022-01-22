#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Navigator {
	VertexArray* VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return new OpenGLVertexArray();
		case RendererAPI::API::None:
			NAV_ERROR("RendererAPI::None is not currently supported for IndexBuffer. Returning nullptr.");
			return nullptr;
		}

		NAV_ERROR("Invalid RendererAPI at VertexArray::Create!");
		return nullptr;
	}
}