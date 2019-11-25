//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_maths_comparer_included_
#define _ctoolhu_maths_comparer_included_

#include <cstdlib>
#include <limits>

namespace Ctoolhu::Maths {

	//Provides simple comparison for floating point types
	template <
		class Decimal = float,	//float or double
		int Precision = 5		//numbers differing beyond this number of decimal digits are considered equal
	>
	class Comparer {

		public:

		template <class Numeric>
		static bool equal(Decimal a, Numeric b)
		{
			static float epsilon = std::pow(10.0f, -_precision);
			return std::abs(a - static_cast<Decimal>(b)) < epsilon;
		}

		template <class Numeric>
		static bool less(Decimal a, Numeric b)
		{
			return !equal(a, b) && a < b;
		}

		template <class Numeric>
		static bool lessOrEqual(Decimal a, Numeric b)
		{
			return equal(a, b) || a < b;
		}

		template <class Numeric>
		static bool greater(Decimal a, Numeric b)
		{
			return !equal(a, b) && a > b;
		}

		template <class Numeric>
		static bool greaterOrEqual(Decimal a, Numeric b)
		{
			return equal(a, b) || a > b;
		}

		private:

		static const int _precision{Precision};
		//it would be great to have static epsilon here, but constexpr power doesn't exist
	};

} //ns Ctoolhu::Maths

#endif //file guard
