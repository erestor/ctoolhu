//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_memory_object_pool_included_
#define _ctoolhu_memory_object_pool_included_

#include "object_pool_deleter.hpp"
#include <boost/pool/object_pool.hpp>
#include <exception>
#include <memory>

namespace Ctoolhu::Memory {

	template <class T>
	class PoolIgnoreMallocErrorsPolicy : protected boost::object_pool<T> {

	  protected:

		auto malloc()
		{
			return boost::object_pool<T>::malloc();
		}
	};

	template <class T>
	class PoolThrowOnMallocErrorsPolicy : protected boost::object_pool<T> {

	  protected:

		auto malloc()
		{
			auto ptr = boost::object_pool<T>::malloc();
			if (ptr == nullptr)
				throw std::bad_alloc();

			return ptr;
		}
	};

	//Extension of boost::object_pool that can used to create unique_ptr of T without worrying about the deleter
	template <
		class T,
		template <class> class MallocErrorsPolicy = PoolIgnoreMallocErrorsPolicy //ignore errors by default
	>
	class ObjectPool : protected MallocErrorsPolicy<T> {

		using deleter_t = ObjectPoolDeleter<T>;

	  public:

		using unique_ptr_t = std::unique_ptr<T, deleter_t &>;

		ObjectPool() : _deleter(this) {}

		template <class... Args>
		unique_ptr_t make_unique(Args &&... args)
		{
			auto ptr = this->malloc(); //errors are handled according to the policy template
			return unique_ptr_t{::new (ptr) T(std::forward<Args>(args)...), _deleter};
		}

	  private:

		deleter_t _deleter;
	};

} //ns

#endif //file guard
