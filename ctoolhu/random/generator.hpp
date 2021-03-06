//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_random_generator_included_
#define _ctoolhu_random_generator_included_

#include "engine.hpp"
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_smallint.hpp>

#ifdef _DEBUG_RAND
#include <ctoolhu/event/events.h>
#include <ctoolhu/event/firer.hpp>
#include <string>
#endif

namespace Ctoolhu::Random {

	//shortcut for the generator template we'll be using
	template <class Distribution>
	using RandomGenerator = boost::variate_generator<Private::RandomEngine &, Distribution>;

	//generator with run-time bounds
	template <
		class Distribution,
		typename Boundary = int
	>
	class Generator : public RandomGenerator<Distribution> {
	
		using base_t = RandomGenerator<Distribution>;

	  public:

		//for number generators
		Generator(Boundary lower, Boundary upper)
			: base_t(Private::SingleRandomEngine::Instance(), Distribution(lower, upper)) {}

		//for bool generator
		Generator()
			: base_t(Private::SingleRandomEngine::Instance(), Distribution()) {}

#ifdef _DEBUG_RAND
		auto operator()()
		{
			auto res = this->base_t::operator()();
			Event::Fire(Event::Message{"random: " + std::to_string(res)});
			return res;
		}
#endif
	};

	//generator with compile-time bounds
	template <
		int LowerBound,
		int UpperBound,
		class Distribution = std::uniform_int_distribution<>
	>
	class StaticGenerator : public RandomGenerator<Distribution> {

	  public:

		StaticGenerator()
			: RandomGenerator<Distribution>(Private::SingleRandomEngine::Instance(), Distribution(LowerBound, UpperBound)) {}
	};

	//expose typical usages of the dynamic generator
	using IntGenerator =		Generator<std::uniform_int_distribution<>>;
	using SmallIntGenerator =	Generator<boost::uniform_smallint<>>;
	using FloatGenerator =		Generator<std::uniform_real_distribution<float>, float>;
	using BoolGenerator =		Generator<std::bernoulli_distribution>;

} //ns Ctoolhu::Random

#endif
