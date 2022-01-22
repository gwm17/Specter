#include "OpenGLRendererAPI.h"

#include "glad/glad.h"

namespace Navigator {

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color_array)
	{
		glClearColor(color_array[0], color_array[1], color_array[2], color_array[3]);
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& array)
	{
		glDrawElements(GL_TRIANGLES, array->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
}