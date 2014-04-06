//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_event_aggregator_included_
#define _ctoolhu_event_aggregator_included_

#include <boost/signals2.hpp>
#include <loki/Singleton.h>

namespace Ctoolhu {
	
	namespace Event {

		namespace Private {

			//facilitates event handling between unrelated publishers and subscribers
			template <class Event>
			class Aggregator {

			  public:

				typedef boost::signals2::signal<void (Event *)> signal_type;
				typedef typename signal_type::slot_type slot_type; 
			  
				boost::signals2::connection Subscribe(const slot_type &handler)
				{
					return _signal.connect(handler);
				}

				void Fire(Event &e)
				{
					_signal(&e);
				}

			  private:

				friend struct Loki::CreateUsingNew<Aggregator>;

				Aggregator() {} //prevents instantiation outside the singleton

				//prevent copying and assignment
				Aggregator(const Aggregator &);
				Aggregator &operator =(const Aggregator &);

				signal_type _signal;
			};

			template <class Event>
			class SingleAggregator : public Loki::SingletonHolder<Private::Aggregator<Event>> {};

		}; //ns Private

	}; //ns Event

}; //ns Ctoolhu

#endif
