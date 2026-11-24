#include <ctoolhu/memory/object_pool.hpp>
#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

using namespace Ctoolhu::Memory;

namespace {

	struct Throwing {
		explicit Throwing(bool shouldThrow)
		{
			if (shouldThrow)
				throw std::runtime_error("constructor failed");
		}
	};

	struct Counted {
		static inline int alive = 0;
		Counted() { ++alive; }
		~Counted() { --alive; }
		Counted(const Counted &) = delete;
		Counted &operator=(const Counted &) = delete;
	};

} //ns

TEST(ObjectPoolTest, ThrowingConstructor_PropagatesAndPoolStaysUsable)
{
	ObjectPool<Throwing> pool;
	EXPECT_THROW(pool.make_unique(true), std::runtime_error);
	auto p = pool.make_unique(false);
	EXPECT_NE(p, nullptr);
}

TEST(ObjectPoolTest, DestroysObjectsWhenPointersGoAway)
{
	ObjectPool<Counted, PoolThrowOnMallocErrorsPolicy> pool;
	{
		std::vector<decltype(pool)::unique_ptr_t> objects;
		for (int i = 0; i < 1000; ++i)
			objects.push_back(pool.make_unique());
		EXPECT_EQ(Counted::alive, 1000);
	}
	EXPECT_EQ(Counted::alive, 0);
}
