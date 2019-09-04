//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_thread_async_included_
#define _ctoolhu_thread_async_included_

#include "pool.hpp"

namespace Ctoolhu {

	namespace Thread {

		//Schedules a job for asynchronous processing by the default thread pool.
		//Returns a future for obtaining the result.
		//
		//Note:
		//  This is especially useful as a replacement for std::async for Emscripten WebAssembly builds,
		//  where std::async should be avoided, because the implementation is naive and doesn't use a thread pool.
		//  Moreover the WebAssembly pthread creation/destruction is flawed and threads/workers can be
		//  left dangling if the thread lifetime is extremely short (Emscripten 1.38.41).
		template <typename Func, typename... Args>
		auto Async(Func &&job, Args &&... args)
		{
			return SinglePool::Instance().submit(job, args...);
		}

	} //ns Thread

} //ns Ctoolhu

#endif //file guard
