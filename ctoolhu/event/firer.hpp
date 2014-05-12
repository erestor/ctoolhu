//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_event_firer_included_
#define _ctoolhu_event_firer_included_

#include "aggregator.hpp"
#include <loki/Singleton.h>
#include <type_traits>

namespace Ctoolhu {

	namespace Event {

		//for firing events without parameters ('singleton events')
		template <class Event>
		void Fire()
		{
			static_assert(std::is_empty<Event>::value, "can't fire events with parameters by type only");
			Private::SingleAggregator<Event>::Instance().Fire(Loki::SingletonHolder<Event>::Instance());
		}

		//for firing full events
		template <class Event>
		void Fire(Event &e)
		{
			Private::SingleAggregator<Event>::Instance().Fire(e);
		}

	} //ns Event

} //ns Ctoolhu

#endif
