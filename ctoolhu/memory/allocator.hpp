#ifndef _ctoolhu_memory_allocator_included_
#define _ctoolhu_memory_allocator_included_

#include "../singleton/holder.hpp"
#include <cassert>

namespace Ctoolhu::Memory {
/*
	template <class T>
	class Allocator { 

	  public:

		using value_type = T;

		Allocator() noexcept {}  // not required, unless used
		template <class U> Allocator(Allocator<U> const&) noexcept {}

		value_type *allocate(std::size_t Size)
		{
			return static_cast<value_type*>(::operator new (Size * sizeof(value_type)));
		}

		void deallocate(value_type* p, std::size_t) noexcept  // Use pointer if pointer is not a value_type*
		{
			::operator delete(p);
		}
	};

	template <class T, class U>
	bool operator==(Allocator<T> const&, Allocator<U> const&) noexcept
	{
		return true;
	}

	template <class T, class U>
	bool operator!=(Allocator<T> const& x, Allocator<U> const& y) noexcept
	{
		return !(x == y);
	}
*/

	/// Returns a multiple of A needed to store `Size` bytes.
	inline uintptr_t alignTo(uintptr_t Size, size_t alignment)
	{
		const uintptr_t Value = alignment;
		// The following line is equivalent to `(Size + Value - 1) / Value * Value`.

		// The division followed by a multiplication can be thought of as a right
		// shift followed by a left shift which zeros out the extra bits produced in
		// the bump; `~(Value - 1)` is a mask where all those bits being zeroed out
		// are just zero.

		// Most compilers can generate this code but the pattern may be missed when
		// multiple functions gets inlined.
		return (Size + Value - 1) & ~(Value - 1U);
	}

	inline uintptr_t alignAddr(const void *Addr, size_t Alignment)
	{
		uintptr_t ArithAddr = reinterpret_cast<uintptr_t>(Addr);
		assert(static_cast<uintptr_t>(ArithAddr + Alignment - 1) >= ArithAddr && "Overflow");
		return alignTo(ArithAddr, Alignment);
	}

	/// Returns the offset to the next integer (mod 2**64) that is greater than
	/// or equal to \p Value and is a multiple of \p Align.
	inline uintptr_t offsetToAlignment(uintptr_t Value, size_t Alignment)
	{
		return alignTo(Value, Alignment) - Value;
	}

	/// Returns the necessary adjustment for aligning `Addr` to `Alignment`
	/// bytes, rounding up.
	inline uintptr_t offsetToAlignedAddr(const void *Addr, size_t Alignment)
	{
		return offsetToAlignment(reinterpret_cast<uintptr_t>(Addr), Alignment);
	}

	/// CRTP base class providing obvious overloads for the core \c
	/// Allocate() methods of LLVM-style allocators.
	///
	/// This base class both documents the full public interface exposed by all
	/// LLVM-style allocators, and redirects all of the overloads to a single core
	/// set of methods which the derived class must define.
	template <typename DerivedT> class AllocatorBase {
	
	  public:

		/// Allocate \a Size bytes of \a Alignment aligned memory. This method
		/// must be implemented by \c DerivedT.
		void *allocate(size_t Size, size_t Alignment)
		{
			return static_cast<DerivedT *>(this)->allocate(Size, Alignment);
		}

		/// Deallocate \a Ptr to \a Size bytes of memory allocated by this
		/// allocator.
		void deallocate(const void *Ptr, size_t Size)
		{
			return static_cast<DerivedT *>(this)->deallocate(Ptr, Size);
		}

		// The rest of these methods are helpers that redirect to one of the above
		// core methods.

		/// Allocate space for a sequence of objects without constructing them.
		template <typename T> T *allocate(size_t Num = 1)
		{
			return static_cast<T *>(allocate(Num * sizeof(T), alignof(T)));
		}

		/// Deallocate space for a sequence of objects without constructing them.
		template <typename T>
		std::enable_if_t<!std::is_same<std::remove_cv_t<T>, void>::value, void>
			deallocate(T *Ptr, size_t Num = 1)
		{
			deallocate(static_cast<const void *>(Ptr), Num * sizeof(T));
		}
	};

	class MallocAllocator : public AllocatorBase<MallocAllocator> {
	public:
		void Reset() {}

		void *allocate(size_t Size, size_t /*Alignment*/)
		{
			return std::malloc(Size);
		}

		// Pull in base class overloads.
		using AllocatorBase<MallocAllocator>::allocate;

		void deallocate(const void *Ptr, size_t /*Size*/)
		{
			std::free(const_cast<void *>(Ptr));
		}

		// Pull in base class overloads.
		using AllocatorBase<MallocAllocator>::deallocate;
	};

	template <size_t SlabSize = 4096, size_t SizeThreshold = SlabSize, size_t GrowthDelay = 128>
	class BumpPtrAllocatorImpl : public AllocatorBase<BumpPtrAllocatorImpl<SlabSize, SizeThreshold, GrowthDelay>> {

	  public:

		BumpPtrAllocatorImpl() = default;

		template <typename T>
		BumpPtrAllocatorImpl(T &&allocator)
			: _allocator(std::forward<T &&>(allocator))
		{
		}

		// Manually implement a move constructor as we must clear the old allocator's
		// slabs as a matter of correctness.
		BumpPtrAllocatorImpl(BumpPtrAllocatorImpl &&Old)
			: CurPtr(Old.CurPtr),
			End(Old.End),
			Slabs(std::move(Old.Slabs)),
			CustomSizedSlabs(std::move(Old.CustomSizedSlabs)),
			BytesAllocated(Old.BytesAllocated),
			_allocator(std::move(Old._allocator))
		{
			Old.CurPtr = Old.End = nullptr;
			Old.BytesAllocated = 0;
			Old.Slabs.clear();
			Old.CustomSizedSlabs.clear();
		}

		~BumpPtrAllocatorImpl()
		{
			DeallocateSlabs(Slabs.begin(), Slabs.end());
			DeallocateCustomSizedSlabs();
		}

		BumpPtrAllocatorImpl &operator=(BumpPtrAllocatorImpl &&RHS)
		{
			DeallocateSlabs(Slabs.begin(), Slabs.end());
			DeallocateCustomSizedSlabs();

			CurPtr = RHS.CurPtr;
			End = RHS.End;
			BytesAllocated = RHS.BytesAllocated;
			Slabs = std::move(RHS.Slabs);
			CustomSizedSlabs = std::move(RHS.CustomSizedSlabs);
			Allocator = std::move(RHS.Allocator);

			RHS.CurPtr = RHS.End = nullptr;
			RHS.BytesAllocated = 0;
			RHS.Slabs.clear();
			RHS.CustomSizedSlabs.clear();
			return *this;
		}

		/// Deallocate all but the current slab and reset the current pointer
		/// to the beginning of it, freeing all memory allocated so far.
		void Reset()
		{
			// Deallocate all but the first slab, and deallocate all custom-sized slabs.
			DeallocateCustomSizedSlabs();
			CustomSizedSlabs.clear();

			if (Slabs.empty())
				return;

			// Reset the state.
			BytesAllocated = 0;
			CurPtr = (char *)Slabs.front();
			End = CurPtr + SlabSize;

			DeallocateSlabs(std::next(Slabs.begin()), Slabs.end());
			Slabs.erase(std::next(Slabs.begin()), Slabs.end());
		}

		/// Allocate space at the specified alignment.
		void *allocate(size_t Size, size_t Alignment)
		{
			// Keep track of how many bytes we've allocated.
			BytesAllocated += Size;

		    size_t Adjustment = offsetToAlignedAddr(CurPtr, Alignment);
			assert(Adjustment + Size >= Size && "Adjustment + Size must not overflow");

			size_t SizeToAllocate = Size;

			// Check if we have enough space.
			if (Adjustment + SizeToAllocate <= size_t(End - CurPtr)) {
				char *AlignedPtr = CurPtr + Adjustment;
				CurPtr = AlignedPtr + SizeToAllocate;
				return AlignedPtr;
			}

			// If Size is really big, allocate a separate slab for it.
			size_t PaddedSize = SizeToAllocate + Alignment - 1;
			if (PaddedSize > SizeThreshold) {
				void *NewSlab = _allocator.allocate(PaddedSize, 0);
				CustomSizedSlabs.push_back(std::make_pair(NewSlab, PaddedSize));

				uintptr_t AlignedAddr = alignAddr(NewSlab, Alignment);
				assert(AlignedAddr + Size <= (uintptr_t)NewSlab + PaddedSize);
				char *AlignedPtr = (char*)AlignedAddr;
				return AlignedPtr;
			}

			// Otherwise, start a new slab and try again.
			StartNewSlab();
			uintptr_t AlignedAddr = alignAddr(CurPtr, Alignment);
			char *AlignedPtr = (char*)AlignedAddr;
			CurPtr = AlignedPtr + SizeToAllocate;
			return AlignedPtr;
		}

		// Pull in base class overloads.
		using AllocatorBase<BumpPtrAllocatorImpl>::allocate;

		// Bump pointer allocators are expected to never free their storage; and
		// clients expect pointers to remain valid for non-dereferencing uses even
		// after deallocation.
		void deallocate(const void * /*Ptr*/, size_t /*Size*/)
		{
			//do nothing
		}

		// Pull in base class overloads.
		using AllocatorBase<BumpPtrAllocatorImpl>::deallocate;

		size_t GetNumSlabs() const { return Slabs.size() + CustomSizedSlabs.size(); }

		size_t getTotalMemory() const
		{
			size_t TotalMemory = 0;
			for (auto I = Slabs.begin(), E = Slabs.end(); I != E; ++I)
				TotalMemory += computeSlabSize(std::distance(Slabs.begin(), I));
			for (auto &PtrAndSize : CustomSizedSlabs)
				TotalMemory += PtrAndSize.second;
			return TotalMemory;
		}

		size_t getBytesAllocated() const { return BytesAllocated; }

	  private:

		/// The current pointer into the current slab.
		///
		/// This points to the next free byte in the slab.
		char *CurPtr = nullptr;

		/// The end of the current slab.
		char *End = nullptr;

		/// The slabs allocated so far.
		std::vector<void *> Slabs;

		/// Custom-sized slabs allocated for too-large allocation requests.
		std::vector<std::pair<void *, size_t>> CustomSizedSlabs;

		/// How many bytes we've allocated.
		///
		/// Used so that we can compute how much space was wasted.
		size_t BytesAllocated = 0;

		/// The allocator instance we use to get slabs of memory.
		MallocAllocator _allocator;

		static size_t computeSlabSize(unsigned SlabIdx)
		{
			// Scale the actual allocated slab size based on the number of slabs
			// allocated. Every GrowthDelay slabs allocated, we double
			// the allocated size to reduce allocation frequency, but saturate at
			// multiplying the slab size by 2^30.
			return SlabSize *
				((size_t)1 << std::min<size_t>(30, SlabIdx / GrowthDelay));
		}

		/// Allocate a new slab and move the bump pointers over into the new
		/// slab, modifying CurPtr and End.
		void StartNewSlab()
		{
			size_t AllocatedSlabSize = computeSlabSize(Slabs.size());
			void *NewSlab = _allocator.allocate(AllocatedSlabSize, 0);
			Slabs.push_back(NewSlab);
			CurPtr = (char *)(NewSlab);
			End = ((char *)NewSlab) + AllocatedSlabSize;
		}

		/// Deallocate a sequence of slabs.
		void DeallocateSlabs(std::vector<void *>::iterator I,
			std::vector<void *>::iterator E)
		{
			for (; I != E; ++I) {
				size_t AllocatedSlabSize = computeSlabSize(std::distance(Slabs.begin(), I));
				_allocator.deallocate(*I, AllocatedSlabSize);
			}
		}

		/// Deallocate all memory for custom sized slabs.
		void DeallocateCustomSizedSlabs()
		{
			for (auto &PtrAndSize : CustomSizedSlabs) {
				void *Ptr = PtrAndSize.first;
				size_t Size = PtrAndSize.second;
				_allocator.deallocate(Ptr, Size);
			}
		}

		template <typename T> friend class SpecificBumpPtrAllocator;
		template <typename T> friend class SpecificAllocator;
	};

	/// The standard BumpPtrAllocator which just uses the default template
	/// parameters.
	typedef BumpPtrAllocatorImpl<> BumpPtrAllocator;

	/// A BumpPtrAllocator that allows only elements of a specific type to be
	/// allocated.
	///
	/// This allows calling the destructor in DestroyAll() and when the allocator is
	/// destroyed.
	template <typename T> class SpecificBumpPtrAllocator
	{
		BumpPtrAllocator _allocator;

	  public:

		using value_type = T;

		SpecificBumpPtrAllocator() = default;
		
		SpecificBumpPtrAllocator(SpecificBumpPtrAllocator &&Old)
			: _allocator(std::move(Old._allocator))
		{
		}

		~SpecificBumpPtrAllocator()
		{
			DestroyAll();
		}

		SpecificBumpPtrAllocator &operator=(SpecificBumpPtrAllocator &&RHS)
		{
			_allocator = std::move(RHS._allocator);
			return *this;
		}

		/// Call the destructor of each allocated object and deallocate all but the
		/// current slab and reset the current pointer to the beginning of it, freeing
		/// all memory allocated so far.
		void DestroyAll()
		{
			auto DestroyElements = [](char *Begin, char *End) {
				for (char *Ptr = Begin; Ptr + sizeof(T) <= End; Ptr += sizeof(T))
					reinterpret_cast<T *>(Ptr)->~T();
			};

			for (auto I = _allocator.Slabs.begin(), E = _allocator.Slabs.end(); I != E; ++I) {
				size_t AllocatedSlabSize = BumpPtrAllocator::computeSlabSize(std::distance(_allocator.Slabs.begin(), I));
				char *Begin = (char *)alignAddr(*I, alignof(T));
				char *End = *I == _allocator.Slabs.back() ? _allocator.CurPtr : (char *)*I + AllocatedSlabSize;
				DestroyElements(Begin, End);
			}

			for (auto &PtrAndSize : _allocator.CustomSizedSlabs) {
				void *Ptr = PtrAndSize.first;
				size_t Size = PtrAndSize.second;
				DestroyElements((char *)alignAddr(Ptr, alignof(T)), (char *)Ptr + Size);
			}

			_allocator.Reset();
		}

		/// Allocate space for an array of objects without constructing them.
		T *allocate(size_t num = 1)
		{
			return _allocator.allocate<T>(num);
		}

		void deallocate(const void *Ptr, size_t Size)
		{
			_allocator.deallocate(Ptr, Size);
		}
	};

	template <class T>
	using SingleSpecificBumpPtrAllocator = Ctoolhu::Singleton::Holder<SpecificBumpPtrAllocator<T>>;

	template <class T>
	class ConformingAllocator {

	  public:

		using value_type = T;

		ConformingAllocator() = default;
		ConformingAllocator(const ConformingAllocator &) = default;
		ConformingAllocator(ConformingAllocator &&) = default;
		//ConformingAllocator &operator=(const ConformingAllocator &) = default;
		//ConformingAllocator &operator=(ConformingAllocator &&) = default;

		template <class U> ConformingAllocator(const ConformingAllocator<U> &) noexcept {}

		value_type *allocate(std::size_t num)
		{
			return SingleSpecificBumpPtrAllocator<T>::Instance().allocate(num);
		}

		void deallocate(const void *Ptr, size_t Size) noexcept
		{
			SingleSpecificBumpPtrAllocator<T>::Instance().deallocate(Ptr, Size);
		}
	};

	template <class T, class U>
	bool operator==(ConformingAllocator<T> const&, ConformingAllocator<U> const&) noexcept
	{
		return true;
	}

	template <class T, class U>
	bool operator!=(ConformingAllocator<T> const& x, ConformingAllocator<U> const& y) noexcept
	{
		return !(x == y);
	}

	template <class T>
	class StatefulAllocator {

	  public:

		using value_type = T;

		explicit StatefulAllocator(BumpPtrAllocator &a)
			: _allocator{&a} {}

		StatefulAllocator(const StatefulAllocator &) = default;
		StatefulAllocator(StatefulAllocator &&) = default;

		template <class U>
		StatefulAllocator(const StatefulAllocator<U> &src)
			: _allocator{src.getAllocator()} {}

		T *allocate(std::size_t num)
		{
			return _allocator->allocate<T>(num);
		}

		void deallocate(const void *Ptr, size_t Size) noexcept
		{
			_allocator->deallocate(Ptr, Size);
		}

		BumpPtrAllocator *getAllocator() const noexcept
		{
			return _allocator;
		}

	  private:

		BumpPtrAllocator *_allocator;
	};

	template <class T, class U>
	bool operator==(const StatefulAllocator<T> &a, const StatefulAllocator<U> &b) noexcept
	{
		return a.getAllocator() == b.getAllocator();
	}

	template <class T, class U>
	bool operator!=(const StatefulAllocator<T> &a, const StatefulAllocator<U> &b) noexcept
	{
		return !(a == b);
	}

} //ns Ctoolhu::Memory

template <size_t SlabSize, size_t SizeThreshold, size_t GrowthDelay>
void *operator new(size_t Size, Ctoolhu::Memory::BumpPtrAllocatorImpl<SlabSize, SizeThreshold, GrowthDelay> &allocator)
{
	struct S {
		char c;
		union {
			double D;
			long double LD;
			long long L;
			void *P;
		} x;
	};
	return allocator.allocate(
		Size, std::min((size_t)llvm::NextPowerOf2(Size), offsetof(S, x)));
}

template <size_t SlabSize, size_t SizeThreshold, size_t GrowthDelay>
	void operator delete(void *, Ctoolhu::Memory::BumpPtrAllocatorImpl<SlabSize, SizeThreshold, GrowthDelay> &)
{
}

#endif //file guard
