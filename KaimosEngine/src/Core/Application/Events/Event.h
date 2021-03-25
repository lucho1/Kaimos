#ifndef _EVENT_H_
#define	_EVENT_H_

#include <functional>
#include "Core/Core.h"

namespace Kaimos {

	// Events are following the Blocking Strategy: when an event occurs, it gets dispatched and dealt inmediately
	// A better way it could be to buffer events and process them during an "Event" part/step of the Update stage
	enum class EVENT_TYPE
	{
		NONE = 0,
		WINDOW_CLOSE, WINDOW_RESIZE, WINDOW_FOCUS, WINDOW_LOST_FOCUS, WINDOW_MOVED,
		APP_TICK, APP_UPDATE, APP_RENDER,
		KEY_PRESSED, KEY_RELEASED, KEY_TYPED,
		MOUSE_BUTTON_PRESSED, MOUSE_BUTTON_RELEASED, MOUSE_DISPLACED, MOUSE_SCROLLED
	};

	enum class EVENT_CATEGORY
	{
		NONE = 0,
		APPLICATION		= BIT(0),
		INPUT			= BIT(1),
		KEYBOARD		= BIT(2),
		MOUSE			= BIT(3),
		MOUSE_BUTTON	= BIT(4)
	};

	EVENT_CATEGORY operator ~(EVENT_CATEGORY right_ev);
	EVENT_CATEGORY operator |(EVENT_CATEGORY left_ev, EVENT_CATEGORY right_ev);
	EVENT_CATEGORY operator &(EVENT_CATEGORY left_ev, EVENT_CATEGORY right_ev);
	EVENT_CATEGORY operator ^(EVENT_CATEGORY left_ev, EVENT_CATEGORY right_ev);
	EVENT_CATEGORY& operator |=(EVENT_CATEGORY& left_ev, EVENT_CATEGORY right_ev);
	EVENT_CATEGORY& operator &=(EVENT_CATEGORY& left_ev, EVENT_CATEGORY right_ev);
	EVENT_CATEGORY& operator ^=(EVENT_CATEGORY& left_ev, EVENT_CATEGORY right_ev);


#define EVENT_CLASS_TYPE(type)	inline static EVENT_TYPE GetStaticType()	{ return EVENT_TYPE::type; }\
								inline virtual EVENT_TYPE GetEventType()	const override { return GetStaticType(); }\
								inline virtual const char* GetName()		const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) inline virtual EVENT_CATEGORY GetCategoryFlags() const override { return (category); }


	// ----------------------- Classes -----------------------------------------------------
	class Event
	{
		friend class EventDispatcher;
	public:

		virtual ~Event() = default;

		virtual EVENT_TYPE GetEventType()			const = 0;
		virtual const char* GetName()				const = 0;
		virtual EVENT_CATEGORY GetCategoryFlags()	const = 0;
		inline virtual std::string ToString()		const { return GetName(); }


		inline bool IsHandled()						const { return m_Handled; }
		inline void SetHandled(bool handled)		{ m_Handled = handled; }

		inline bool IsInCategory(EVENT_CATEGORY category)	{ return static_cast<bool>(GetCategoryFlags() & category); }

	protected:
		bool m_Handled = false;
	};


	class EventDispatcher
	{
		//template<typename T>
		//using EventFn = std::function<bool(T&)>;

	public:

		EventDispatcher(Event& event) : m_Event(event) {}

		//template<typename T>
		//bool Dispatch(EventFn<T> func) --> Then m_Event.m_Handled = func(*(T*)&m_Event);
		// This is better than std::function (performance)... 'F' will be deduced by compiler
		template<typename T, typename F>
		bool Dispatch(const F& func) const
		{
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled |= func(static_cast<T&>(m_Event));
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

#endif //_EVENT_H_