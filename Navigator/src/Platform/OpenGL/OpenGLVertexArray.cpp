#include "OpenGLVertexArray.h"

#include "glad/glad.h"

namespace Navigator {

	static GLenum ConvertShaderDataTypeToGLenum(ShaderDataType type)
	{
		switch (type)
		{
		case Navigator::ShaderDataType::Float:  return GL_FLOAT;
		case Navigator::ShaderDataType::Float2: return GL_FLOAT;
		case Navigator::ShaderDataType::Float3: return GL_FLOAT;
		case Navigator::ShaderDataType::Float4: return GL_FLOAT;
		case Navigator::ShaderDataType::Int:    return GL_INT;
		case Navigator::ShaderDataType::Int2:   return GL_INT;
		case Navigator::ShaderDataType::Int3:   return GL_INT;
		case Navigator::ShaderDataType::Int4:   return GL_INT;
		case Navigator::ShaderDataType::Mat3:   return GL_FLOAT;
		case Navigator::ShaderDataType::Mat4:   return GL_FLOAT;
		case Navigator::ShaderDataType::Bool:   return GL_BOOL;
		}

		NAV_ERROR("Unrecognized ShaderDataType at ConvertShaderDataTypeToGLenum!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &m_rendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_rendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_rendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& buffer)
	{
		if(buffer->GetLayout().GetElements().size() == 0)
			NAV_ERROR("VertexBuffer with no layout passed to VertexArray::AddVertexBuffer!");
		glBindVertexArray(m_rendererID);
		buffer->Bind();


		uint32_t index = 0;
		const auto& layout = buffer->GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, element.GetComponentCount(), ConvertShaderDataTypeToGLenum(element.type), element.normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)element.offset);
			index++;
		}

		m_vertexBuffers.push_back(buffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const std::shared_ptr<IndexBuffer>& buffer)
	{
		glBindVertexArray(m_rendererID);
		buffer->Bind();

		m_indexBuffer = buffer;
	}

}