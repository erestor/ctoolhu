//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_event_subscriber_included_
#define _ctoolhu_event_subscriber_included_

#include "aggregator.hpp"
#include <boost/bind.hpp>
#include <boost/signals2.hpp>

namespace Ctoolhu {

	namespace Event {

		//provides effortless access to subscription of an event handler to the event aggregator
		//
		//inheriting from this class will subscribe event handler automatically upon object construction
		//Event is the event we want to subscribe for, HandlerHolder is typically the child class
		//
		//Doesn't support multiple handlers for one event in one child class
		template <class Event, class HandlerHolder>
		class Subscriber {

		  protected:

			//automatically subscribes method 'on(Event *)' as a handler
			Subscriber()
			{
				_connection = Private::SingleAggregator<Event>::Instance().Subscribe(
					boost::bind((void (HandlerHolder::*)(Event *))&HandlerHolder::on, static_cast<HandlerHolder *>(this), _1));
			}

			//manually subscribes a given custom handler
			Subscriber(void (HandlerHolder::*handler)(Event *))
			{
				_connection = Private::SingleAggregator<Event>::Instance().Subscribe(
					boost::bind(handler, static_cast<HandlerHolder *>(this), _1));
			}

			~Subscriber()
			{
				if (_connection.connected())
					_connection.disconnect();
			}

		  private:

			boost::signals2::connection _connection;
		};

	}; //ns Event

}; //ns Ctoolhu

#endif
