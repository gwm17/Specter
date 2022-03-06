/*
	RenderCommand.cpp
	Render call abstraction. Exists to allow Navigator the felxibility to be backend api agnostic, though currently
	only OpenGL is implemented. Entirely based upon the work done by @TheCherno in his game engine series. See his content for more details.

	GWM -- Feb 2022
*/
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Navigator {
	RendererAPI* RenderCommand::s_api = new OpenGLRendererAPI();
}