//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_random_engine_included_
#define _ctoolhu_random_engine_included_

#include "../singleton/holder.hpp"
#include <mutex>
#include <random>

namespace Ctoolhu::Random::Private {

	//Random generator engine shared by all the generators.
	//The generators can be used from multiple threads (e.g. jobs in a thread pool),
	//so access to the underlying engine is serialised.
	template <class Engine>
	class LockedEngine {

	  public:

		using result_type = typename Engine::result_type;

		static constexpr result_type min() { return Engine::min(); }
		static constexpr result_type max() { return Engine::max(); }

		result_type operator()()
		{
			std::lock_guard lock{_mutex};
			return _engine();
		}

		void seed(result_type value = Engine::default_seed)
		{
			std::lock_guard lock{_mutex};
			_engine.seed(value);
		}

	  private:

		Engine _engine;
		std::mutex _mutex;
	};

	using RandomEngine = LockedEngine<std::mt19937>; //random generator engine
	using SingleRandomEngine = Singleton::Holder<RandomEngine>;

}

#endif //file guard
