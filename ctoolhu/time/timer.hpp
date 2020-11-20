//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_time_timer_included_
#define _ctoolhu_time_timer_included_

#include <chrono>

namespace Ctoolhu::Time {

	//tool for measuring elapsed time since instantiation or manual clock start
	template <
		class Resolution //how accurate should the timer be - use std::chrono::(hours|minutes|seconds|milliseconds) etc.
	>
	class Timer {

	  public:

		using duration_t = Resolution;
		using clock_t = std::chrono::steady_clock;

		Timer() noexcept
		{
			StartClock();
		}

		void StartClock() noexcept
		{
			_startTime = clock_t::now();
		}

		duration_t ElapsedTime() const noexcept
		{
			return std::chrono::duration_cast<duration_t>(clock_t::now() - _startTime);
		}

	  private:

		clock_t::time_point _startTime;
	};

} //ns Ctoolhu::Time

#endif //file guard
