//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_time_timer_included_
#define _ctoolhu_time_timer_included_

#include <chrono>

namespace Ctoolhu {

	namespace Time {

		//tool for measuring elapsed time since instantiation or manual clock start
		template <
			class Resolution //how accurate should the timer be - use std::chrono::(hours|minutes|seconds|milliseconds) etc.
		>
		class Timer {

		  public:

			typedef Resolution duration_type;
			typedef std::chrono::steady_clock clock_type;

			Timer()
			{
				StartClock();
			}

			void StartClock()
			{
				_startTime = clock_type::now();
			}

			duration_type ElapsedTime() const
			{
				return std::chrono::duration_cast<duration_type>(clock_type::now() - _startTime);
			}

		  private:

			clock_type::time_point _startTime;
		};

	} //ns Time

} //ns Ctoolhu

#endif //file guard
