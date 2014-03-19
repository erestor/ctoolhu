//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_random_engine_included_
#define _ctoolhu_random_engine_included_

#include <boost/random/mersenne_twister.hpp>
#include <loki/singleton.h>

namespace Ctoolhu {

	namespace Random {
	
		namespace Private {

			//random generator engine
			class MersenneEngine
				: public boost::mt19937 {

			  private: 

				friend struct Loki::CreateUsingNew<MersenneEngine>;

				MersenneEngine() {}; //prevents instantiation outside the singleton
			};

			typedef Loki::SingletonHolder<MersenneEngine> SingleMersenneEngine;

		}; //ns Private

	}; //ns Random

}; //ns Ctoolhu

#endif
