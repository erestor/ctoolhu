#ifndef _ctoolhu_std_ext_included_
#define _ctoolhu_std_ext_included_

#include <algorithm>

namespace std_ext {

	template <class Container, class Predicate>
	bool all_of(const Container &c, const Predicate &p)
	{
		return std::all_of(cbegin(c), cend(c), p);
	}

	template <class Container, class Predicate>
	bool any_of(const Container &c, const Predicate &p)
	{
		return std::any_of(cbegin(c), cend(c), p);
	}

	template <class Container, class Predicate>
	void erase_if(Container &c, const Predicate &p)
	{
		c.erase(remove_if(c, p), end(c));
	}

	template <class Container, class Value>
	auto find(Container &c, const Value &v)
		-> decltype(std::find(begin(c), end(c), v))
	{
		return std::find(begin(c), end(c), v);
	}

	template <class Container, class Predicate>
	auto find_if(Container &c, const Predicate &p)
		-> decltype(std::find_if(begin(c), end(c), p))
	{
		return std::find_if(begin(c), end(c), p);
	}

	template <class Container, class Predicate>
	auto remove_if(Container &c, const Predicate &p)
		-> decltype(std::remove_if(begin(c), end(c), p))
	{
		return std::remove_if(begin(c), end(c), p);
	}

	template <class Container>
	void sort(Container &c)
	{
		std::sort(begin(c), end(c));
	}

	template <class Container, class Comparator>
	void sort(Container &c, const Comparator &comp)
	{
		std::sort(begin(c), end(c), comp);
	}
}

#endif //file guard
