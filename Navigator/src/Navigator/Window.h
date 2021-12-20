#ifndef WINDOW_H
#define WINDOW_H

#include "NavCore.h"
#include "Events/Event.h"

namespace Navigator {

	class WindowProperties
	{
	public:
		unsigned int width, height;
		std::string name;

		WindowProperties(const std::string& title="Navigator", unsigned int w=1280, unsigned int h=720) :
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