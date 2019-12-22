//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_random_selector_included_
#define _ctoolhu_random_selector_included_

#include "generator.hpp"
#include <cassert>
#include <iterator>

namespace Ctoolhu::Random {

	class Selector {

	  public:

		//returns a reference to a random member in the container
		template <typename Container>
		decltype(auto) operator()(const Container &c)
		{
			assert(!c.empty() && "Can't select a random element from an empty container");
			return *Select(std::cbegin(c), std::cend(c));
		}

		//returns an iterator to a random member in the container
		template <typename Iterator>
		Iterator Select(Iterator begin, Iterator end)
		{
			IntGenerator gen(0, std::distance(begin, end) - 1);
			std::advance(begin, gen());
			return begin;
		}
	};

	template <typename Container>
	decltype(auto) Select(const Container &c)
	{
		Selector s;
		return s(c);
	}

} //ns Ctoolhu::Random

#endif //file guard
