#pragma once

#include "detail/common_slist_algorithm.h"

namespace i3
{
	namespace intrusive
	{
		template<class NodeTraits>
		struct linear_slist_algorithm : detail::common_slist_algorithm<NodeTraits>
		{
			typedef detail::common_slist_algorithm<NodeTraits>	base_type;

			typedef NodeTraits						node_traits;
			typedef typename node_traits::node		node;

			static void init_head( node* this_node) {  node_traits::set_next(this_node, nullptr);  }

			static node* get_previous_node( node* prev_init_node, node* this_node)
			{  return base_type::get_previous_node(prev_init_node, this_node);   }

			static std::size_t count( const node* this_node)
			{
				std::size_t result = 0;
				const node* p = this_node;
				do
				{
					p = node_traits::get_next(p);
					++result;
				} while (p);
				return result;
			}

			static void swap_trailing_nodes(node* this_node, node* other_node)
			{
				node* this_nxt    = node_traits::get_next(this_node);
				node* other_nxt   = node_traits::get_next(other_node);
				node_traits::set_next(this_node, other_nxt);
				node_traits::set_next(other_node, this_nxt);
			}

			static node* reverse(node* p)
			{
				if(!p) return nullptr;

				node* i = node_traits::get_next(p);
				node* first(p);
				while(i)
				{
					node* nxti(node_traits::get_next(i));
					base_type::unlink_after(p);
					node_traits::set_next(i, first);
					first = i;
					i = nxti;
				}
				return first;
			}
			
		};
	}
}