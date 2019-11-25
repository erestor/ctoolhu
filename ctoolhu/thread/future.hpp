/*
	Copyright (c) 2018, Will Pearce

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
	1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Original source: http://roar11.com/2016/01/a-platform-independent-thread-pool-using-c14/

	Modified by Martin Klemsa
*/

#ifndef _ctoolhu_thread_future_included_
#define _ctoolhu_thread_future_included_

#include <future>

namespace Ctoolhu::Thread {

	//A wrapper around a std::future that adds the behavior of futures returned from std::async, to be used with the thread pool.
	//Specifically, this object will block and wait for execution to finish before going out of scope.
	template <typename T>
	class Future {

		public:

		Future(std::future<T> &&future)
			: _future{std::move(future)}
		{
		}

		//prevent copying
		Future(const Future &) = delete;
		Future &operator=(const Future &) = delete;

		//allow moving
		Future(Future &&) = default;
		Future &operator=(Future &&) = default;

		~Future()
		{
			if (_future.valid())
				_future.get();
		}

		auto get()
		{
			return _future.get();
		}

		private:

		std::future<T> _future;
	};

} //ns Ctoolhu::Thread

#endif //file guard
