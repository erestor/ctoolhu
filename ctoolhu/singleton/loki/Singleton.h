////////////////////////////////////////////////////////////////////////////////
// The Loki Library
// Copyright (c) 2001 by Andrei Alexandrescu
// This code accompanies the book:
// Alexandrescu, Andrei. "Modern C++ Design: Generic Programming and Design 
//     Patterns Applied". Copyright (c) 2001. Addison-Wesley.
// Permission to use, copy, modify, distribute and sell this software for any 
//     purpose is hereby granted without fee, provided that the above copyright 
//     notice appear in all copies and that both that copyright notice and this 
//     permission notice appear in supporting documentation.
// The author or Addison-Wesley Longman make no representations about the 
//     suitability of this software for any purpose. It is provided "as is" 
//     without express or implied warranty.
//
// Modified by Martin Klemsa
//
////////////////////////////////////////////////////////////////////////////////
#ifndef LOKI_SINGLETON_INC_
#define LOKI_SINGLETON_INC_

// $Id: Singleton.h 834 2007-08-02 19:36:10Z syntheticpp $

#include <atomic>
#include <stdexcept>
#include <cassert>
#include <cstdlib>
#include <mutex>

#ifdef _MSC_VER
#define LOKI_C_CALLING_CONVENTION_QUALIFIER __cdecl 
#else
#define LOKI_C_CALLING_CONVENTION_QUALIFIER 
#endif

namespace Loki
{
    typedef void (LOKI_C_CALLING_CONVENTION_QUALIFIER *atexit_pfn_t)();

    ////////////////////////////////////////////////////////////////////////////////
    ///  \struct CreateUsingNew 
    ///
    ///  \ingroup CreationGroup
    ///  Implementation of the CreationPolicy used by SingletonHolder
    ///  Creates objects using a straight call to the new operator 
    ////////////////////////////////////////////////////////////////////////////////
    template <class T> struct CreateUsingNew
    {
        static T* Create()
        { return new T; }
        
        static void Destroy(T* p)
        { delete p; }
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    ///  \struct DefaultLifetime
    ///
    ///  \ingroup LifetimeGroup
    ///  Implementation of the LifetimePolicy used by SingletonHolder
    ///  Schedules an object's destruction as per C++ rules
    ///  Forwards to std::atexit
    ////////////////////////////////////////////////////////////////////////////////
    template <class T>
    struct DefaultLifetime
    {
        static void ScheduleDestruction(T*, atexit_pfn_t pFun)
        { std::atexit(pFun); }
        
        static void OnDeadReference()
        { throw std::logic_error("Dead Reference Detected"); }
    };

    ////////////////////////////////////////////////////////////////////////////////
    ///  \struct NoDestroy
    ///
    ///  \ingroup LifetimeGroup
    ///  Implementation of the LifetimePolicy used by SingletonHolder
    ///  Never destroys the object
    ////////////////////////////////////////////////////////////////////////////////
    template <class T>
    struct NoDestroy
    {
        static void ScheduleDestruction(T*, atexit_pfn_t)
        {}
        
        static void OnDeadReference()
        {}
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    ///  \class  SingletonHolder
    ///
    ///  \ingroup SingletonGroup
    ///
    ///  Provides Singleton amenities for a type T
    ///  To protect that type from spurious instantiations, 
    ///  you have to protect it yourself.
    ///  
    ///  \param CreationPolicy Creation policy, default: CreateUsingNew
    ///  \param LifetimePolicy Lifetime policy, default: DefaultLifetime,
    ////////////////////////////////////////////////////////////////////////////////
    template
    <
        typename T,
        template <class> class CreationPolicy = CreateUsingNew,
        template <class> class LifetimePolicy = DefaultLifetime
    >
    class SingletonHolder
    {
    public:

        ///  Type of the singleton object
        typedef T ObjectType;

        ///  Returns a reference to singleton object
        static T& Instance();
        
    private:
        // Helpers
        static T* MakeInstance();
        static void LOKI_C_CALLING_CONVENTION_QUALIFIER DestroySingleton();

        // Protection
        SingletonHolder();

        // Data
        // Creation is thread-safe (double-checked locking), the lock is only taken until the instance exists.
        using PtrInstanceType = std::atomic<T*>;
        static PtrInstanceType pInstance_;
        static bool destroyed_; //guarded by the mutex
        static std::mutex& Mutex();
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    // SingletonHolder's data
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        template <class> class C,
        template <class> class L
    >
    typename SingletonHolder<T, C, L>::PtrInstanceType
        SingletonHolder<T, C, L>::pInstance_{nullptr};

    template
    <
        class T,
        template <class> class C,
        template <class> class L
    >
    bool SingletonHolder<T, C, L>::destroyed_ = false;

    ////////////////////////////////////////////////////////////////////////////////
    // SingletonHolder::Instance
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        template <class> class CreationPolicy,
        template <class> class LifetimePolicy
    >
    inline T& SingletonHolder<T, CreationPolicy, LifetimePolicy>::Instance()
    {
        T* p = pInstance_.load(std::memory_order_acquire);
        if (!p)
        {
            p = MakeInstance();
        }
        return *p;
    }

    template
    <
        class T,
        template <class> class C,
        template <class> class L
    >
    std::mutex& SingletonHolder<T, C, L>::Mutex()
    {
        static std::mutex mutex; //function-local static, so it is initialised (thread-safely) before first use
        return mutex;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // SingletonHolder::MakeInstance (helper for Instance)
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        template <class> class CreationPolicy,
        template <class> class LifetimePolicy
    >
    T* SingletonHolder<T, CreationPolicy, LifetimePolicy>::MakeInstance()
    {
        std::lock_guard lock{Mutex()};
        T* p = pInstance_.load(std::memory_order_relaxed);
        if (!p)
        {
            if (destroyed_)
            {
                LifetimePolicy<T>::OnDeadReference(); //may throw, the singleton then stays dead
                destroyed_ = false;
            }
            p = CreationPolicy<T>::Create();
            pInstance_.store(p, std::memory_order_release);
            LifetimePolicy<T>::ScheduleDestruction(p, &DestroySingleton);
        }
        return p;
    }

    template
    <
        class T,
        template <class> class CreationPolicy,
        template <class> class L
    >
    void LOKI_C_CALLING_CONVENTION_QUALIFIER 
    SingletonHolder<T, CreationPolicy, L>::DestroySingleton()
    {
        T* p;
        {
            std::lock_guard lock{Mutex()};
            assert(!destroyed_);
            p = pInstance_.exchange(nullptr, std::memory_order_acq_rel);
            destroyed_ = true;
        }
        //destroy outside the lock: T's destructor may wait for other threads that call Instance()
        //(e.g. the thread pool joining jobs that use Async), which would otherwise deadlock;
        //those calls see a dead reference and are handled by the lifetime policy instead
        CreationPolicy<T>::Destroy(p);
    }

} // namespace Loki

#endif // end file guardian
