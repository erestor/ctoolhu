//----------------------------------------------------------------------------
// Author:		Martin Klemsa
//----------------------------------------------------------------------------
#ifndef _ctoolhu_typesafe_id_included_
#define _ctoolhu_typesafe_id_included_

#include <iosfwd>

namespace Ctoolhu {

	namespace TypeSafe {

		//stores a value (id) and exposes some typical operations usually performed on ids
		template <typename IdType>
		class Storage {
			
			using self_type = Storage<IdType>;

		  public:

			bool operator ==(self_type comp) const
			{
				return _id == comp._id;
			}

			bool operator !=(self_type comp) const
			{
				return _id != comp._id;
			}

			bool operator <(self_type comp) const
			{
				return _id < comp._id;
			}

			bool operator >(self_type comp) const
			{
				return _id > comp._id;
			}

			bool operator <=(self_type comp) const
			{
				return _id <= comp._id;
			}

			bool operator >=(self_type comp) const
			{
				return _id >= comp._id;
			}

			friend std::ostream &operator <<(std::ostream &out, self_type storage)
			{
				return out << storage._id;
			}

		  protected:

			Storage() = default;

			IdType _id;
		};

		//policy defining that the conversion to stored id type is implicit
		template <typename IdType>
		class ImplicitConversion : public Storage<IdType> {

		  public:

			operator IdType() const
			{
				return this->_id;
			}

		  protected:

			ImplicitConversion() = default;
		};

		//policy defining that the conversion to stored id type is explicit so that it cannot be mistakenly juxtaposed for the underlying type
		template <typename IdType>
		class ExplicitConversion : public Storage<IdType> {

		  public:

			explicit operator IdType() const
			{
				return this->_id;
			}

		  protected:

			ExplicitConversion() = default;
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

			using object_type = RequestingObject;
			using id_type = IdType;

			explicit Id(IdType id)
#ifdef _DEBUG
				: _val(this->_id)
#endif
			{
				this->_id = id;
			}

#ifdef _DEBUG
			Id(const Id &src)
				: _val(this->_id)
			{
				this->_id = src._id;
			}

			Id &operator =(const Id &src)
			{
				this->_id = src._id;
				return *this;
			}

		  private:

			const IdType &_val; //so that we can see the value readily in watch window of the debugger
#endif
		};

	} //ns TypeSafe

} //ns Ctoolhu

#endif //file guard
