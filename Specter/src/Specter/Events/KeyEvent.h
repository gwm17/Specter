/*
	KeyEvent.h
	Events related to key presses. Again, based on @TheCherno's work.

	GWM -- Feb 2022
*/
#ifndef KEY_EVENT_H
#define KEY_EVENT_H

#include "Event.h"

namespace Specter {

	//Since so many key related events, have a base
	class KeyEvent : public Event
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

	class KeyPressedEvent : public KeyEvent
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

	class KeyReleasedEvent : public KeyEvent 
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

	class KeyTypedEvent : public KeyEvent 
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