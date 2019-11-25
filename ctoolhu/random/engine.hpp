//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_random_engine_included_
#define _ctoolhu_random_engine_included_

#include "../singleton/holder.hpp"
#include <random>

namespace Ctoolhu::Random::Private {

	using RandomEngine = std::mt19937; //random generator engine
	using SingleRandomEngine = Singleton::Holder<RandomEngine>;

}

#endif //file guard
