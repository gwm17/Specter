#ifndef OPENGL_BUFFER_H
#define OPENGL_BUFFER_H

#include "Navigator/Renderer/Buffer.h"

namespace Navigator {

	class NAV_API OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(size_t size, float* arr);
		~OpenGLVertexBuffer();
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetLayout(const BufferLayout& layout) override { m_layout = layout; }
		virtual const BufferLayout& GetLayout() const override { return m_layout; }

	private:
		uint32_t m_renderID;
		BufferLayout m_layout;
	};

	class NAV_API OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t count, uint32_t* arr);
		~OpenGLIndexBuffer();
		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual uint32_t GetCount() const override { return m_count; }

	private:
		uint32_t m_renderID;
		uint32_t m_count;
	};

}

#endif