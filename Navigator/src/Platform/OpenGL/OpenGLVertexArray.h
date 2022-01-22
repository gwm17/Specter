#ifndef OPENGL_VERTEX_ARRAY
#define OPENGL_VERTEX_ARRAY

#include "Navigator/Renderer/VertexArray.h"

namespace Navigator {

	class NAV_API OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		virtual ~OpenGLVertexArray();
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer) override;
		virtual void SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer) override;

		inline virtual const std::vector<std::shared_ptr<VertexBuffer>>& GetVertexBuffers() const override { return m_vertexBuffers; }
		inline virtual const std::shared_ptr<IndexBuffer>& GetIndexBuffer() const { return m_indexBuffer; }

	private:
		std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers;
		std::shared_ptr<IndexBuffer> m_indexBuffer;

		uint32_t m_rendererID;
	};

}

#endif