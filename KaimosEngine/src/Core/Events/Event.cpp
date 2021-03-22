#include "kspch.h"
#include "Event.h"

namespace Kaimos {

	EVENT_CATEGORY operator ~(EVENT_CATEGORY right_ev)
	{
		return static_cast<EVENT_CATEGORY>(~static_cast<std::underlying_type<EVENT_CATEGORY>::type>(right_ev));
	}

	EVENT_CATEGORY operator |(EVENT_CATEGORY left_ev, EVENT_CATEGORY right_ev)
	{
		return static_cast<EVENT_CATEGORY>(static_cast<std::underlying_type<EVENT_CATEGORY>::type>(left_ev) | static_cast<std::underlying_type<EVENT_CATEGORY>::type>(right_ev));
	}

	EVENT_CATEGORY operator &(EVENT_CATEGORY left_ev, EVENT_CATEGORY right_ev)
	{
		return static_cast<EVENT_CATEGORY>(static_cast<std::underlying_type<EVENT_CATEGORY>::type>(left_ev) & static_cast<std::underlying_type<EVENT_CATEGORY>::type>(right_ev));
	}

	EVENT_CATEGORY operator ^(EVENT_CATEGORY left_ev, EVENT_CATEGORY right_ev)
	{
		return static_cast<EVENT_CATEGORY>(static_cast<std::underlying_type<EVENT_CATEGORY>::type>(left_ev) ^ static_cast<std::underlying_type<EVENT_CATEGORY>::type>(right_ev));
	}

	EVENT_CATEGORY& operator |=(EVENT_CATEGORY& left_ev, EVENT_CATEGORY right_ev)
	{
		left_ev = static_cast<EVENT_CATEGORY>(static_cast<std::underlying_type<EVENT_CATEGORY>::type>(left_ev) | static_cast<std::underlying_type<EVENT_CATEGORY>::type>(right_ev));
		return left_ev;
	}

	EVENT_CATEGORY& operator &=(EVENT_CATEGORY& left_ev, EVENT_CATEGORY right_ev)
	{
		left_ev = static_cast<EVENT_CATEGORY>(static_cast<std::underlying_type<EVENT_CATEGORY>::type>(left_ev) & static_cast<std::underlying_type<EVENT_CATEGORY>::type>(right_ev));
		return left_ev;
	}

	EVENT_CATEGORY& operator ^=(EVENT_CATEGORY& left_ev, EVENT_CATEGORY right_ev)
	{
		left_ev = static_cast<EVENT_CATEGORY>(static_cast<std::underlying_type<EVENT_CATEGORY>::type>(left_ev) ^ static_cast<std::underlying_type<EVENT_CATEGORY>::type>(right_ev));
		return left_ev;
	}
}
