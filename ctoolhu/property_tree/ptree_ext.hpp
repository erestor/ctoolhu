#ifndef _ctoolhu_property_tree_ptree_ext_included_
#define _ctoolhu_property_tree_ptree_ext_included_

#include <boost/property_tree/ptree.hpp>

namespace Ctoolhu::PropertyTree {

	//creates a tree with unnamed child tree for each element in the container
	//(which is equivalent to a JSON array)
	template <class Source, class Builder>
	boost::property_tree::ptree create_array(const Source &container, Builder element_creator)
	{
		using boost::property_tree::ptree;

		ptree array_tree;
		for (auto const &obj : container) {
			//build the element in place, push_back would copy the whole subtree
			auto &obj_tree = array_tree.push_back(ptree::value_type{})->second;
			element_creator(obj, obj_tree);
			if (obj_tree.empty())
				array_tree.pop_back();
		}
		return array_tree;
	}

} //ns Ctoolhu::PropertyTree

#endif //file guard
