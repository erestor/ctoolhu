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

#include "Threads.h"
#include <stdexcept>
#include <cassert>
#include <cstdlib>

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
    ///  \param ThreadingModel Threading policy, 
    ///                         default: LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL
    ////////////////////////////////////////////////////////////////////////////////
    template
    <
        typename T,
        template <class> class CreationPolicy = CreateUsingNew,
        template <class> class LifetimePolicy = DefaultLifetime,
        template <class, class> class ThreadingModel = LOKI_DEFAULT_THREADING_NO_OBJ_LEVEL,
        class MutexPolicy = LOKI_DEFAULT_MUTEX
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
        static void MakeInstance();
        static void LOKI_C_CALLING_CONVENTION_QUALIFIER DestroySingleton();
        
        // Protection
        SingletonHolder();
        
        // Data
        typedef typename ThreadingModel<T*,MutexPolicy>::VolatileType PtrInstanceType;
        static PtrInstanceType pInstance_;
        static bool destroyed_;
    };
    
    ////////////////////////////////////////////////////////////////////////////////
    // SingletonHolder's data
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        template <class> class C,
        template <class> class L,
        template <class, class> class M,
        class X
    >
    typename SingletonHolder<T, C, L, M, X>::PtrInstanceType
        SingletonHolder<T, C, L, M, X>::pInstance_ = 0;

    template
    <
        class T,
        template <class> class C,
        template <class> class L,
        template <class, class> class M,
        class X
    >
    bool SingletonHolder<T, C, L, M, X>::destroyed_ = false;

    ////////////////////////////////////////////////////////////////////////////////
    // SingletonHolder::Instance
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        template <class> class CreationPolicy,
        template <class> class LifetimePolicy,
        template <class, class> class ThreadingModel,
        class MutexPolicy
    >
    inline T& SingletonHolder<T, CreationPolicy, 
        LifetimePolicy, ThreadingModel, MutexPolicy>::Instance()
    {
        if (!pInstance_)
        {
            MakeInstance();
        }
        return *pInstance_;
    }

    ////////////////////////////////////////////////////////////////////////////////
    // SingletonHolder::MakeInstance (helper for Instance)
    ////////////////////////////////////////////////////////////////////////////////

    template
    <
        class T,
        template <class> class CreationPolicy,
        template <class> class LifetimePolicy,
        template <class, class> class ThreadingModel,
        class MutexPolicy
    >
    void SingletonHolder<T, CreationPolicy, 
        LifetimePolicy, ThreadingModel, MutexPolicy>::MakeInstance()
    {
        typename ThreadingModel<SingletonHolder,MutexPolicy>::Lock guard;
        (void)guard;
        
        if (!pInstance_)
        {
            if (destroyed_)
            {
                destroyed_ = false;
                LifetimePolicy<T>::OnDeadReference();
            }
            pInstance_ = CreationPolicy<T>::Create();
            LifetimePolicy<T>::ScheduleDestruction(pInstance_, 
                &DestroySingleton);
        }
    }

    template
    <
        class T,
        template <class> class CreationPolicy,
        template <class> class L,
        template <class, class> class M,
        class X
    >
    void LOKI_C_CALLING_CONVENTION_QUALIFIER 
    SingletonHolder<T, CreationPolicy, L, M, X>::DestroySingleton()
    {
        assert(!destroyed_);
        CreationPolicy<T>::Destroy(pInstance_);
        pInstance_ = 0;
        destroyed_ = true;
    }

} // namespace Loki

#endif // end file guardian

