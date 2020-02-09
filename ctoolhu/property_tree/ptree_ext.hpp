#ifndef _ctoolhu_property_tree_ptree_ext_included_
#define _ctoolhu_property_tree_ptree_ext_included_

#include <boost/property_tree/ptree.hpp>
#include <string>
#include <utility>

namespace boost::property_tree {

	//creates a tree with unnamed child tree for each element in the container
	//(which is equivalent to a JSON array)
	template <class Source, class Builder>
	ptree create_array(const Source &container, Builder element_creator)
	{
		ptree array_tree;
		for (auto const &obj : container) {
			ptree obj_tree;
			element_creator(obj, obj_tree);
			if (!obj_tree.empty())
				array_tree.push_back(std::pair{std::string{}, obj_tree});
		}
		return array_tree;
	}

} //ns boost::property_tree

#endif //file guard
