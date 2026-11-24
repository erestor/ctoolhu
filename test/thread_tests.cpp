#include <ctoolhu/thread/async.hpp>
#include <ctoolhu/thread/lockable.hpp>
#include <ctoolhu/thread/proxy.hpp>
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

using namespace Ctoolhu::Thread;

TEST(AsyncTest, MoveOnlyArgument)
{
	auto f = Async([](std::unique_ptr<int> p) { return *p * 2; }, std::make_unique<int>(21));
	EXPECT_EQ(f.get(), 42);
}

TEST(AsyncTest, ReferenceArgumentViaStdRef)
{
	int target = 0;
	Async([](int &t) { t = 5; }, std::ref(target)).get();
	EXPECT_EQ(target, 5);
}

TEST(AsyncTest, ExceptionIsPropagated)
{
	auto f = Async([] { throw std::runtime_error("e"); });
	EXPECT_THROW(f.get(), std::runtime_error);
}

TEST(PoolTest, Destruction_RunsAllQueuedJobs)
{
	std::atomic<int> done{0};
	{
		Pool pool{2};
		for (int i = 0; i < 100; ++i)
			(void)pool.submit([&done] {
				std::this_thread::sleep_for(std::chrono::microseconds(50));
				++done;
			});
	}
	EXPECT_EQ(done, 100);
}

TEST(PoolTest, SinglePool_ConcurrentInstance_CreatesOneInstance)
{
	std::vector<void *> seen(16);
	std::vector<std::thread> threads;
	for (int i = 0; i < 16; ++i)
		threads.emplace_back([&seen, i] { seen[i] = &SinglePool::Instance(); });
	for (auto &t : threads)
		t.join();
	for (auto p : seen)
		EXPECT_EQ(p, seen[0]);
}

TEST(FutureTest, MoveAssignment_TakesNewResult)
{
	Future<int> f{Async([] { return 1; })};
	f = Future<int>{Async([] { return 2; })};
	EXPECT_EQ(f.get(), 2);
}

TEST(FutureTest, MoveAssignment_WaitsForPreviousJob)
{
	std::atomic<bool> firstFinished{false};
	Future<void> f{Async([&firstFinished] {
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		firstFinished = true;
	})};
	f = Future<void>{Async([] {})};
	EXPECT_TRUE(firstFinished);
	f.get();
}

TEST(FutureTest, ReferenceResult_IsNotCopied)
{
	int x = 3;
	std::promise<int &> promise;
	Future<int &> f{promise.get_future()};
	promise.set_value(x);
	EXPECT_EQ(&f.get(), &x);
}

namespace {

	struct Guarded : Lockable<std::mutex> {
		int value = 0;
	};

} //ns

TEST(LockableTest, WorksWithStdLockGuard)
{
	Guarded g;
	{
		std::lock_guard lock{g};
		g.value = 1;
	}
	EXPECT_EQ(g.value, 1);
}

TEST(LockingProxyTest, AccessesTheObject)
{
	Guarded g;
	LockingProxy<Guarded> p{&g};
	p->value = 2;
	EXPECT_EQ(g.value, 2);
}

TEST(LockingProxyTest, MoveConstruction_TransfersTheLock)
{
	Guarded g;
	{
		LockingProxy<Guarded> p1{&g};
		LockingProxy<Guarded> p2{std::move(p1)};
		p2->value = 3;
	} //only p2 unlocks; a double unlock would assert in debug
	std::lock_guard lock{g}; //fails if the mutex were left locked
	EXPECT_EQ(g.value, 3);
}
