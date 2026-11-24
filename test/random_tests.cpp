#include <ctoolhu/random/selector.hpp>
#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>

using namespace Ctoolhu::Random;

TEST(RandomEngineTest, SameSeed_GivesSameNumbers)
{
	Private::SingleRandomEngine::Instance().seed(17);
	IntGenerator g1(0, 1000000);
	int const first = g1();

	Private::SingleRandomEngine::Instance().seed(17);
	IntGenerator g2(0, 1000000);
	EXPECT_EQ(g2(), first);
}

TEST(RandomEngineTest, SharedGeneratorFromManyThreads_StaysInRange)
{
	FloatGenerator shared{0, 1};
	std::atomic<int> outOfRange{0};
	std::vector<std::thread> threads;
	for (int i = 0; i < 8; ++i)
		threads.emplace_back([&] {
			for (int j = 0; j < 100000; ++j) {
				float const f = shared();
				if (f < 0 || f >= 1)
					++outOfRange;
			}
		});
	for (auto &t : threads)
		t.join();
	EXPECT_EQ(outOfRange, 0);
}

TEST(SelectorTest, Select_ReturnsAnElementOfTheContainer)
{
	std::vector<int> const c{4, 5, 6};
	for (int i = 0; i < 100; ++i) {
		int const selected = Select(c);
		EXPECT_GE(selected, 4);
		EXPECT_LE(selected, 6);
	}
}
