//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_typesafe_id_included_
#define _ctoolhu_typesafe_id_included_

#include <iosfwd>
#include <type_traits>

namespace Ctoolhu::TypeSafe {

	namespace Private {

		//stores a value (id) and exposes some typical operations usually performed on ids
		template <typename IdType>
		class Storage {
			
		  public:

			constexpr bool operator ==(Storage comp) const noexcept
			{
				return _id == comp._id;
			}

			constexpr bool operator !=(Storage comp) const noexcept
			{
				return _id != comp._id;
			}

			constexpr bool operator <(Storage comp) const noexcept
			{
				return _id < comp._id;
			}

			constexpr bool operator >(Storage comp) const noexcept
			{
				return _id > comp._id;
			}

			constexpr bool operator <=(Storage comp) const noexcept
			{
				return _id <= comp._id;
			}

			constexpr bool operator >=(Storage comp) const noexcept
			{
				return _id >= comp._id;
			}

			friend std::ostream &operator <<(std::ostream &out, Storage storage)
			{
				return out << storage._id;
			}

		  protected:

			constexpr Storage() noexcept = default;

			IdType _id;
		};

	} //ns Private

	//policy defining that the conversion to stored id type is implicit
	template <typename IdType>
	class ImplicitConversion : public Private::Storage<IdType> {

	  public:

		constexpr operator IdType() const noexcept
		{
			return this->_id;
		}

	  protected:

		constexpr ImplicitConversion() noexcept = default;
	};

	//policy defining that the conversion to stored id type is explicit so that it cannot be mistakenly juxtaposed with the underlying type
	template <typename IdType>
	class ExplicitConversion : public Private::Storage<IdType> {

	  public:

		constexpr explicit operator IdType() const noexcept
		{
			return this->_id;
		}

	  protected:

		constexpr ExplicitConversion() noexcept = default;
	};

	//Tool for preventing mix-up of ids of different objects by means of
	//distinguishing their id types and disallowing conversion between them.
	//The conversions to the underlying type are based on the chosen policy.
	//
	//e.g. for lesson ID use (inside class Lesson)
	//
	//	Id<Lesson> _id;
	//
	//instead of
	//
	//	int _id;
	//
	template <
		class RequestingObject,		//type of the object which will have this id
		typename IdType = int,		//type of the id that would normally be used
		template <typename> class ConversionPolicy = ImplicitConversion
	>
	class Id : public ConversionPolicy<IdType> {

	  public:

		using object_t = RequestingObject;
		using id_t = IdType;

		constexpr Id() = default;

		explicit constexpr Id(id_t id) noexcept
		{
			this->_id = id;
		}
	};

	static_assert(sizeof(int) == sizeof(Id<int, int, ImplicitConversion>));
	static_assert(std::is_trivial_v<Id<int, int, ImplicitConversion>>);
	static_assert(std::is_trivial_v<Id<int, int, ExplicitConversion>>);

} //ns Ctoolhu::TypeSafe

#endif //file guard
