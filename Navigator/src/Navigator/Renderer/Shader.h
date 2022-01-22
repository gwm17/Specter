#ifndef SHADER_H
#define SHADER_H

#include "glm/glm.hpp"

namespace Navigator {

	class NAV_API Shader
	{
	public:
		Shader(const std::string& vertexSource, const std::string& fragmentSource);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

	private:
		uint32_t m_renderID;
	};

}

#endif