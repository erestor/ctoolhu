//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_event_subscriber_included_
#define _ctoolhu_event_subscriber_included_

#include "aggregator.hpp"
#include <boost/signals2.hpp>

namespace Ctoolhu {

	namespace Event {

		//provides effortless access to subscription of an event handler to the event aggregator -
		//- inheriting from this class will subscribe event handler automatically upon object construction
		//
		//Doesn't support multiple handlers for one event in one child class
		template <
			class Event,			//event we want to subscribe for
			class HandlerHolder		//typically the child class
		>
		class Subscriber {

			typedef boost::signals2::connection connection_type;
			using handler_type = void (HandlerHolder::*)(Event *);

		  protected:

			//automatically subscribes method 'on(Event *)' as a handler
			Subscriber()
			{
				_connection = Subscribe();
			}

			//manually subscribes a given custom handler
			Subscriber(handler_type handler)
			{
				_connection = Subscribe(handler);
			}

			Subscriber(const Subscriber &) = delete;
			Subscriber &operator =(const Subscriber &) = delete;

			~Subscriber()
			{
				Unsubscribe();
			}

		  private:

			connection_type Subscribe()
			{
				return Private::SingleAggregator<Event>::Instance().Subscribe([this](Event *e) {
					static_cast<HandlerHolder *>(this)->on(e);
				});
			}

			connection_type Subscribe(handler_type handler)
			{
				return Private::SingleAggregator<Event>::Instance().Subscribe([this, handler](Event *ev) {
					(static_cast<HandlerHolder *>(this)->*handler)(ev);
				});
			}

			void Unsubscribe()
			{
				if (_connection.connected())
					_connection.disconnect();
			}

			connection_type _connection;
		};

	}; //ns Event

}; //ns Ctoolhu

#endif
