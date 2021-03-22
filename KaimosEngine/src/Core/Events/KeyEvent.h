#ifndef _KEYEVENT_H_
#define	_KEYEVENT_H_

#include "Event.h"
#include "Core/Input/KaimosInputCodes.h"

namespace Kaimos {

	class KeyEvent : public Event
	{
	public:

		inline KEY_CODE GetKeyCode() const { return m_KeyCode; }
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY::KEYBOARD | EVENT_CATEGORY::INPUT)

	protected:

		KeyEvent(const KEY_CODE keycode) : m_KeyCode(keycode) {}
		KEY_CODE m_KeyCode;
	};


	class KeyPressedEvent : public KeyEvent
	{
	public:

		KeyPressedEvent(const KEY_CODE keycode, const uint repeat_count) : KeyEvent(keycode), m_RepeatCount(repeat_count) {}

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

		KeyReleasedEvent(const KEY_CODE keycode) : KeyEvent(keycode) {}

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

		KeyTypedEvent(const KEY_CODE keycode) : KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KEY_TYPED)
	};
}

#endif //_KEYEVENT_H_
