#ifndef _ctoolhu_visitor_visitable_included_
#define _ctoolhu_visitor_visitable_included_

namespace Ctoolhu::Visitor {

	//base class of the visitable hierarchy
	template <class Visitor>
	class VisitableRoot {

	  public:

		virtual Visitor::ret_t accept(const Visitor &) = 0;
		virtual Visitor::ret_t accept(const Visitor &) const = 0;
	};

	//derived classes of the visitable hierarchy
	template <
		class Derived,	//concrete visited class that will derive from Visitable
		class Root,		//base class of the visited hierarchy - can be derived from VisitableRoot above
		class Visitor
	>
	class Visitable : public Root {

	  public:

		Visitor::ret_t accept(const Visitor &v) final {
			return v.visit(static_cast<Derived &>(*this));
		}

		Visitor::ret_t accept(const Visitor &v) const final {
			return v.visit(static_cast<const Derived &>(*this));
		}

	  protected:

		using Root::Root;
	};

} //ns

#endif //file guard
