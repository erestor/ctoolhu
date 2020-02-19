#ifndef _ctoolhu_visitor_visitor_included_
#define _ctoolhu_visitor_visitor_included_

namespace Ctoolhu::Visitor {

	template <class ReturnType, class... VisitedTypes>
	class Visitor {

	  public:

		using ret_t = ReturnType;

	  protected:

		void visit(); //syntactic sugar to allow the using clause in the recursion
		virtual ~Visitor() = default;
	};

	template <class ReturnType, class Visited, class... VisitedTypes>
	class Visitor<ReturnType, Visited, VisitedTypes...> : public Visitor<ReturnType, VisitedTypes...> {

	  public:

		using Visitor<ReturnType, VisitedTypes...>::visit;
		virtual ReturnType visit(Visited &) const = 0;
	};


	template <class ReturnType, class... VisitedTypes>
	class ConstVisitor {

	  public:

		using ret_t = ReturnType;

	  protected:

		void visit();
		virtual ~ConstVisitor() = default;
	};

	template <class ReturnType, class Visited, class... VisitedTypes>
	class ConstVisitor<ReturnType, Visited, VisitedTypes...> : public ConstVisitor<ReturnType, VisitedTypes...> {

	  public:

		using ConstVisitor<ReturnType, VisitedTypes...>::visit;
		virtual ReturnType visit(const Visited &) const = 0;
	};

} //ns

#endif //file guard
