//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_std_ext_included_
#define _ctoolhu_std_ext_included_

#include <algorithm>

//this is a std extension by purpose, so will stay outside the Ctoolhu namespace

namespace std_ext {

    template <class Container> typename Container::const_iterator find(const Container &c, const typename Container::value_type &v);

	template <class Container, class Predicate>
	bool all_of(const Container &c, const Predicate &p)
	{
		return std::all_of(std::cbegin(c), std::cend(c), p);
	}

	template <class Container, class Predicate>
	bool any_of(const Container &c, const Predicate &p)
	{
		return std::any_of(std::cbegin(c), std::cend(c), p);
	}

	template <class Destination, class Source>
	auto concat_move(Destination &d, Source &s)
		-> decltype(std::move(std::begin(s), std::end(s), std::back_inserter(d)))
	{
		return std::move(std::begin(s), std::end(s), std::back_inserter(d));
	}

	template <class Container, class Predicate>
	auto count_if(Container &c, const Predicate &p)
		-> decltype(std::count_if(std::begin(c), std::end(c), p))
	{
		return std::count_if(std::begin(c), std::end(c), p);
	}

	template <class Container>
	typename Container::iterator erase(Container &c, const typename Container::value_type &v)
	{
		return c.erase(find(c, v));
	}

	template <class Container, class Predicate>
	typename Container::iterator erase_if(Container &c, const Predicate &p)
	{
		return c.erase(std::remove_if(std::begin(c), std::end(c), p), std::end(c));
	}

	template <class Container>
	typename Container::const_iterator find(const Container &c, const typename Container::value_type &v)
	{
		return std::find(std::cbegin(c), std::cend(c), v);
	}

	template <class Container, class Predicate>
	typename Container::iterator find_if(Container &c, const Predicate &p)
	{
		return std::find_if(std::begin(c), std::end(c), p);
	}

	template <class Container, class Predicate>
	typename Container::const_iterator find_if(const Container &c, const Predicate &p)
	{
		return std::find_if(std::cbegin(c), std::cend(c), p);
	}

	template <class LookupContainer>
	bool contains(const LookupContainer &c, const typename LookupContainer::value_type &v)
	{
		return find(c, v) != std::end(c);
	}

	template <class Container, class Predicate>
	typename Container::iterator remove_if(Container &c, const Predicate &p)
	{
		return std::remove_if(std::begin(c), std::end(c), p);
	}

	template <class Container>
	void sort(Container &c)
	{
		std::sort(std::begin(c), std::end(c));
	}

	template <class Container, class Comparator>
	void sort(Container &c, Comparator comp)
	{
		std::sort(std::begin(c), std::end(c), comp);
	}

	template <class Container>
	void unique(Container &c)
	{
		c.erase(std::unique(std::begin(c), std::end(c)), std::end(c));
	}

	template <class Container, class Comparator>
	void unique(Container &c, Comparator comp)
	{
		c.erase(std::unique(std::begin(c), std::end(c), comp), std::end(c));
	}

	template <class Container, class Sorter, class Predicate>
	void unique(Container &c, Sorter s, Predicate p)
	{
		sort(c, s);
		unique(c, p);
	}
}

#endif //file guard
