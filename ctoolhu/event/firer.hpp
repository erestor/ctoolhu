//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_event_firer_included_
#define _ctoolhu_event_firer_included_

#include "aggregator.hpp"
#include <loki/Singleton.h>

namespace Ctoolhu {

	namespace Event {

		//use this class to fire events effortlessly
		class Firer {

		  public:

			//for events without parameters ('singleton events')
			template <class Event>
			static void Fire()
			{
				struct Empty {};
				static_assert(sizeof(Event) == sizeof(Empty), "can't fire events with parameters by type only");
				Private::SingleAggregator<Event>::Instance().Fire(Loki::SingletonHolder<Event>::Instance());
			}

			//for full events
			template <class Event>
			static void Fire(Event &e)
			{
				Private::SingleAggregator<Event>::Instance().Fire(e);
			}
		};

	}; //ns Event

}; //ns Ctoolhu

#endif
