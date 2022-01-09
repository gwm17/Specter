#ifndef EVENT_H
#define EVENT_H

#include "Navigator/NavCore.h"

namespace Navigator {

	enum class NAV_API EventType
	{
		None=0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseScrolled, MouseMoved,
		AppUpdate,
		PhysicsStart, PhysicsStop
	};

	enum NAV_API EventCategory
	{
		EventCategoryNone=0,
		EventCategoryApp=BIT(0),
		EventCategoryInput=BIT(1),
		EventCategoryKey=BIT(2),
		EventCategoryMouse=BIT(3),
		EventCategoryWindow=BIT(4),
		EventCategoryPhysics=BIT(5),
	};

	//Some function generation automation to reduce code written for all events
	#define EVENT_CATEGORY_SETUP(cat) virtual int GetCategoryFlags() const override { return cat; }

	#define EVENT_TYPE_SETUP(type) static EventType GetStaticType() { return EventType::type; } \
								   virtual EventType GetEventType() const override { return GetStaticType(); } \
								   virtual const char* GetName() const override { return #type; }

	class NAV_API Event
	{
		friend class EventDispatcher;
	public:
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }
		inline bool IsCategory(EventCategory cat) const { return GetCategoryFlags() & cat; }
		bool handledFlag = false;
	};

	class NAV_API EventDispatcher
	{
		template<typename T>
			using EventFunc = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& e) :
			m_event(e)
		{
		}

		template<typename T>
		bool Dispatch(EventFunc<T> function)
		{
			if(m_event.GetEventType() == T::GetStaticType())
			{
				m_event.handledFlag = function(*(T*)&m_event);
				return true;
			}
			else
			{
				return false;
			}
		}

	private:
		Event& m_event;
	};

	NAV_API inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}

#endif