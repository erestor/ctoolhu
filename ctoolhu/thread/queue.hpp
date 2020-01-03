/*
	Copyright (c) 2018, Will Pearce

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Original source: http://roar11.com/2016/01/a-platform-independent-thread-pool-using-c14/

	Modified by Martin Klemsa
*/

#ifndef _ctoolhu_thread_queue_included_
#define _ctoolhu_thread_queue_included_

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

namespace Ctoolhu::Thread {

	/**
		* The Queue class.
		* Provides a wrapper around a basic queue to provide thread safety.
		*/
	template <typename T>
	class Queue {

		using lock_guard_t = std::lock_guard<std::mutex>;

	  public:

		~Queue()
		{
			invalidate();
		}

		/**
		* Attempt to get the first value in the queue.
		* Returns true if a value was successfully written to the out parameter, false otherwise.
		*/
		bool tryPop(T &out)
		{
			lock_guard_t lock{_mutex};
			if (_queue.empty() || !_valid)
				return false;

			out = std::move(_queue.front());
			_queue.pop();
			return true;
		}

		/**
		* Get the first value in the queue.
		* Will block until a value is available unless clear is called or the instance is destructed.
		* Returns true if a value was successfully written to the out parameter, false otherwise.
		*/
		bool waitPop(T &out)
		{
			std::unique_lock<std::mutex> lock{_mutex};
			_changed.wait(lock, [this]() {
				return !_queue.empty() || !_valid;
			});

			/*
				* Using the condition in the predicate ensures that spurious wakeups with a valid
				* but empty queue will not proceed, so only need to check for validity before proceeding.
				*/
			if (!_valid)
				return false;

			out = std::move(_queue.front());
			_queue.pop();
			return true;
		}

		//push a new value onto the queue
		void push(T value)
		{
			{
				lock_guard_t lock{_mutex};
				_queue.push(std::move(value));
			}
			_changed.notify_one();
		}

		//check whether or not the queue is empty
		[[nodiscard]] bool empty() const
		{
			lock_guard_t lock{_mutex};
			return _queue.empty();
		}

		//clear all items from the queue
		void clear()
		{
			{
				lock_guard_t lock{_mutex};
				while (!_queue.empty())
					_queue.pop();
			}
			_changed.notify_all();
		}

		/**
		* Invalidate the queue.
		* Used to ensure no conditions are being waited on in waitPop when
		* a thread or the application is trying to exit.
		* The queue is invalid after calling this method and it is an error
		* to continue using a queue after this method has been called.
		*/
		void invalidate()
		{
			_valid = false;
			_changed.notify_all();
		}

		//returns whether or not this queue is valid
		[[nodiscard]] bool isValid() const noexcept
		{
			return _valid;
		}

	  private:

		std::queue<T> _queue;
		std::atomic_bool _valid{true};

		mutable std::mutex _mutex;
		std::condition_variable _changed;
	};

} //ns Ctoolhu::Thread

#endif //file guard
