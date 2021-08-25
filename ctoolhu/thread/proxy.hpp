//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_thread_proxy_included_
#define _ctoolhu_thread_proxy_included_

#include <utility>

namespace Ctoolhu::Thread {

	//Provides object-level locking for any lockable class.
	//If used in conjunction with inheriting from Lockable the client class internals needn't be touched
	//(indeed this is the same implementation as appears in Modern C++ design by Alexandrescu)
	template <class T>
	class LockingProxy {

	  public:

		explicit LockingProxy(T *client) : _client(client)
		{
			_client->Lock();
		}

		LockingProxy(LockingProxy &&src)
		{
			_client = std::exchange(src._client, nullptr);
		}

		LockingProxy &operator=(LockingProxy &&src)
		{
			_client = std::exchange(src._client, nullptr);
			return *this;
		}

		LockingProxy(const LockingProxy &) = delete;
		LockingProxy &operator=(const LockingProxy &) = delete;

		~LockingProxy()
		{
			if (_client)
				_client->Unlock();
		}

		T *operator->() const noexcept
		{
			return _client;
		}

	  private:

		T *_client;
	};

} //ns Ctoolhu::Thread

#endif //file guard
