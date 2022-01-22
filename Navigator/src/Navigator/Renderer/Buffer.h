#ifndef BUFFER_H
#define BUFFER_H

namespace Navigator {

	NAV_API enum class ShaderDataType {
		None,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Mat3, Mat4,
		Bool
	};

	static uint32_t GetShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::None:
		{
			NAV_ERROR("ShaderDataType::None is not supported for GetShaderDataTypeSizes");
			return 0;
		}
		case ShaderDataType::Float:
			return 4;
		case ShaderDataType::Float2:
			return 4 * 2;
		case ShaderDataType::Float3:
			return 4 * 3;
		case ShaderDataType::Float4:
			return 4 * 4;
		case ShaderDataType::Int:
			return 4;
		case ShaderDataType::Int2:
			return 4 * 2;
		case ShaderDataType::Int3:
			return 4 * 3;
		case ShaderDataType::Int4:
			return 4 * 4;
		case ShaderDataType::Mat3:
			return 4 * 3 * 3;
		case ShaderDataType::Mat4:
			return 4 * 4 * 4;
		case ShaderDataType::Bool:
			return 1;
		}

		NAV_ERROR("Unrecognized ShaderDataType at GetShaderDataTypeSize!");
		return 0;
	}

	struct NAV_API BufferElement
	{
		std::string name;
		ShaderDataType type;
		uint32_t size;
		uint32_t offset;
		bool normalized;

		BufferElement() {}
		BufferElement(ShaderDataType itype, const std::string& iname, bool norm = false) :
			name(iname), type(itype), size(GetShaderDataTypeSize(itype)), offset(0), normalized(norm)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (type)
			{
			case ShaderDataType::Float:  return 1;
			case ShaderDataType::Float2: return 2;
			case ShaderDataType::Float3: return 3;
			case ShaderDataType::Float4: return 4;
			case ShaderDataType::Int:    return 1;
			case ShaderDataType::Int2:   return 2;
			case ShaderDataType::Int3:   return 3;
			case ShaderDataType::Int4:   return 4;
			case ShaderDataType::Mat3:   return 9;
			case ShaderDataType::Mat4:   return 16;
			case ShaderDataType::Bool:   return 1;
			}

			NAV_ERROR("Unrecognized ShaderDataType at BufferElement::GetComponentCount!");
			return 0;
		}
	};

	class NAV_API BufferLayout
	{
	public:

		BufferLayout() {}
		BufferLayout(const std::initializer_list<BufferElement>& elements) :
			m_elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		inline const std::vector<BufferElement>& GetElements() const { return m_elements; }
		inline const uint32_t GetStride() const { return m_stride; }

		std::vector<BufferElement>::iterator begin() { return m_elements.begin(); };
		std::vector<BufferElement>::iterator end() { return m_elements.end(); };
		std::vector<BufferElement>::const_iterator begin() const { return m_elements.begin(); };
		std::vector<BufferElement>::const_iterator end() const { return m_elements.end(); };

	private:
		void CalculateOffsetsAndStride()
		{
			uint32_t offset = 0;
			for (auto& element : m_elements)
			{
				element.offset = offset;
				offset += element.size;
			}

			m_stride = offset;
		}

		std::vector<BufferElement> m_elements;
		uint32_t m_stride;
	};

	class NAV_API VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {};
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual const BufferLayout& GetLayout() const = 0;

		static VertexBuffer* Create(size_t size, float* arr);

	};

	class NAV_API IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {};
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual uint32_t GetCount() const = 0;

		static IndexBuffer* Create(uint32_t count, uint32_t* arr);
	};
}

#endif
