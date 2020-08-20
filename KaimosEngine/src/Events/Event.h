#ifndef _EVENT_H_
#define	_EVENT_H_

#include "Core/Core.h"

namespace Kaimos {

	// Events are following the Blocking Strategy: when an event occurs, it gets dispatched and dealt inmediately
	// A better way it could be to buffer events and process them during an "Event" part/step of the Update stage

	enum class EVENT_TYPE
	{
		NONE = 0,
		WINDOW_CLOSE, WINDOW_RESIZE, WINDOW_FOCUS, WINDOW_LOST_FOCUS, WINDOW_MOVED,
		APP_TICK, APP_UPDATE, APP_RENDER,
		KEY_PRESSED, KEY_RELEASED, KEY_REPEAT,
		MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED, MOUSE_BUTTON_REPEAT, MOUSE_DISPLACED, MOUSE_SCROLLED
	};

	enum EVENT_CATEGORY
	{
		NONE = 0,
		EVENT_CATEGORY_APPLICATION		= BIT(0),
		EVENT_CATEGORY_INPUT			= BIT(1),
		EVENT_CATEGORY_KEYBOARD			= BIT(2),
		EVENT_CATEGORY_MOUSE			= BIT(3),
		EVENT_CATEGORY_MOUSE_BUTTON		= BIT(4)
	};

#define EVENT_CLASS_TYPE(type)	static EVENT_TYPE GetStaticType() { return EVENT_TYPE::##type; }\
								virtual EVENT_TYPE GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	class KAIMOS_API Event
	{
		friend class EventDispatcher;
	public:

		virtual EVENT_TYPE GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EVENT_CATEGORY category) { return GetCategoryFlags() & category; }

		inline bool IsHandled() const { return m_Handled; }

	protected:
		bool m_Handled = false;
	};

	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public:

		EventDispatcher(Event& event) : m_Event(event) {}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};


	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}

#endif