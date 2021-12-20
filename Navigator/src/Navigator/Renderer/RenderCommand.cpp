#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Navigator {
	RendererAPI* RenderCommand::s_api = new OpenGLRendererAPI();
}