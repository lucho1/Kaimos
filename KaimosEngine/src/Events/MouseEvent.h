#ifndef _MOUSEEVENT_H_
#define	_MOUSEEVENT_H_

#include "Event.h"

namespace Kaimos {

	class KAIMOS_API MouseMovedEvent : public Event
	{
	public:

		MouseMovedEvent(float x, float y) : m_MouseX(x), m_MouseY(y) {}

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MOUSE_DISPLACED)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)

	private:
		float m_MouseX = 0.0f, m_MouseY = 0.0f;
	};


	class KAIMOS_API MouseScrolledEvent : public Event
	{
	public:

		MouseScrolledEvent(float xOffset, float yOffset) : m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << m_XOffset << ", " << m_YOffset;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MOUSE_SCROLLED)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)

	private:

		float m_XOffset = 0.0f, m_YOffset = 0.0f;
	};


	class KAIMOS_API MouseButtonEvent : public Event
	{
	public:

		inline int GetMouseButton() const { return m_Button; }
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_MOUSE | EVENT_CATEGORY_INPUT)

	protected:

		MouseButtonEvent(int button) : m_Button(button) {}
		int m_Button = 0;
	};


	class KAIMOS_API MouseButtonPressedEvent : public MouseButtonEvent
	{
	public:

		MouseButtonPressedEvent(int button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}
		
		EVENT_CLASS_TYPE(MOUSE_BUTTON_PRESSED)
	};


	class KAIMOS_API MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:

		MouseButtonReleasedEvent(int button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MOUSE_BUTTON_RELEASED)
	};

}

#endif