//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_memory_object_pool_included_
#define _ctoolhu_memory_object_pool_included_

#include "object_pool_deleter.hpp"
#include <boost/pool/pool.hpp>
#include <cstddef>
#include <memory>
#include <new>
#include <utility>

namespace Ctoolhu::Memory {

	namespace Private {

		//Unordered pool of chunks for objects of type T.
		//Unlike boost::object_pool, allocation and deallocation are constant time
		//(boost::object_pool keeps the free list ordered so that it can destroy the remaining objects in its destructor).
		template <class T>
		class PoolStorage {

			static_assert(alignof(T) <= alignof(std::max_align_t), "over-aligned types are not supported");

		  protected:

			boost::pool<> _pool{sizeof(T)};
		};

	} //ns Private

	//malloc returns nullptr when memory can't be allocated
	template <class T>
	class PoolIgnoreMallocErrorsPolicy : protected Private::PoolStorage<T> {

	  protected:

		void *malloc()
		{
			return this->_pool.malloc();
		}
	};

	//malloc throws std::bad_alloc when memory can't be allocated
	template <class T>
	class PoolThrowOnMallocErrorsPolicy : protected Private::PoolStorage<T> {

	  protected:

		void *malloc()
		{
			auto ptr = this->_pool.malloc();
			if (ptr == nullptr)
				throw std::bad_alloc();

			return ptr;
		}
	};

	//Pool of objects of type T handed out as unique_ptr, so that they're returned to the pool automatically.
	//All the pointers must be destroyed (or reset) before the pool is destroyed: the pool frees its memory
	//without running destructors of objects that are still alive.
	//Don't call unique_ptr::release() on them, the object would never be destroyed and its chunk never returned.
	template <
		class T,
		template <class> class MallocErrorsPolicy = PoolIgnoreMallocErrorsPolicy //ignore errors by default
	>
	class ObjectPool : protected MallocErrorsPolicy<T> {

		using deleter_t = ObjectPoolDeleter<T>;

	  public:

		using unique_ptr_t = std::unique_ptr<T, deleter_t>;

		ObjectPool() = default;
		ObjectPool(const ObjectPool &) = delete;
		ObjectPool &operator=(const ObjectPool &) = delete;

		//returns an empty pointer if the memory can't be allocated and errors are ignored
		template <class... Args>
		unique_ptr_t make_unique(Args &&... args)
		{
			deleter_t deleter{&this->_pool};
			void *mem = this->malloc(); //errors are handled according to the policy template
			if (mem == nullptr)
				return unique_ptr_t{nullptr, deleter};

			try {
				return unique_ptr_t{::new (mem) T(std::forward<Args>(args)...), deleter};
			}
			catch (...) {
				this->_pool.free(mem); //the constructor threw, return the chunk
				throw;
			}
		}
	};

} //ns

#endif //file guard
