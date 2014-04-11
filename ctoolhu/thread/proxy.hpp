#ifndef _ctoolhu_thread_proxy_included_
#define _ctoolhu_thread_proxy_included_

namespace Ctoolhu {

	namespace Thread {

		//Provides object-level locking for any lockable class.
		//If used in conjunction with inheriting from Lockable the client class internals needn't be touched
		//(indeed this is the same implementation as appears in Modern C++ design by Alexandrescu)
		template <class T>
		class LockingProxy {

		  public:

			LockingProxy(T *client) : _client(client)
			{
				_client->Lock();
			}

			~LockingProxy()
			{
				_client->Unlock();
			}

			T *operator ->() const
			{
				return _client;
			}

			LockingProxy(const LockingProxy &) = delete;
			LockingProxy &operator =(const LockingProxy &) = delete;

		  private:

			T *_client;
		};

	}; //ns Thread

}; //ns Ctoolhu

#endif //file guard
