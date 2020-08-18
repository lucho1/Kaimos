#ifndef _KEYEVENT_H_
#define	_KEYEVENT_H_

#include "Event.h"

namespace Kaimos {

	class KAIMOS_API KeyEvent : public Event
	{
	public:

		inline int GetKeyCode() const { return m_KeyCode; }
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT)

	protected:

		KeyEvent(int keycode) {}
		int m_KeyCode;
	};


	class KAIMOS_API KeyPressedEvent : public KeyEvent
	{
	public:

		KeyPressedEvent(int keycode, int repeatCount) : KeyEvent(keycode), m_RepeatCount(repeatCount) {}

		inline int GetRepeatCount() const { return m_RepeatCount; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << "(" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KEY_PRESSED)

	private:
		int m_RepeatCount;
	};


	class KAIMOS_API KeyReleasedEvent : public KeyEvent
	{
	public:

		KeyReleasedEvent(int keycode) : KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent" << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KEY_RELEASED)
	};
}

#endif