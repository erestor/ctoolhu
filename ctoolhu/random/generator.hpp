//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_random_generator_included_
#define _ctoolhu_random_generator_included_

#include "engine.hpp"
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/uniform_smallint.hpp>
#include <boost/random/bernoulli_distribution.hpp>

namespace Ctoolhu {

	namespace Random {

		//generator with run-time bounds
		template <
			class Distribution,
			typename Boundary = int
		>
		class Generator :
			public boost::variate_generator<Private::MersenneEngine &, Distribution> {
	
		  public:

			//for number generators
			Generator(Boundary lower, Boundary upper) :
				boost::variate_generator<Private::MersenneEngine &, Distribution>(
					Private::SingleMersenneEngine::Instance(), Distribution(lower, upper)) {}

			//for bool generator
			Generator() :
				boost::variate_generator<Private::MersenneEngine &, Distribution>(
					Private::SingleMersenneEngine::Instance(), Distribution()) {}
		};

		//generator with compile-time bounds
		template <
			int LowerBound,
			int UpperBound,
			class Distribution = boost::uniform_int<>
		>
		class StaticGenerator :
			public boost::variate_generator<Private::MersenneEngine &, Distribution> {
	
		  public:

			StaticGenerator() :
				boost::variate_generator<Private::MersenneEngine &, Distribution>(
					Private::SingleMersenneEngine::Instance(), Distribution(LowerBound, UpperBound)) {}
		};

		//expose typical usages of the dynamic generator
		typedef Generator<boost::uniform_int<> >				IntGenerator;
		typedef Generator<boost::uniform_smallint<> >			SmallIntGenerator;
		typedef Generator<boost::uniform_real<float>, float>	FloatGenerator;
		typedef Generator<boost::bernoulli_distribution<> >		BoolGenerator;

	}; //ns Random

}; //ns Ctoolhu

#endif
