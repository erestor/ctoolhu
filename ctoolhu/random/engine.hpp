//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_random_engine_included_
#define _ctoolhu_random_engine_included_

#include <random>
#include <loki/Singleton.h>

namespace Ctoolhu {

	namespace Random {
	
		namespace Private {
			
			typedef std::mt19937 RandomEngine; //random generator engine
			typedef Loki::SingletonHolder<RandomEngine> SingleRandomEngine;

		}; //ns Private

	}; //ns Random

}; //ns Ctoolhu

#endif
