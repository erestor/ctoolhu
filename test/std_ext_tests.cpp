#include <ctoolhu/std_ext.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <functional>
#include <memory>
#include <numeric>
#include <vector>

namespace {

	//the previous implementation of next_combination, used as the reference
	bool referenceNextCombination(std::vector<int> &c, int choose)
	{
		auto const first = std::begin(c), last = std::end(c);
		auto const middle = std::next(first, choose);
		bool result;
		do {
			std::reverse(middle, last);
			result = std::next_permutation(first, last);
		} while (std::adjacent_find(first, middle, std::greater{}) != middle);
		return result;
	}

	void expectSameSequenceAsReference(const std::vector<int> &base)
	{
		for (int k = 0; k <= static_cast<int>(base.size()); ++k) {
			auto actual = base;
			auto expected = base;
			for (;;) {
				ASSERT_TRUE(std::equal(actual.begin(), actual.begin() + k, expected.begin())) << "k=" << k;
				ASSERT_TRUE(std::is_sorted(actual.begin(), actual.begin() + k)) << "k=" << k;
				ASSERT_TRUE(std::is_sorted(actual.begin() + k, actual.end())) << "k=" << k;
				bool const actualResult = std_ext::next_combination(actual, k);
				bool const expectedResult = referenceNextCombination(expected, k);
				ASSERT_EQ(actualResult, expectedResult) << "k=" << k;
				if (!actualResult)
					break;
			}
			EXPECT_EQ(actual, base) << "k=" << k << ": must return to the initial state";
		}
	}

} //ns

TEST(NextCombinationTest, DistinctValues_MatchesReference)
{
	expectSameSequenceAsReference({0, 1, 2, 3, 4, 5, 6});
}

TEST(NextCombinationTest, DuplicateValues_MatchesReference)
{
	expectSameSequenceAsReference({1, 1, 2, 2, 2, 3, 5});
}

TEST(NextCombinationTest, EmptyAndSingle_MatchesReference)
{
	expectSameSequenceAsReference({});
	expectSameSequenceAsReference({4});
}

TEST(NextCombinationTest, TwentyChooseSix_ProducesAllCombinations)
{
	std::vector<int> v(20);
	std::iota(v.begin(), v.end(), 0);
	long count = 1;
	while (std_ext::next_combination(v, 6))
		++count;
	EXPECT_EQ(count, 38760);
}

TEST(ConcatTest, Concat_AppendsAndReturnsBackInserter)
{
	std::vector<int> d{1};
	std::vector<int> const s{2, 3};
	auto out = std_ext::concat(d, s);
	*out++ = 4; //the result keeps appending
	EXPECT_EQ(d, (std::vector<int>{1, 2, 3, 4}));
}

TEST(ConcatTest, Concat_ToItself_DuplicatesTheElements)
{
	std::vector<int> v{1, 2, 3};
	v.shrink_to_fit(); //force a reallocation during the insert
	std_ext::concat(v, v);
	EXPECT_EQ(v, (std::vector<int>{1, 2, 3, 1, 2, 3}));
}

TEST(ConcatTest, ConcatMove_ToItself_DoesNothing)
{
	std::vector<int> v{1, 2};
	std_ext::concat_move(v, v);
	EXPECT_EQ(v, (std::vector<int>{1, 2}));
}

TEST(ConcatTest, ConcatMove_MovesMoveOnlyElements)
{
	std::vector<std::unique_ptr<int>> d, s;
	s.push_back(std::make_unique<int>(7));
	std_ext::concat_move(d, s);
	ASSERT_EQ(d.size(), 1u);
	EXPECT_EQ(*d[0], 7);
}

TEST(CountSortedTest, CountsEqualElements)
{
	std::vector<int> const v{1, 2, 2, 2, 3};
	EXPECT_EQ(std_ext::count_sorted(v, 2), 3);
	EXPECT_EQ(std_ext::count_sorted(v, 4), 0);
}
