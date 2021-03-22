#ifndef _MOUSEEVENT_H_
#define	_MOUSEEVENT_H_

#include "Event.h"
#include "Core/Input/KaimosInputCodes.h"

namespace Kaimos {

	class MouseMovedEvent : public Event
	{
	public:

		MouseMovedEvent(const float x, const float y) : m_MouseX(x), m_MouseY(y) {}

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MOUSE_DISPLACED)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY::MOUSE | EVENT_CATEGORY::INPUT)

	private:
		float m_MouseX, m_MouseY;
	};


	class MouseScrolledEvent : public Event
	{
	public:

		MouseScrolledEvent(const float xoffset, const float yoffset) : m_XOffset(xoffset), m_YOffset(yoffset) {}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << m_XOffset << ", " << m_YOffset;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MOUSE_SCROLLED)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY::MOUSE | EVENT_CATEGORY::INPUT)

	private:
		float m_XOffset, m_YOffset;
	};


	class MouseButtonEvent : public Event
	{
	public:

		inline MOUSE_CODE GetMouseButton() const { return m_Button; }
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY::MOUSE | EVENT_CATEGORY::INPUT | EVENT_CATEGORY::MOUSE_BUTTON)

	protected:

		MouseButtonEvent(const MOUSE_CODE button) : m_Button(button) {}
		MOUSE_CODE m_Button;
	};


	class MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:

		MouseButtonPressedEvent(const MOUSE_CODE button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}
		
		EVENT_CLASS_TYPE(MOUSE_BUTTON_PRESSED)
	};


	class MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:

		MouseButtonReleasedEvent(const MOUSE_CODE button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MOUSE_BUTTON_RELEASED)
	};
}

#endif //_MOUSEEVENT_H_
