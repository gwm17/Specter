/*
	Window.h

	Abstract class for a window in Specter. Based entirely upon the work of @TheCherno, see his Hazel repository. This exists primarily to allow for the option
	to extend Specter to other rendering backends, most likely Metal, or potentially DX12. See code in Platform for specific implementations (currently only for OpenGL).

	GWM -- Feb 2022
*/
#ifndef WINDOW_H
#define WINDOW_H

#include "SpecCore.h"
#include "Specter/Events/Event.h"

namespace Specter {

	class WindowProperties
	{
	public:
		unsigned int width, height;
		std::string name;

		WindowProperties(const std::string& title="Specter", unsigned int w=1280, unsigned int h=720) :
			width(w), height(h), name(title)
		{
		}

	};

	class Window
	{
	public:

		using EventCallbackFunc = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual std::string GetName() const = 0;

		virtual void SetEventCallback(const EventCallbackFunc& function) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const =0;

		static Window* Create(const WindowProperties& props = WindowProperties());
	};
}

#endif