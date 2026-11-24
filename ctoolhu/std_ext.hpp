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
		auto const [first, last] = std::equal_range(std::cbegin(c), std::cend(c), v);
		return std::distance(first, last);
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

	//appends the source elements to the destination, returns an iterator to the first appended element
	//(range insert keeps the geometric growth of the destination, unlike reserving the exact size on each call)
	template <class Destination, class Source>
	auto concat(Destination &d, const Source &s)
	{
		return d.insert(std::end(d), std::cbegin(s), std::cend(s));
	}

	template <class Destination, class Source>
	auto concat_move(Destination &d, Source &s)
	{
		return d.insert(std::end(d), std::make_move_iterator(std::begin(s)), std::make_move_iterator(std::end(s)));
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

	//Rearranges the container so that its first 'choose' elements form the next combination in lexicographical order.
	//Start with a sorted container. Both the chosen part and the rest are kept sorted.
	//Returns false when there are no more combinations; the container is then sorted again (i.e. back at the first combination).
	//Each step is linear, unlike filtering the k-permutations, which costs k! times more.
	template <class Container>
	bool next_combination(Container &c, int choose)
	{
		auto const first = std::begin(c);
		auto const last = std::end(c);
		assert(choose >= 0 && choose <= std::distance(first, last) && "choose must be within the container bounds");

		auto const middle = std::next(first, choose);
		if (first == middle || middle == last)
			return false; //only one combination exists

		//find the rightmost chosen element that can be increased, i.e. that is less than the largest unchosen element
		auto const largestRest = std::prev(last);
		auto pivot = middle;
		while (pivot != first) {
			--pivot;
			if (*pivot < *largestRest) {
				//swap it with the smallest unchosen element greater than it (the unchosen part stays sorted)
				auto const successor = std::upper_bound(middle, last, *pivot);
				std::iter_swap(pivot, successor);

				//the chosen elements after the pivot are all >= every unchosen one, so the sequence
				//[after successor, last) + [after pivot, middle) is sorted and holds the smallest candidates to follow the pivot;
				//move it right after the pivot, which leaves [middle, successor] at the end
				auto const afterSuccessor = std::next(successor);
				auto const largerCount = std::distance(afterSuccessor, last);
				std::rotate(std::next(pivot), afterSuccessor, last);

				//the unchosen part is now two sorted runs, the smaller elements last: swap the runs to sort it
				std::rotate(middle, std::next(middle, largerCount), last);
				return true;
			}
		}

		//no chosen element can be increased, this was the last combination: restore the initial (sorted) order
		std::rotate(first, middle, last);
		return false;
	}
}

#endif //file guard
