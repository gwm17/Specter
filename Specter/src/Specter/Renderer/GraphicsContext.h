/*
	GraphicsContext.h
	Abstraction of graphical rendering context. Exists to allow Specter the felxibility to be backend api agnostic, though currently
	only OpenGL is implemented. Entirely based upon the work done by @TheCherno in his game engine series. See his content for more details.

	GWM -- Feb 2022
*/
#ifndef GRAPHICS_CONTEXT_H
#define GRAPHICS_CONTEXT_H

namespace Specter {

	class GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;
	};

}

#endif