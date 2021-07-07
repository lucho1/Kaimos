#ifndef _APPLICATIONEVENT_H_
#define _APPLICATIONEVENT_H_

#include "Event.h"
#include <sstream>

namespace Kaimos {

	class WindowResizeEvent : public Event
	{
	public:

		WindowResizeEvent(uint width, uint height) : m_Width(width), m_Height(height) {}

		inline uint GetWidth()	const { return m_Width; }
		inline uint GetHeight()	const { return m_Height; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(WINDOW_RESIZE)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY::APPLICATION)

	private:
		uint m_Width, m_Height;
	};


	class WindowCloseEvent : public Event
	{
	public:

		WindowCloseEvent() = default;
		EVENT_CLASS_TYPE(WINDOW_CLOSE)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY::APPLICATION)
	};

	class WindowDragDropEvent : public Event
	{
	public:
		WindowDragDropEvent(uint drop_count, const char* const paths[])
		{
			for (uint i = 0; i < drop_count; ++i)
				m_PathsDropped.push_back(paths[i]);
		}

		inline const std::vector<const char*>& GetPaths()	const { return m_PathsDropped; }
		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "WindowDragDropEvent: " << m_PathsDropped.size() << " - Paths Dropped:";
			for (uint i = 0; i < m_PathsDropped.size(); ++i)
				ss << "\n\t" << m_PathsDropped[i];

			return ss.str();
		}

		EVENT_CLASS_TYPE(WINDOW_DRAGDROP)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY::APPLICATION)

	private:
		std::vector<const char*> m_PathsDropped;
	};


	class AppTickEvent : public Event
	{
	public:

		AppTickEvent() = default;
		EVENT_CLASS_TYPE(APP_TICK)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY::APPLICATION)
	};


	class AppUpdateEvent : public Event
	{
	public:

		AppUpdateEvent() = default;
		EVENT_CLASS_TYPE(APP_UPDATE)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY::APPLICATION)
	};


	class AppRenderEvent : public Event
	{
	public:

		AppRenderEvent() = default;
		EVENT_CLASS_TYPE(APP_RENDER)
		EVENT_CLASS_CATEGORY(EVENT_CATEGORY::APPLICATION)
	};
}

#endif //_APPLICATIONEVENT_H_
