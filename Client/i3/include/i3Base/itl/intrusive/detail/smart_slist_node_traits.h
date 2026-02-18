#pragma once

#include "smart_slist_node.h"

namespace i3
{
	namespace intrusive
	{
		template<class SmartPtr>
		struct smart_slist_node_traits
		{
			typedef slist_node								head_node_type;
			typedef slist_node_with_smart_ptr<SmartPtr>		node;

			static node* get_next( const node* n) { return static_cast<node*>(n->next); }
			static node* get_next( node* n) { return static_cast<node*>(n->next); }
			static void  set_next( node* n, node* next) { n->next = next; }
		};

	}
}