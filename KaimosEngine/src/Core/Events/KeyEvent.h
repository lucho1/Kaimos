#ifndef _KEYEVENT_H_
#define	_KEYEVENT_H_

#include "Event.h"
#include "Core/Input/KaimosInputCodes.h"

namespace Kaimos {

	class KeyEvent : public Event
	{
	public:

		inline KEYCODE GetKeyCode() const { return m_KeyCode; }
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY_KEYBOARD | EVENT_CATEGORY_INPUT)

	protected:

		KeyEvent(const KEYCODE keycode) : m_KeyCode(keycode) {}
		KEYCODE m_KeyCode;
	};


	class KeyPressedEvent : public KeyEvent
	{
	public:

		KeyPressedEvent(const KEYCODE keycode, const uint repeatCount) : KeyEvent(keycode), m_RepeatCount(repeatCount) {}

		inline uint GetRepeatCount() const { return m_RepeatCount; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << "(" << m_RepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KEY_PRESSED)

	private:
		uint m_RepeatCount;
	};


	class KeyReleasedEvent : public KeyEvent
	{
	public:

		KeyReleasedEvent(const KEYCODE keycode) : KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent" << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KEY_RELEASED)
	};


	class KeyTypedEvent : public KeyEvent
	{
	public:

		KeyTypedEvent(const KEYCODE keycode) : KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KEY_TYPED)
	};
}

#endif
