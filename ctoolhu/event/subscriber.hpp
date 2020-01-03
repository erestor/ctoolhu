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

		virtual void on() {}
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
		Subscriber &operator =(const Subscriber &) = delete;

		using base_t::on;
		virtual void on(Event *) = 0;	//override this in the child class

	  private:

		connection_t _connection;
	};


	//provides means to attach any number of any handlers to the event
	template <
		class Event,			//event we want to subscribe for
		class HandlerHolder		//class defining the method for handling the event, typically the child class
	>
	class ManualSubscriber {

	  public:

		using handler_type = void (HandlerHolder::*)(Event *);

		//subscribes a given custom handler which is a member method of the child class
		connection_t Subscribe(handler_type handler)
		{
			return _store(Private::SingleAggregator<Event>::Instance().Subscribe([this](Event *e) {
				(static_cast<HandlerHolder *>(this)->*handler)(e);
			}));
		}

		//subscribes a handler of some other type
		template <typename OtherHandler>
		connection_t Subscribe(const OtherHandler &handler)
		{
			return _store(Private::SingleAggregator<Event>::Instance().Subscribe([&handler](Event *e) {
				handler(e);
			}));
		}

		//unsubscribes custom handler by connection handle returned by Subscribe
		void Unsubscribe(const connection_t &conn)
		{
			auto it = std::find(_connections.begin(), _connections.end(), conn);
			if (it != _connections.end()) {
				if (it->connected())
					it->disconnect();

				_connections.erase(it);
			}
		}

	  protected:

		ManualSubscriber() = default;

		explicit ManualSubscriber(handler_type handler)
		{
			Subscribe(handler);
		}

		~ManualSubscriber()
		{
			for (auto &conn : _connections) {
				if (conn.connected())
					conn.disconnect();
			}
		}

		ManualSubscriber(const ManualSubscriber &) = delete;
		ManualSubscriber &operator =(const ManualSubscriber &) = delete;

	  private:

		connection_t _store(const connection_t &conn)
		{
			_connections.push_back(conn);
			return conn;
		}

		std::vector<connection_t> _connections;
	};

} //ns Ctoolhu::Event

#endif
