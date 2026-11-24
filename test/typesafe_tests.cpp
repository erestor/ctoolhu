#include <ctoolhu/typesafe/id.hpp>
#include <gtest/gtest.h>
#include <concepts>
#include <unordered_set>

namespace {

	struct A {};
	struct B {};

	using IdA = Ctoolhu::TypeSafe::Id<A>;
	using IdB = Ctoolhu::TypeSafe::Id<B>;

	static_assert(!std::equality_comparable_with<IdA, IdB>, "ids of different objects must not be comparable");
	static_assert(std::totally_ordered<IdA>);
	static_assert(!std::is_convertible_v<IdA, int>, "explicit conversion policy must not convert implicitly");
	static_assert(std::is_convertible_v<Ctoolhu::TypeSafe::Id<A, int, Ctoolhu::TypeSafe::ImplicitConversion>, int>);

	constexpr IdA constantId{5};
	static_assert(constantId == IdA{5}, "comparisons must be usable in constant expressions");

} //ns

TEST(IdTest, Comparisons)
{
	EXPECT_TRUE(IdA{3} == IdA{3});
	EXPECT_TRUE(IdA{3} != IdA{4});
	EXPECT_TRUE(IdA{1} < IdA{2});
	EXPECT_TRUE(IdA{2} >= IdA{2});
}

TEST(IdTest, Value_ReturnsStoredId)
{
	EXPECT_EQ(IdA{42}.value(), 42);
}

TEST(IdTest, Hash_AllowsUnorderedContainers)
{
	std::unordered_set<IdA> const ids{IdA{1}, IdA{1}, IdA{2}};
	EXPECT_EQ(ids.size(), 2u);
	EXPECT_TRUE(ids.contains(IdA{2}));
}
