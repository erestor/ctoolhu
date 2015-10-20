//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_thread_lockable_included_
#define _ctoolhu_thread_lockable_included_

namespace Ctoolhu {

	namespace Thread {

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

		  protected:

			Lockable() = default;

		  private:

			Mutex _mutex;
  		};

	} //ns Thread

} //ns Ctoolhu

#endif //file guard
