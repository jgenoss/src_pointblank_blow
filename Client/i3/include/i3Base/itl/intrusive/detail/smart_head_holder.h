#pragma once

namespace i3
{
	namespace intrusive
	{
		
		template<class NodeTraits>
		struct smart_head_holder : NodeTraits::head_node_type
		{
			typedef NodeTraits								node_traits;
			typedef typename node_traits::head_node_type	head_node_type;
			typedef typename node_traits::node				node;

			smart_head_holder() : head_node_type() {}

			const node*	get_node() const {  return static_cast<const node*>( static_cast<const head_node_type*>(this)); }
			node*		get_node() { return static_cast<node*>( static_cast<head_node_type*>(this)); }

		};

		template<class ValueTraits>
		struct get_smart_head_holder
		{
			typedef smart_head_holder< typename ValueTraits::node_traits>		type;
		};
		



	}

}