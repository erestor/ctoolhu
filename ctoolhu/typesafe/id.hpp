//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_typesafe_id_included_
#define _ctoolhu_typesafe_id_included_

#include <compare>
#include <cstddef>
#include <functional>
#include <iosfwd>
#include <type_traits>

namespace Ctoolhu::TypeSafe {

	namespace Private {

		//stores a value (id) and exposes some typical operations usually performed on ids
		template <typename IdType>
		class Storage {
			
		  public:

			//comparisons are deliberately defined in Id, not here: all ids with the same IdType share this base,
			//so comparisons defined here would make ids of different objects comparable

			friend std::ostream &operator<<(std::ostream &out, Storage storage)
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

		[[nodiscard]] constexpr auto value() const noexcept { return this->_id; }

	protected:

		constexpr ImplicitConversion() noexcept = default;
	};

	//policy defining that the conversion to stored id type is explicit so that it cannot be mistakenly juxtaposed with the underlying type
	template <typename IdType>
	class ExplicitConversion : public Private::Storage<IdType> {

	  public:

		[[nodiscard]] constexpr auto value() const noexcept { return this->_id; }

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
		class TagObject,		//type of the object which will have this id
		typename IdType = int,	//type of the id that would normally be used
		template <typename> class ConversionPolicy = ExplicitConversion
	>
	class Id : public ConversionPolicy<IdType> {

	  public:

		using tag_t = TagObject;
		using id_t = IdType;

		constexpr Id() = default;

		explicit constexpr Id(id_t id) noexcept
		{
			this->_id = id;
		}

		//only ids with the same tag can be compared
		friend constexpr bool operator==(const Id &a, const Id &b) noexcept
		{
			return a._id == b._id;
		}

		friend constexpr auto operator<=>(const Id &a, const Id &b) noexcept
		{
			return a._id <=> b._id;
		}

		static_assert(std::is_trivially_copyable_v<IdType>);
	};

	static_assert(sizeof(int) == sizeof(Id<int, int, ImplicitConversion>));
	static_assert(sizeof(int) == sizeof(Id<int, int, ExplicitConversion>));
	static_assert(std::is_trivial_v<Id<int, int, ImplicitConversion>>);
	static_assert(std::is_trivial_v<Id<int, int, ExplicitConversion>>);

	namespace Private {

		struct OtherTag;
		template <class T>
		concept EqualityComparableWithOtherTag = requires(T a, Id<OtherTag> b) { a == b; };
	}
	static_assert(!Private::EqualityComparableWithOtherTag<Id<int>>, "ids of different objects must not be comparable");

} //ns Ctoolhu::TypeSafe

//allows using ids as keys in unordered containers
template <class TagObject, typename IdType, template <typename> class ConversionPolicy>
struct std::hash<Ctoolhu::TypeSafe::Id<TagObject, IdType, ConversionPolicy>> {

	std::size_t operator()(const Ctoolhu::TypeSafe::Id<TagObject, IdType, ConversionPolicy> &id) const noexcept
	{
		return std::hash<IdType>{}(id.value());
	}
};

#endif //file guard
