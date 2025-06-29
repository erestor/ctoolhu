//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_event_free_subscriber_included_
#define _ctoolhu_event_free_subscriber_included_

#include "aggregator.hpp"
#include <boost/signals2.hpp>

namespace Ctoolhu::Event {

	//provides means to attach handlers to events without the need to inherit from a base class
	class FreeSubscriber {

	  public:

		FreeSubscriber() = default;
		FreeSubscriber(const FreeSubscriber &) = delete;
		FreeSubscriber &operator=(const FreeSubscriber &) = delete;

		FreeSubscriber(FreeSubscriber &&src) noexcept
		{
			_connections = std::move(src._connections);
			src._connections.clear();
		}

		FreeSubscriber &operator=(FreeSubscriber &&src) noexcept
		{
			_connections = std::move(src._connections);
			src._connections.clear();
			return *this;
		}

		~FreeSubscriber()
		{
			for (auto &conn : _connections) {
				if (conn.connected())
					conn.disconnect();
			}
		}

		//subscribes a member handler to a stateful event
		template <typename Event, typename T>
		connection_t Subscribe(T *obj, void (T::*handler)(Event *))
		{
			auto conn = Private::SingleAggregator<Event>::Instance().Subscribe(
				[obj, handler](Event *e) {
					(obj->*handler)(e);
				}
			);
			_connections.push_back(conn);
			return conn;
		}

		//subscribes a member handler to a stateless event
		template <typename Event, typename T>
		connection_t Subscribe(T *obj, void (T::*handler)())
		{
			auto conn = Private::SingleAggregator<Event>::Instance().Subscribe(
				[obj, handler](Event *) {
					(obj->*handler)();
				}
			);
			_connections.push_back(conn);
			return conn;
		}

		//subscribes a generic handler to an event
		template <typename Event>
		connection_t Subscribe(auto &&handler)
		{
			auto conn = Private::SingleAggregator<Event>::Instance().Subscribe(
				[handler = std::forward<decltype(handler)>(handler)](Event *e) {
					handler(e);
				}
			);
			_connections.push_back(conn);
			return conn;
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

	  private:

		std::vector<connection_t> _connections;
	};

} //ns Ctoolhu::Event

#endif
