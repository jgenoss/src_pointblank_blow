#pragma once


namespace i3
{
	namespace intrusive
	{

		template<class NodeTraits>
		struct list_algorithm
		{
			typedef NodeTraits								node_traits;
			typedef typename node_traits::node				node;
		
			static void init(node* n) 
			{
				node_traits::set_next(n, nullptr);	node_traits::set_prev(n, nullptr);
			}
			static bool is_inited(const node* n) { return !node_traits::get_next(n); }

			static void init_head(node* n) 
			{ 
				node_traits::set_next(n,n);  node_traits::set_prev(n,n);
			}

			static bool unique(const node* n) 
			{
				node* next = node_traits::get_next(n);
				return !next || next == n;
			}

			// circular_list 특성에 의한 노드 수 카운팅
			static std::size_t count(const node* n) 
			{
				std::size_t result = 0;
				const node* p = n;
				do 
				{
					p = node_traits::get_next(p);
					++result;
				} while ( p != n);
				return result;
			}

			static node* unlink( node* n)
			{
				node* next = node_traits::get_next(n);
				if (!next) return n;
				node* prev = node_traits::get_prev(n);
				node_traits::set_next(prev, next);
				node_traits::set_prev(next, prev);
				return next;
			}

			static void		unlink( node* beg, node* end)
			{
				if (beg == end ) return;
				node* prev_beg = node_traits::get_prev(beg);
				node_traits::set_prev(end, prev_beg);
				node_traits::set_next(prev_beg, end);
			}

			static void		link_before( node* next_node,  node* n)
			{
				node* next_prev = node_traits::get_prev(next_node);
				node_traits::set_prev(n, next_prev);
				node_traits::set_next(n, next_node);
				node_traits::set_prev(next_node, n);
				node_traits::set_next(next_prev, n);
			}

			static void		link_after( node* prev_node,  node* n)
			{
				node* prev_next = node_traits::get_next(prev_node);
				node_traits::set_prev(n, prev_node);
				node_traits::set_next(n, prev_next);
				node_traits::set_next(prev_node, n);
				node_traits::set_prev(prev_next, n);
			}

			static void		swap_nodes( node* lhs,  node* rhs)
			{
				if ( lhs == rhs )
					return;

				bool lhs_inited = is_inited(lhs);
				bool rhs_inited = is_inited(rhs);
				if (lhs_inited)
					init_head(lhs);
				if (rhs_inited)
					init_head(rhs);

				node* next_lhs = node_traits::get_next(lhs);
				node* prev_lhs = node_traits::get_prev(lhs);
				node* next_rhs = node_traits::get_next(rhs);
				node* prev_rhs = node_traits::get_prev(rhs);

				swap_prev(next_lhs, next_rhs);
				swap_next(prev_lhs, prev_rhs);
				swap_next(lhs, rhs);
				swap_prev(lhs, rhs);

				if (lhs_inited)
					init(rhs);
				if (rhs_inited)
					init(lhs);
			}

			static void transfer( node* pos,  node* beg, node* end)
			{
				if ( beg == end ) return;

				node* pos_prev = node_traits::get_prev(pos);
				node* beg_prev = node_traits::get_prev(beg);
				node* end_prev = node_traits::get_prev(end);

				node_traits::set_next(end_prev, pos);
				node_traits::set_prev(pos, end_prev);
				node_traits::set_next(beg_prev, end);
				node_traits::set_prev(end, beg_prev);
				node_traits::set_next(pos_prev, beg);
				node_traits::set_prev(beg, pos_prev);
			}

			static void transfer( node* pos, node* beg)
			{
				node* end = node_traits::get_next(beg);

				if ( end != pos && beg != pos)
				{
					node* pos_prev = node_traits::get_prev(pos);
					node* beg_prev = node_traits::get_prev(beg);

					node_traits::set_next(pos_prev, beg);
					node_traits::set_prev(beg, pos_prev);
					node_traits::set_next(beg, pos);
					node_traits::set_prev(pos, beg);
					node_traits::set_prev(end, beg_prev);
					node_traits::set_next(beg_prev, end);
				}
			}

			static void reverse( node* pos)
			{
				node* f = node_traits::get_next(pos);
				node* i = node_traits::get_next(f);
				node* e = pos;

				while ( i != e )
				{
					node* n = i;
					i = node_traits::get_next(i);
					transfer(f, n, i);
					f = n;
				}
			}

		private:
			static void swap_prev(node* lhs, node* rhs)
			{
				node* tmp = node_traits::get_prev(lhs);
				node_traits::set_prev(lhs, node_traits::get_prev(rhs));
				node_traits::set_prev(rhs, tmp);
			}
			static void swap_next(node* lhs, node* rhs)
			{
				node* tmp = node_traits::get_next(lhs);
				node_traits::set_next(lhs, node_traits::get_next(rhs));
				node_traits::set_next(rhs, tmp);
			}
		};


	}
	
}