#include "OpenGLBuffer.h"

#include "glad/glad.h"

namespace Navigator {

	OpenGLVertexBuffer::OpenGLVertexBuffer(size_t size, float* arr)
	{
		glCreateBuffers(1, &m_renderID);
		glBindBuffer(GL_ARRAY_BUFFER, m_renderID);
		glBufferData(GL_ARRAY_BUFFER, size, arr, GL_STATIC_DRAW);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_renderID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_renderID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t count, uint32_t* arr) :
		m_count(count)
	{
		glCreateBuffers(1, &m_renderID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_count * sizeof(uint32_t), arr, GL_STATIC_DRAW);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_renderID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}