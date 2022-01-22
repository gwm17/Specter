#include "Renderer.h"

namespace Navigator {


	void Renderer::BeginScene() {}

	void Renderer::EndScene() {}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& data)
	{
		shader->Bind();
		data->Bind();
		RenderCommand::DrawIndexed(data);
	}
}