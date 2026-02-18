#pragma once

#include "smart_generic_hook.h"
#include "../../utility/get_pointer.h"

namespace i3
{
	namespace intrusive
	{
		
		template<class T, class NodeTraits, class Tag>
		struct smart_bhtraits
		{
			typedef NodeTraits						node_traits;
			typedef typename node_traits::node		node;

			typedef node_holder<node, Tag>			node_holder_type;
			typedef T								value_type;

			static T&		to_value_reference(node* n) {	return n->get_smart_ptr(); 	}
			static const T& to_value_reference(const node* n) { return n->get_smart_ptr(); }
			static node*	to_node_ptr(T& v) 
			{
				return static_cast<node*>(static_cast<node_holder_type*>(get_pointer(v)));
			}
			static const node* to_node_ptr(const T& v)
			{
				return static_cast<const node*>( static_cast<const node_holder_type*>(get_pointer(v)));
			}
		};
		
		template<class T, class Hook, class Elem, Hook Elem::* PtoM>
		struct smart_mhtraits
		{
			typedef	Hook								hook_type;
			typedef typename hook_type::node_traits		node_traits;
			typedef typename node_traits::node			node;
			
			typedef T									value_type;

			static T&		to_value_reference(node* n) { return n->get_smart_ptr(); }
			static const T& to_value_reference(const node* n) { return n->get_smart_ptr(); }

			static node*		to_node_ptr(T& v) {	return &static_cast<node&>(get_pointer(v)->*PtoM);  }
			static const node*  to_node_ptr(const T& v) { return &static_cast<const node&>(get_pointer(v)->*PtoM);  }
		};



	}

}