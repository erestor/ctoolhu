//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_std_ext_included_
#define _ctoolhu_std_ext_included_

#include <algorithm>
#include <numeric>
#include <type_traits>

//this is a std extension by purpose, so will stay outside the Ctoolhu namespace

namespace std_ext {

    template <class Container> typename Container::const_iterator find(const Container &c, const typename Container::value_type &v);

	template <class Container, typename Sum>
	Sum accumulate(const Container &c, Sum init)
	{
		return std::accumulate(std::cbegin(c), std::cend(c), init);
	}

	template <class Container, typename Reductor, typename Sum>
	Sum accumulate(const Container &c, Sum init, const Reductor &r)
	{
		return std::accumulate(std::cbegin(c), std::cend(c), init, r);
	}

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

	template <class LookupContainer, class T, class Compare = std::less<>>
	auto binary_find(const LookupContainer &c, const T &val, Compare comp = {})
	{
		auto last = std::cend(c);
		auto first = std::lower_bound(std::cbegin(c), last, val, comp);
		return first != last && !comp(val, *first) ? first : last;
	}

	template <class LookupContainer, class T>
	bool binary_search(const LookupContainer &c, const T &v)
	{
		return std::binary_search(std::cbegin(c), std::cend(c), v);
	}

	template <class Destination, class Source>
	auto concat(Destination &d, const Source &s)
	{
		d.reserve(d.size() + s.size());
		return std::copy(std::cbegin(s), std::cend(s), std::back_inserter(d));
	}

	template <class Destination, class Source>
	auto concat_move(Destination &d, Source &s)
	{
		d.reserve(d.size() + s.size());
		return std::move(std::begin(s), std::end(s), std::back_inserter(d));
	}

	template <class LookupContainer>
	bool contains(const LookupContainer &c, const typename LookupContainer::value_type &v)
	{
		return find(c, v) != std::cend(c);
	}

	template <class LookupContainer>
	bool contains_any(const LookupContainer &c, const LookupContainer &n)
	{
		return any_of(n, [&c](auto const &v) {
			return contains(c, v);
		});
	}

	template <class SourceContainer, class OutputIterator>
	auto copy(const SourceContainer &src, OutputIterator &&dst)
	{
		return std::copy(std::cbegin(src), std::cend(src), std::forward<OutputIterator>(dst));
	}

	template <class SourceContainer, class OutputIterator, class Predicate>
	auto copy_if(const SourceContainer &src, OutputIterator &&dst, const Predicate &p)
	{
		return std::copy_if(std::cbegin(src), std::cend(src), std::forward<OutputIterator>(dst), p);
	}

	template <class Container, class Predicate>
	auto count_if(Container &c, const Predicate &p)
	{
		return std::count_if(std::cbegin(c), std::cend(c), p);
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

	//const version of the above
	template <class Container, class Predicate>
	typename Container::const_iterator find_if(const Container &c, const Predicate &p)
	{
		return std::find_if(std::cbegin(c), std::cend(c), p);
	}

	template <class Container, class Predicate>
	typename Container::const_iterator max_element(const Container &c, const Predicate &p)
	{
		return std::max_element(std::cbegin(c), std::cend(c), p);
	}

	template <class Container>
	typename Container::const_iterator min_element(const Container &c)
	{
		return std::min_element(std::cbegin(c), std::cend(c));
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
	auto unique(Container &c)
	{
		return std::unique(std::begin(c), std::end(c));
	}

	template <class Container, class Comparator>
	auto unique(Container &c, Comparator comp)
	{
		return std::unique(std::begin(c), std::end(c), comp);
	}

	//removes duplicate values from given sorted container
	template <class Container>
	void erase_duplicates(Container &c)
	{
		c.erase(unique(c), std::end(c));
	}

	//removes duplicate values from given sorted container
	template <class Container, class Comparator>
	void erase_duplicates(Container &c, Comparator comp)
	{
		c.erase(unique(c, comp), std::end(c));
	}
}

#endif //file guard
