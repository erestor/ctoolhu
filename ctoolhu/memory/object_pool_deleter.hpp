//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_memory_object_pool_deleter_included_
#define _ctoolhu_memory_object_pool_deleter_included_

#include <boost/pool/object_pool.hpp>

namespace Ctoolhu::Memory {

	template <class T>
	class ObjectPoolDeleter {

	  public:

		explicit ObjectPoolDeleter(boost::object_pool<T> *pool) noexcept
			: _pool{pool} {}

		void operator()(T *obj)
		{
			_pool->destroy(obj);
		}

	  private:

		boost::object_pool<T> *_pool;
	};

} //ns

#endif //file guard
