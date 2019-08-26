/*
	Copyright (c) 2018, Will Pearce

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Original source: http://roar11.com/2016/01/a-platform-independent-thread-pool-using-c14/

	Modified by Martin Klemsa
*/

#ifndef _ctoolhu_thread_pool_included_
#define _ctoolhu_thread_pool_included_

#include "future.hpp"
#include "queue.hpp"
#include "../singleton/holder.hpp"
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace Ctoolhu {

	namespace Thread {

		namespace Private {

			class IThreadTask {

			  public:

				IThreadTask() = default;
				virtual ~IThreadTask() = default;

				IThreadTask(const IThreadTask &) = delete;
				IThreadTask &operator=(const IThreadTask &) = delete;

				IThreadTask(IThreadTask &&) = default;
				IThreadTask &operator=(IThreadTask &&) = default;

				/**
				 * Run the task.
				 */
				virtual void execute() = 0;
			};

			template <typename Func>
			class ThreadTask : public IThreadTask {

			  public:

				ThreadTask(Func &&func)
					: _func{std::move(func)}
				{
				}

				~ThreadTask() override = default;

				ThreadTask(const ThreadTask &) = delete;
				ThreadTask &operator=(const ThreadTask &) = delete;

				ThreadTask(ThreadTask &&) = default;
				ThreadTask &operator=(ThreadTask &&) = default;

				/**
				 * Run the task.
				 */
				void execute() override
				{
					_func();
				}

			  private:

				Func _func;
			};

		} //ns Private

		/**
		 * The Pool class.
		 * Keeps a set of threads constantly waiting to execute incoming jobs.
		 */
		class Pool {

		  public:

			explicit Pool(unsigned int numThreads)
			:	_done{false},
				_workQueue{},
				_threads{}
			{
				try {
					for (unsigned int i{0u}; i < numThreads; ++i)
						_threads.emplace_back(&Pool::worker, this);
				}
				catch(...) {
					destroy();
					throw;
				}
			}

			Pool() : Pool{std::max(std::thread::hardware_concurrency(), 1u)} //always create at least one thread (hardware_concurrency can return 0)
			{
			}

			~Pool()
			{
				destroy();
			}

			Pool(const Pool &) = delete;
			Pool &operator=(const Pool &) = delete;

			/**
			 * Submit a job to be run by the thread pool.
			 */
			template <typename Func, typename... Args>
			auto submit(Func &&func, Args &&... args)
			{
				auto boundTask = std::bind(std::forward<Func>(func), std::forward<Args>(args)...);
				using ResultType = std::result_of_t<decltype(boundTask)()>;
				using PackagedTask = std::packaged_task<ResultType()>;
				using TaskType = Private::ThreadTask<PackagedTask>;

				PackagedTask task{std::move(boundTask)};
				Future<ResultType> result{task.get_future()};
				_workQueue.push(std::make_unique<TaskType>(std::move(task)));
				return result;
			}

		  private:

			/**
			 * Constantly running function each thread uses to acquire work items from the queue.
			 */
			void worker()
			{
				while (!_done) {
					std::unique_ptr<Private::IThreadTask> task;
					if (_workQueue.waitPop(task))
						task->execute();
				}
			}

			/**
			 * Invalidates the queue and joins all running threads.
			 */
			void destroy()
			{
				_done = true;
				_workQueue.invalidate();
				for (auto &thread : _threads) {
					if (thread.joinable())
						thread.join();
				}
			}

			Queue<std::unique_ptr<Private::IThreadTask>> _workQueue;
			std::vector<std::thread> _threads;
			std::atomic_bool _done;
		};

		using SinglePool = Singleton::Holder<Pool>;

	} //ns Thread

} //ns Ctoolhu

#endif //file guard
