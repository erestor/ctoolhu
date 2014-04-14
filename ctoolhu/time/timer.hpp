//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _timer_h_
#define _timer_h_

#include <chrono>

namespace Ctoolhu {

	namespace Time {

		//tool for measuring elapsed time since clock start
		template <
			class Resolution //how accurate should the timer be - use std::chrono::(hours|minutes|seconds|milliseconds) etc.
		>
		class Timer {

		  protected:

			typedef Resolution duration_type;

		  public:

			Timer() = default;
			Timer(const Timer &) = delete;
			Timer &operator =(const Timer &) = delete;

			void StartClock()
			{
				using namespace std::chrono;
				_startTime = steady_clock::now();
			}

			duration_type ElapsedTime() const
			{
				using namespace std::chrono;
				return duration_cast<duration_type>(steady_clock::now() - _startTime);
			}

		  private:

			std::chrono::steady_clock::time_point _startTime;
		};

		//tool for measuring elapsed time since instantiation
		template <
			class Resolution //how accurate should the timer be - use std::chrono::(hours|minutes|seconds|milliseconds) etc.
		>
		class AutoTimer : Timer<Resolution> {

			typedef Timer<Resolution> base_type;

		  public:

			AutoTimer()
			{
				StartClock();
			}

			typename base_type::duration_type ElapsedTime() const
			{
				return base_type::ElapsedTime();
			}
		};

	}; //ns Time

}; //ns Ctoolhu

#endif //file guard
