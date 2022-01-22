#include "Shader.h"
#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"

namespace Navigator {

	Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource)
	{
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar* source = (const GLchar*)vertexSource.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			// Use the infoLog as you see fit.
			NAV_ERROR("Shader InfoLog: {0}", infoLog.data());
			NAV_ERROR("OpenGL Vertex Shader compilation error!");
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = (const GLchar*)fragmentSource.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			// Use the infoLog as you see fit.
			NAV_ERROR("Shader InfoLog: {0}", infoLog.data());
			NAV_ERROR("OpenGL Fragment Shader compilation error!");
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		m_renderID = glCreateProgram();

		// Attach our shaders to our program
		glAttachShader(m_renderID, vertexShader);
		glAttachShader(m_renderID, fragmentShader);

		// Link our program
		glLinkProgram(m_renderID);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(m_renderID, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(m_renderID, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_renderID, maxLength, &maxLength, &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(m_renderID);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			// Use the infoLog as you see fit.
			NAV_ERROR("Program InfoLog: {0}", infoLog.data());
			NAV_ERROR("OpenGL Shader link error!");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(m_renderID, vertexShader);
		glDetachShader(m_renderID, fragmentShader);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_renderID);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_renderID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}
	void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		auto loc = glGetUniformLocation(m_renderID, name.c_str());
		glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(matrix));
	}

}