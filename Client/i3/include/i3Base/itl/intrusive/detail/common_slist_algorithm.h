#pragma once

namespace i3
{
	namespace intrusive
	{
		
		namespace detail
		{
			template<class NodeTraits>
			struct common_slist_algorithm
			{
				typedef NodeTraits                           node_traits;
				typedef typename node_traits::node           node;

				
				static node* get_previous_node(node* p, node* this_node)
				{
					for( node* p_next ; this_node != (p_next = node_traits::get_next(p)); p = p_next)
					{
						assert(p);
					}
					return p;
				}

				static void init(node* this_node) {  node_traits::set_next(this_node, nullptr);  }
				static bool is_inited(const node* this_node) {  return !node_traits::get_next(this_node); }

				static bool unique(const node* this_node)
				{
					node* next = node_traits::get_next(this_node);
					return !next || next == this_node;
				}
			
				static void unlink_after( node* prev_node)
				{
					const node* this_node(node_traits::get_next(prev_node));
					node_traits::set_next(prev_node, node_traits::get_next(this_node));
				}

				static void unlink_after(node* prev_node, node* last_node)
				{  node_traits::set_next(prev_node, last_node);  }

				static void link_after( node* prev_node, node* this_node)
				{
					node_traits::set_next(this_node, node_traits::get_next(prev_node));
					node_traits::set_next(prev_node, this_node);
				}

				static void incorporate_after( node* bp, node* b, node* be)
				{
					node* p(node_traits::get_next(bp));
					node_traits::set_next(bp, b);
					node_traits::set_next(be, p);
				}

				static void transfer_after( node* bp,  node* bb, node* be)
				{
					if (bp != bb && bp != be && bb != be) {
						node* next_b = node_traits::get_next(bb);
						node* next_e = node_traits::get_next(be);
						node* next_p = node_traits::get_next(bp);
						node_traits::set_next(bb, next_e);
						node_traits::set_next(be, next_p);
						node_traits::set_next(bp, next_b);
					}
				}

			};
			
		}

	}
}