#pragma once

#include "detail/common_slist_algorithm.h"

namespace i3
{
	namespace intrusive
	{
		template<class NodeTraits>
		struct circular_slist_algorithm : detail::common_slist_algorithm<NodeTraits>
		{
			typedef detail::common_slist_algorithm<NodeTraits>	base_type;
			
			typedef NodeTraits						node_traits;
			typedef typename node_traits::node		node;

			static void init_head( node* this_node) {  node_traits::set_next(this_node, this_node);  }
			
			static node* get_previous_node( node* prev_init_node, node* this_node)
			{  return base_type::get_previous_node(prev_init_node, this_node);   }

			static node* get_previous_node( node* this_node)
			{  return base_type::get_previous_node(this_node, this_node); }
			
			static node* get_previous_previous_node(node* p, node* this_node)
			{
				node* p_next = node_traits::get_next(p);
				node* p_next_next = node_traits::get_next(p_next);
				while (this_node != p_next_next)
				{
					p = p_next;
					p_next = p_next_next;
					p_next_next = node_traits::get_next(p_next);
				}
				return p;
			}

			static node* get_previous_previous_node( node* this_node)
			{  return get_previous_previous_node(this_node, this_node); }

			static std::size_t count( const node* this_node)
			{
				std::size_t result = 0;
				const node* p = this_node;
				do{
					p = node_traits::get_next(p);
					++result;
				} while (p != this_node);
				return result;
			}

			static void unlink( node* this_node)
			{
				if(node_traits::get_next(this_node))
					base_type::unlink_after(get_previous_node(this_node));
			}
			static void link_before ( node* nxt_node, node* this_node)
			{  base_type::link_after(get_previous_node(nxt_node), this_node);   }

			static void swap_nodes( node* this_node, node* other_node)
			{
				if (other_node == this_node)
					return;
				const node* this_next = node_traits::get_next(this_node);
				const node* other_next = node_traits::get_next(other_node);
				const bool this_null   = !this_next;
				const bool other_null  = !other_next;
				const bool this_empty  = this_next == this_node;
				const bool other_empty = other_next == other_node;

				if(!(other_null || other_empty))
				{
					node_traits::set_next(this_next == other_node ? other_node : get_previous_node(other_node), this_node );
				}
				if(!(this_null || this_empty))
				{
					node_traits::set_next(other_next == this_node ? this_node  : get_previous_node(this_node), other_node );
				}
				node_traits::set_next(this_node,  other_empty ? this_node  : (other_next == this_node ? other_node : other_next) );
				node_traits::set_next(other_node, this_empty  ? other_node : (this_next == other_node ? this_node :  this_next ) );
			}

			static void reverse( node* p)
			{
				node* i = node_traits::get_next(p), e(p);
				for (;;) 
				{
					node* nxt(node_traits::get_next(i));
					if (nxt == e)
						break;
					base_type::transfer_after(e, i, nxt);
				}
			}

		};

	}


}