#ifndef KEY_EVENT_H
#define KEY_EVENT_H

#include "Navigator/NavCore.h"
#include "Event.h"

namespace Navigator {

	class NAV_API KeyEvent : public Event
	{
	public:
		inline int GetKeycode() const { return m_keycode; }
		EVENT_CATEGORY_SETUP(EventCategoryKey | EventCategoryInput)
	protected:
		KeyEvent(int code) :
			m_keycode(code)
		{
		}

		int m_keycode;

	};

	class NAV_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int code, int count) :
			KeyEvent(code), m_repeatCount(count)
		{
		}

		EVENT_TYPE_SETUP(KeyPressed)

		inline int GetRepeatCount() const { return m_repeatCount; }

		std::string ToString() const override
		{
			std::stringstream stream;
			stream << GetName() << " with code "<<m_keycode << " pressed "<<m_repeatCount<<" times.";
			return stream.str();
		}


	private:
		int m_repeatCount;
	};

	class NAV_API KeyReleasedEvent : public KeyEvent 
	{
	public:
		KeyReleasedEvent(int code) :
			KeyEvent(code)
		{
		};

		EVENT_TYPE_SETUP(KeyReleased)

		std::string ToString() const override 
		{
			std::stringstream ss;
			ss << GetName() << " with code " << m_keycode;
			return ss.str();
		}

	};

	class NAV_API KeyTypedEvent : public KeyEvent 
	{
	public:
		KeyTypedEvent(int code) :
			KeyEvent(code)
		{
		};

		EVENT_TYPE_SETUP(KeyTyped)

		unsigned int GetCharacter() { return (unsigned int)m_keycode; }

		std::string ToString() const override 
		{
			std::stringstream ss;
			ss << GetName() << " with code " << m_keycode;
			return ss.str();
		}

	};
}

#endif