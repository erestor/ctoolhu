//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_event_aggregator_included_
#define _ctoolhu_event_aggregator_included_

#include "../singleton/holder.hpp"
#include <boost/signals2.hpp>

namespace Ctoolhu::Event {

	using connection_t = boost::signals2::connection;

	namespace Private {

		//facilitates event handling between unrelated publishers and subscribers
		template <class Event>
		class Aggregator {

		  public:

			Aggregator(const Aggregator &) = delete;
			Aggregator &operator=(const Aggregator &) = delete;
			Aggregator(Aggregator &&) = delete;
			Aggregator &operator=(Aggregator &&) = delete;

			using signal_t = boost::signals2::signal<void (Event *)>;
			using slot_t = typename signal_t::slot_type;
			  
			connection_t Subscribe(const slot_t &handler)
			{
				return _signal.connect(handler);
			}

			void Fire(const Event &e) const
			{
				_signal(const_cast<Event *>(&e)); //TODO is there a way without const cast? I.e., can we have more than one signal_t?
			}

			void Fire(Event &e) const
			{
				_signal(&e);
			}

		  private:

			friend struct Loki::CreateUsingNew<Aggregator>;
			Aggregator() noexcept = default;

			signal_t _signal;
		};

		template <class Event>
		using SingleAggregator = Singleton::Holder<Private::Aggregator<Event>>;

	} //ns Private

} //ns Ctoolhu::Event

#endif
