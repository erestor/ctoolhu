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

namespace Ctoolhu {

	namespace Random {

		//shortcut for the generator template we'll be using
		template <class Distribution>
		using RandomGenerator = boost::variate_generator<Private::RandomEngine &, Distribution>;

		//generator with run-time bounds
		template <
			class Distribution,
			typename Boundary = int
		>
		class Generator : public RandomGenerator<Distribution> {
	
			typedef RandomGenerator<Distribution> base_type;

		  public:

			//for number generators
			Generator(Boundary lower, Boundary upper)
				: base_type(Private::SingleRandomEngine::Instance(), Distribution(lower, upper)) {}

			//for bool generator
			Generator()
				: base_type(Private::SingleRandomEngine::Instance(), Distribution()) {}

#ifdef _DEBUG_RAND
			auto operator()()
			{
				auto res = this->base_type::operator()();
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
		typedef Generator<std::uniform_int_distribution<>>	IntGenerator;
		typedef Generator<boost::uniform_smallint<>>		SmallIntGenerator;
		typedef Generator<std::uniform_real_distribution<float>, float>	FloatGenerator;
		typedef Generator<std::bernoulli_distribution>		BoolGenerator;

	} //ns Random

} //ns Ctoolhu

#endif
