#pragma once

#include "smart_list_node.h"

namespace i3
{
	namespace intrusive
	{
		template<class SmartPtr>
		struct smart_list_node_traits
		{
			typedef list_node					head_node_type;

			typedef	list_node_with_smart_ptr<SmartPtr>  node;
		
			static node* get_prev( const node* n) { return static_cast<node*>(n->prev); }
			static node* get_prev( node* n) { return static_cast<node*>(n->prev); }
			static void  set_prev( node* n, node* prev) { n->prev = prev; }

			static node* get_next( const node* n) { return static_cast<node*>(n->next); }
			static node* get_next( node* n) { return static_cast<node*>(n->next); }
			static void  set_next( node* n, node* next) { n->next = next; }
		};

	}

}
