#include "Buffer.h"
#include "Renderer.h"
#include "RendererAPI.h"
#include "Platform/OpenGL/OpenGLBuffer.h"

namespace Navigator {

	VertexBuffer* VertexBuffer::Create(size_t size, float* arr)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return new OpenGLVertexBuffer(size, arr);
			return nullptr;
		case RendererAPI::API::None:
			NAV_ERROR("RendererAPI::None is not currently supported for VertexBuffer. Returning nullptr.");
			return nullptr;
		}

		NAV_ERROR("Invalid RendererAPI at VertexBuffer::Create(). Returning nullptr.");
		return nullptr;
	}

	IndexBuffer* IndexBuffer::Create(uint32_t count, uint32_t* arr)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::OpenGL:
			return new OpenGLIndexBuffer(count, arr);
			return nullptr;
		case RendererAPI::API::None:
			NAV_ERROR("RendererAPI::None is not currently supported for IndexBuffer. Returning nullptr.");
			return nullptr;
		}

		NAV_ERROR("Invalid RendererAPI at IndexBuffer::Create(). Returning nullptr.");
		return nullptr;
	}

}