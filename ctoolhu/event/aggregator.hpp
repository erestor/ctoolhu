//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_event_aggregator_included_
#define _ctoolhu_event_aggregator_included_

#include "../singleton/holder.hpp"
#include <loki/Singleton.h>
#include <boost/signals2.hpp>

namespace Ctoolhu {
	
	namespace Event {

		using connection_type = boost::signals2::connection;

		namespace Private {

			//facilitates event handling between unrelated publishers and subscribers
			template <class Event>
			class Aggregator {

			  public:

				Aggregator(const Aggregator &) = delete;
				Aggregator &operator =(const Aggregator &) = delete;

				using signal_type = boost::signals2::signal<void (Event *)>;
				using slot_type = typename signal_type::slot_type;
			  
				connection_type Subscribe(const slot_type &handler)
				{
					return _signal.connect(handler);
				}

				void Fire(const Event &e) const
				{
					_signal(const_cast<Event *>(&e)); //TODO is there a type-safe way? I.e., can we have more than one signal_type?
				}

				void Fire(Event &e) const
				{
					_signal(&e);
				}

			  private:

				friend struct Loki::CreateUsingNew<Aggregator>;
				Aggregator() = default;

				signal_type _signal;
			};

			template <class Event>
			using SingleAggregator = Singleton::Holder<Private::Aggregator<Event>>;

		} //ns Private

	} //ns Event

} //ns Ctoolhu

#endif
