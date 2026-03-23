//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_std_ext_included_
#define _ctoolhu_std_ext_included_

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <numeric>
#include <ranges>
#include <utility>

//this is a std extension by purpose, so stays outside the Ctoolhu namespace

namespace std_ext {

	template <class Container, typename Sum>
	constexpr Sum accumulate(const Container &c, Sum init)
	{
		return std::accumulate(std::cbegin(c), std::cend(c), init);
	}

	template <class Container, typename Sum, class Reductor>
	constexpr Sum accumulate(const Container &c, Sum init, Reductor &&r)
	{
		return std::accumulate(std::cbegin(c), std::cend(c), init, std::forward<Reductor>(r));
	}

	template <class LookupContainer, class T>
	constexpr bool binary_search(const LookupContainer &c, const T &v)
	{
		assert(std::is_sorted(std::cbegin(c), std::cend(c)) && "Container should be sorted");
		return std::binary_search(std::cbegin(c), std::cend(c), v);
	}

	template <class Container, class Value>
	constexpr auto count_sorted(const Container &c, const Value &v)
	{
		assert(std::is_sorted(std::cbegin(c), std::cend(c)) && "Container should be sorted");
		return std::distance(std::lower_bound(std::cbegin(c), std::cend(c), v), std::upper_bound(std::cbegin(c), std::cend(c), v));
	}

	template <class Container, class Value>
	auto insert_sorted(Container &c, const Value &v)
	{
		assert(std::is_sorted(std::cbegin(c), std::cend(c)) && "Container should be sorted");
		return c.insert(std::upper_bound(c.begin(), c.end(), v), v);
	}

	template <class Container>
	auto unique(Container &c)
	{
		assert(std::is_sorted(std::cbegin(c), std::cend(c)) && "Container should be sorted");
		return std::unique(std::begin(c), std::end(c));
	}

	template <class Container, class Comparator>
	auto unique(Container &c, Comparator &&comp)
	{
		return std::unique(std::begin(c), std::end(c), std::forward<Comparator>(comp));
	}

	//utilities

	template <class LookupContainer, class T, class Compare = std::less<>>
	auto binary_find(const LookupContainer &c, const T &val, Compare comp = {})
	{
		assert(std::is_sorted(std::cbegin(c), std::cend(c), comp) && "Container should be sorted");
		auto last = std::cend(c);
		auto first = std::lower_bound(std::cbegin(c), last, val, comp);
		return first != last && !comp(val, *first) ? first : last;
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

	template <class LookupContainer, class Value>
	constexpr bool contains(const LookupContainer &c, Value &&v)
	{
		return std::ranges::find(c, std::forward<Value>(v)) != std::cend(c);
	}

	template <class HayContainer, class NeedleContainer>
	constexpr bool contains_any(const HayContainer &haystack, const NeedleContainer &needles)
	{
		return std::ranges::any_of(needles, [&haystack](auto const &n) {
			return contains(haystack, n);
		});
	}

	template <class Container, class Value>
	auto erase(Container &c, const Value &v)
	{
		auto it = std::find(std::begin(c), std::end(c), v);
		if (it == std::end(c))
			return it;

		return c.erase(it);
	}

	//for Boost containers
	template <class Container, class Predicate>
	constexpr auto erase_if(Container &c, Predicate &&p)
	{
		return c.erase(std::remove_if(std::begin(c), std::end(c), std::forward<Predicate>(p)), std::end(c));
	}

	//removes duplicate values from given sorted container
	template <class Container>
	void erase_duplicates(Container &c)
	{
		c.erase(unique(c), std::end(c));
	}

	//removes duplicate values from given sorted container
	template <class Container, class Comparator>
	void erase_duplicates(Container &c, Comparator &&comp)
	{
		c.erase(unique(c, std::forward<Comparator>(comp)), std::end(c));
	}

	template <class Container>
	bool next_k_permutation(Container &c, int choose)
	{
		auto const first = std::begin(c);
		auto const last = std::end(c);
		assert(choose >= 0 && choose <= std::distance(first, last) && "choose must be within the container bounds");

		auto const middle = std::next(first, choose);
		std::reverse(middle, last);
		return std::next_permutation(first, last);
	}

	template <class Container>
	bool next_combination(Container &c, int choose)
	{
		auto const first = std::begin(c);
		auto const last = std::end(c);
		assert(choose >= 0 && choose <= std::distance(first, last) && "choose must be within the container bounds");

		auto const middle = std::next(first, choose);
		bool result;
		do {
			result = next_k_permutation(c, choose);
		} while (std::adjacent_find(first, middle, std::greater{}) != middle);
		return result;
	}
}

#endif //file guard
