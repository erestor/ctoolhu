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

			//random generator engine
			class MersenneEngine : public std::mt19937 {

				MersenneEngine(const MersenneEngine &) = delete;
				MersenneEngine &operator =(const MersenneEngine &) = delete;

				friend struct Loki::CreateUsingNew<MersenneEngine>;

				MersenneEngine() {} //prevents instantiation outside the singleton
			};

			typedef Loki::SingletonHolder<MersenneEngine> SingleMersenneEngine;

		}; //ns Private

	}; //ns Random

}; //ns Ctoolhu

#endif
