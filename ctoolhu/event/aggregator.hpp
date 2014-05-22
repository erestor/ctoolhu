//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_event_aggregator_included_
#define _ctoolhu_event_aggregator_included_

#include <boost/signals2.hpp>
#include <loki/Singleton.h>

namespace Ctoolhu {
	
	namespace Event {

		typedef boost::signals2::connection connection_type;

		namespace Private {

			//facilitates event handling between unrelated publishers and subscribers
			template <class Event>
			class Aggregator {

			  public:

				Aggregator(const Aggregator &) = delete;
				Aggregator &operator =(const Aggregator &) = delete;

				typedef boost::signals2::signal<void (Event *)> signal_type;
				typedef typename signal_type::slot_type slot_type; 
			  
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
			using SingleAggregator = Loki::SingletonHolder<Private::Aggregator<Event>>;

		} //ns Private

	} //ns Event

} //ns Ctoolhu

#endif
