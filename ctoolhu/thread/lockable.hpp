//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_thread_lockable_included_
#define _ctoolhu_thread_lockable_included_

namespace Ctoolhu::Thread {

	//inheriting from this class makes any class usable for the locking proxy
	template <class Mutex>
	class Lockable {

	  public:

		void Lock()
		{
			_mutex.lock();
		}

		void Unlock()
		{
			_mutex.unlock();
		}

		//BasicLockable interface, so that std::lock_guard, std::unique_lock and std::scoped_lock can be used too
		void lock() { Lock(); }
		void unlock() { Unlock(); }

	  protected:

		Lockable() = default;

	  private:

		Mutex _mutex;
	};

} //ns Ctoolhu::Thread

#endif //file guard
