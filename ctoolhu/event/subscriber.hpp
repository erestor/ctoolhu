//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_event_subscriber_included_
#define _ctoolhu_event_subscriber_included_

#include "aggregator.hpp"
#include <boost/signals2.hpp>

namespace Ctoolhu::Event {

	//provides effortless subscription of event handlers for events given as template parameters to the event aggregator -
	//- inheriting from this class will subscribe default event handlers automatically upon object construction
	//- compile-time error will occurr if the handler isn't implemented
	template <class... EventTypes>
	class Subscriber {

	  public:

		virtual ~Subscriber() = default;

	  protected:

		void on(); //syntactic sugar to allow the using clause in the recursion
	};

	template <class Event, class... EventTypes> //events we're subscribing for
	class Subscriber<Event, EventTypes...> : Subscriber<EventTypes...> {

		using base_t = Subscriber<EventTypes...>;

	  protected:

		//automatically subscribes method 'on(Event *)' as a handler
		Subscriber()
		{
			_connection = Private::SingleAggregator<Event>::Instance().Subscribe([this](Event *e) {
				on(e);
			});
		}

		~Subscriber()
		{
			if (_connection.connected())
				_connection.disconnect();
		}

		Subscriber(const Subscriber &) = delete;
		Subscriber(Subscriber &&) = delete;
		Subscriber &operator=(const Subscriber &) = delete;
		Subscriber &operator=(Subscriber &&) = delete;

		using base_t::on;
		virtual void on(Event *) = 0;	//override this in the child class

	  private:

		connection_t _connection;
	};

} //ns Ctoolhu::Event

#endif
