#ifndef _ctoolhu_typesafe_id_included_
#define _ctoolhu_typesafe_id_included_

namespace Ctoolhu {

	namespace TypeSafe {

		//stores a value (id) and exposes some typical operations usually performed on ids
		template <class IdType>
		class Storage {
			
			typedef Storage<IdType> self_type;

		  public:

			bool operator ==(const self_type &comp) const
			{
				return _id == comp._id;
			}

			bool operator !=(const self_type &comp) const
			{
				return _id != comp._id;
			}

			bool operator <(const self_type &comp) const
			{
				return _id < comp._id;
			}

			bool operator >(const self_type &comp) const
			{
				return _id > comp._id;
			}

			bool operator <=(const self_type &comp) const
			{
				return _id <= comp._id;
			}

			bool operator >=(const self_type &comp) const
			{
				return _id >= comp._id;
			}

		  protected:

			Storage() {} //not for stand-alone instantiation

			IdType _id;
		};

		//conversion to stored id type is implicit
		template <typename IdType>
		class ImplicitConversion : public Storage<IdType> {

		  public:

			operator IdType() const
			{
				return _id;
			}

		  protected:

			ImplicitConversion() {} //not for stand-alone instantiation
		};

		//conversion to stored id type is explicit, via the operator ()
		template <typename IdType>
		class ExplicitConversion : public Storage<IdType> {

		  public:

			IdType operator()() const
			{
				return _id;
			}

		  protected:

			ExplicitConversion() {} //not for stand-alone instantiation
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
		//	unsigned int _id;
		//
		template <
			class RequestingObject,				//type of the object which will have this id
			typename IdType = unsigned int,		//type of the id that would normally be used
			template <typename> class ConversionPolicy = ImplicitConversion
		>
		class Id : public ConversionPolicy<IdType> {

			typedef Id<RequestingObject, IdType, ConversionPolicy> self_type;

		  public:

			typedef RequestingObject object_type;
			typedef IdType id_type;

			explicit Id(IdType id)
			{
				_id = id;
			}

			Id &operator =(const self_type &src)
			{
				if (this != &src) {
					_id = src._id;
				}
				return *this;
			}
		};

	}; //ns TypeSafe

}; //ns Ctoolhu

#endif //file guard
