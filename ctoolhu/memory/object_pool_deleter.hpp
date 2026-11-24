//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_memory_object_pool_deleter_included_
#define _ctoolhu_memory_object_pool_deleter_included_

#include <boost/pool/pool.hpp>

namespace Ctoolhu::Memory {

	//destroys an object created in a pool's chunk and returns the chunk to the pool
	template <class T>
	class ObjectPoolDeleter {

	  public:

		ObjectPoolDeleter() noexcept = default;

		explicit ObjectPoolDeleter(boost::pool<> *pool) noexcept
			: _pool{pool} {}

		void operator()(T *obj) const noexcept
		{
			obj->~T();
			_pool->free(obj);
		}

	  private:

		boost::pool<> *_pool{nullptr};
	};

} //ns

#endif //file guard
