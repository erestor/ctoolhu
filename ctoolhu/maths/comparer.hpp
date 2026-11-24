//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_maths_comparer_included_
#define _ctoolhu_maths_comparer_included_

#include <cmath>

namespace Ctoolhu::Maths {

	namespace Private {

		//10 to the power of -exponent, computed at compile time
		constexpr double negativePowerOf10(int exponent) noexcept
		{
			double result{1.0};
			for (int i = 0; i < exponent; ++i)
				result *= 10.0;

			return 1.0 / result;
		}

	} //ns Private

	//Provides simple comparison for floating point types
	template <
		class Decimal = float,	//float or double
		int Precision = 5		//numbers differing beyond this number of decimal digits are considered equal
	>
	class Comparer {

	  public:

		template <class Numeric>
		static bool equal(Decimal a, Numeric b) noexcept
		{
			return std::abs(a - static_cast<Decimal>(b)) < _epsilon;
		}

		template <class Numeric>
		static bool less(Decimal a, Numeric b) noexcept
		{
			return !equal(a, b) && a < b;
		}

		template <class Numeric>
		static bool lessOrEqual(Decimal a, Numeric b) noexcept
		{
			return equal(a, b) || a < b;
		}

		template <class Numeric>
		static bool greater(Decimal a, Numeric b) noexcept
		{
			return !equal(a, b) && a > b;
		}

		template <class Numeric>
		static bool greaterOrEqual(Decimal a, Numeric b) noexcept
		{
			return equal(a, b) || a > b;
		}

	  private:

		static_assert(Precision >= 0, "Precision must not be negative");
		static constexpr double _epsilon{Private::negativePowerOf10(Precision)};
	};

} //ns Ctoolhu::Maths

#endif //file guard
