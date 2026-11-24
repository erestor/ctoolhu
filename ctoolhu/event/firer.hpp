//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_event_firer_included_
#define _ctoolhu_event_firer_included_

#include "aggregator.hpp"
#include <type_traits>

namespace Ctoolhu::Event {

	//for firing events without parameters ('singleton events')
	template <class Event>
	void Fire()
	{
		static_assert(std::is_empty_v<Event>, "can't fire events with parameters by type only");
		static Event e; //an empty event carries no data, a shared instance is enough
		Private::SingleAggregator<Event>::Instance().Fire(e);
	}

	//for firing const events with data
	template <class Event>
	void Fire(const Event &e)
	{
		Private::SingleAggregator<Event>::Instance().Fire(e);
	}

	//for firing events with data expecting some data back
	template <class Event>
	void Fire(Event &e)
	{
		Private::SingleAggregator<Event>::Instance().Fire(e);
	}

} //ns Ctoolhu::Event

#endif
