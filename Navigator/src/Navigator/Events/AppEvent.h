#ifndef APP_EVENT_H
#define APP_EVENT_H

#include "Event.h"

namespace Navigator {

	class WindowCloseEvent : public Event 
	{
	public:
		WindowCloseEvent() {};

		EVENT_CATEGORY_SETUP(EventCategoryApp)
		EVENT_TYPE_SETUP(WindowClose)
	};

	class WindowResizeEvent : public Event 
	{
	public:
		WindowResizeEvent(float x, float y) :
			m_xSize(x), m_ySize(y)
		{
		}

		inline float GetXSize() { return m_xSize; }
		inline float GetYSize() { return m_ySize; }
		std::string ToString() const override 
		{
			std::stringstream ss;
			ss << GetName() << " to size: (" << m_xSize << ", " << m_ySize << ")";
			return ss.str();
		}

		EVENT_CATEGORY_SETUP(EventCategoryApp)
		EVENT_TYPE_SETUP(WindowResize)

	private:
		float m_xSize, m_ySize;
	};

	class AppUpdateEvent : public Event 
	{
	public:
		AppUpdateEvent() = default;

		EVENT_CATEGORY_SETUP(EventCategoryApp)
		EVENT_TYPE_SETUP(AppUpdate)
	};

}

#endif