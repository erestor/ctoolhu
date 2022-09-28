//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_singleton_holder_included_
#define _ctoolhu_singleton_holder_included_

#include "loki/Singleton.h"

namespace Ctoolhu::Singleton {

	//This is an alias of Loki SingletonHolder for simplifying Emscripten builds.

#ifdef __EMSCRIPTEN__
	//Singletons cannot be destroyed if building WebAssembly with NO_EXIT_RUNTIME, because atexit() is never called.
	//NO_EXIT_RUNTIME is the default, so it seems rational to use a consistent lifetime policy here.
	template <typename T>
	using Holder = Loki::SingletonHolder<T, Loki::CreateUsingNew, Loki::NoDestroy>;
#else
	template <typename T>
	using Holder = Loki::SingletonHolder<T>;
#endif

} //ns Ctoolhu::Singleton

#endif
