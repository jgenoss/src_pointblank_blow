#pragma once
#include "smart_slist_fwd.h"

#include "detail/smart_slist_iterator.h"
#include "detail/smart_head_holder.h"
#include "detail/last_holder.h"
#include "detail/size_holder.h"

#include "circular_slist_algorithm.h"
#include "linear_slist_algorithm.h"

#include "../utility/policy_selector.h"
#include "option.h"
#include "detail/default_hook.h"
#include "detail/get_smart_value_traits.h"
#include "detail/smart_hook_traits.h"
#include "smart_slist_hook.h"

#include "detail/equal_to_value.h"

#include "../utility/get_parent_from_member.h"

#include "../../smart_ptr/shared_ptr_fwd.h"
#include "../../smart_ptr/intrusive_ptr_fwd.h"

namespace i3
{
	namespace intrusive
	{

		template<class ValueTraits, class Linear, class ConstantTimeSize, class CacheLast>
		class smart_basic_slist
		{
			I3_MOVABLE(smart_basic_slist);
		public:
			typedef smart_basic_slist						my_type;
			typedef ValueTraits								value_traits;

			typedef typename value_traits::value_type		value_type;
			typedef const value_type*						const_pointer;
			typedef value_type*								pointer;
			typedef const value_type&						const_reference;
			typedef value_type&								reference;
			
			typedef std::ptrdiff_t							difference_type;
			typedef std::size_t								size_type;

			typedef smart_const_slist_iterator<value_traits>	const_iterator;
			typedef const_iterator								iterator;

			typedef typename value_traits::node_traits			node_traits;
			typedef typename node_traits::node					node;


			typedef smart_head_holder<node_traits>				head_holder_type;

			
			static const bool linear = Linear::value;
			typedef  integral_constant<bool, linear>	linear_type;
			
			typedef typename mpl::if_c<linear, 
				linear_slist_algorithm<node_traits>, 
				circular_slist_algorithm<node_traits> >::type					node_algorithm;


			static const bool constant_time_size = ConstantTimeSize::value;
			typedef integral_constant<bool, constant_time_size>	constant_time_size_type;
				
			static const bool cache_last = CacheLast::value;
			typedef  integral_constant<bool, cache_last>	cache_last_type;
			
			typedef  last_holder<cache_last, node>			last_holder_type;

			typedef size_holder<constant_time_size, size_type>		size_traits;
			
			smart_basic_slist() { set_default_constructed_state(); }
			
			template<class It>
			smart_basic_slist(It f, It l) 
			{
				set_default_constructed_state();
				insert_after(cbefore_begin(), f, l);
			}
			
			smart_basic_slist(rv_ref(smart_basic_slist) rhs)
			{
				set_default_constructed_state();
				swap(rhs);
			}

			smart_basic_slist& operator=(rv_ref(smart_basic_slist) rhs) {	swap(rhs);	return *this; }
			
			~smart_basic_slist()
			{
				const_iterator f = begin();
				const_iterator l = end();
				while (f != l)
				{
					node* to_erase = f.get_node();
					++f;
					node_algorithm::init(to_erase);
					to_erase->destroy_smart_ptr();
				}
			}
			
			void clear()
			{
				const_iterator f = begin();
				const_iterator l = end();
				while (f != l)
				{
					node* to_erase = f.get_node();
					++f;
					node_algorithm::init(to_erase);
					to_erase->destroy_smart_ptr();
				}
				set_default_constructed_state();
			}
			
			void push_front(reference val)
			{
				node* to_insert = value_traits::to_node_ptr(val);
				assert( node_algorithm::is_inited(to_insert));
				to_insert->init_smart_ptr(val);
				set_last_node_if_empty(to_insert, cache_last_type());
				node_algorithm::link_after(get_root_node(), to_insert);
				get_size_traits().increment();
			}
			
			// cache_last가 존재할때만 인터페이스가 존재하도록..
			void push_back(reference val) { push_back_impl(val, cache_last_type()); }


			void pop_front()
			{
				node* to_erase = node_traits::get_next(get_root_node());
				node_algorithm::unlink_after(get_root_node());
				get_size_traits().decrement();
				node_algorithm::init(to_erase);
				to_erase->destroy_smart_ptr();
				set_last_node_if_empty(get_root_node(), cache_last_type());
			}
			
			const_reference	front() const { return value_traits::to_value_reference(node_traits::get_next(get_root_node())); }

			const_reference back() const { return back_impl(cache_last_type()); }

			const_iterator begin() const  { return const_iterator(node_traits::get_next(get_root_node()));  }
			const_iterator cbegin() const { return const_iterator(node_traits::get_next(get_root_node()));  }
			
			const_iterator end() const { return const_iterator(const_cast<node*>(get_end_node())); }
			const_iterator cend() const { return const_iterator(const_cast<node*>(get_end_node())); }

			const_iterator before_begin() const {  return const_iterator(const_cast<node*>(get_root_node())); }
			const_iterator cbefore_begin() const {  return const_iterator(const_cast<node*>(get_root_node())); }


			const_iterator last() const { return last_impl(cache_last_type());  }
			const_iterator clast() const { return last_impl(cache_last_type());  }

			size_type size() const { return size_impl(constant_time_size_type()); }

			bool empty() const { return node_algorithm::unique(get_root_node()); }

			void swap(smart_basic_slist& rhs)
			{
				swap_impl1(rhs, cache_last_type());
				swap_size_impl(rhs, constant_time_size_type());				
			}
			
			iterator insert_after(const_iterator before, reference val)
			{
				node* to_insert = value_traits::to_node_ptr(val);
				assert( node_algorithm::is_inited(to_insert));
				to_insert->init_smart_ptr(val);
				
				node* prev = before.get_node();
				node_algorithm::link_after(prev, to_insert);
				
				set_last_node_if_last(prev, to_insert, cache_last_type());
				get_size_traits().increment();
				return iterator(to_insert);				
			}
			
			template<class It>
			void	insert_after(const_iterator before, It f, It l)
			{
				size_type count = 0;
				node* prev = before.get_node();
				
				for ( ; f != l ; ++f, ++count)
				{
					node* n = value_traits::to_node_ptr(const_cast<reference>(*f) );
					assert( node_algorithm::is_inited(n));
					n->init_smart_ptr(*f);
					node_algorithm::link_after(prev, n);
					prev = n;
				}

				set_last_node_if_last(before.get_node(), prev, cache_last_type());
				get_size_traits().increase(count);
			}
			
			iterator insert(const_iterator pos, reference val)
			{
				return insert_after(previous(pos), val);
			}

			template<class It>
			void insert(const_iterator pos, It f, It l)
			{
				insert_after(previous(pos), f, l);
			}
			
			iterator erase_after(const_iterator before)
			{
				const_iterator it = before;	++it;
				node* to_erase = it.get_node();	++it;
				node* prev = before.get_node();

				node_algorithm::unlink_after(prev);

				set_last_node_if_last(to_erase, prev, cache_last_type());
				node_algorithm::init(to_erase);
				get_size_traits().decrement();

				to_erase->destroy_smart_ptr();
				return iterator(it.get_node());
			}
			
			iterator erase_after(const_iterator before, const_iterator l)
			{
				node* before_node = before.get_node();
				node* last_node   = l.get_node();
				node* first_node = node_traits::get_next(before_node);
				
				node_algorithm::unlink_after(before_node, last_node);

				while ( first_node != last_node)
				{
					node* to_erase = first_node;
					first_node = node_traits::get_next(first_node);
					node_algorithm::init(to_erase);
					get_size_traits().decrement();
					to_erase->destroy_smart_ptr();
				}

				set_last_node_if_end(node_traits::get_next(before_node), before_node);
				return iterator(l);				
			}

			iterator erase(const_iterator pos) { return erase_after(previous(pos)); }
			iterator erase(const_iterator f, const_iterator l) { return erase_after(previous(f), l); }

			template<class It>
			void assign(It f, It l)
			{
				clear();
				insert_after(cbefore_begin(), f, l);
			}
			
			void splice_after(const_iterator before_pos, my_type& rhs)
			{
				if (rhs.empty()) return;
				if (linear && empty()) { swap(rhs); return; }
				
				const_iterator last_rhs = rhs.previous(rhs.end());
				node* prev_node = before_pos.get_node();
				node* last_rhs_node = last_rhs.get_node();

				splice_after_impl_set_last_node(rhs, prev_node, last_rhs_node, cache_last_type());
				
				node_algorithm::transfer_after(prev_node, rhs.before_begin().get_node(), last_rhs_node);
				get_size_traits().increase(rhs.get_size_traits().get_size());
				rhs.get_size_traits().set_size(size_type(0));
			}

			void splice_after(const_iterator before_pos, my_type& rhs, const_iterator* l)
			{
				if (rhs.empty()) { *l = before_pos; return; }
				if (linear && empty()) { swap(rhs);	 *l = previous(cend()); return; }

				const_iterator last_rhs = rhs.previous(rhs.end());
				node* prev_node = before_pos.get_node();
				node* last_rhs_node = last_rhs.get_node();

				splice_after_impl_set_last_node(rhs, prev_node, last_rhs_node, cache_last_type());

				node_algorithm::transfer_after(prev_node, rhs.before_begin().get_node(), last_rhs_node);
				get_size_traits().increase(rhs.get_size_traits().get_size());
				rhs.get_size_traits().set_size(size_type(0));
				*l = last_rhs;
			}

			void splice_after(const_iterator before_pos, my_type& rhs, const_iterator before_target)
			{
				const_iterator target = before_target;		++target;
				splice_after(before_pos, rhs, before_target, target);
			}

			void splice_after(const_iterator before_pos, my_type& rhs, const_iterator before_f, const_iterator before_l)
			{
				splice_after_impl_by_cts(before_pos, rhs, before_f, before_l, constant_time_size_type());
			}

			void splice(const_iterator pos, my_type& rhs) { splice_after(previous(pos), rhs); }
			void splice(const_iterator pos, my_type& rhs, const_iterator target) { splice_after(previous(pos), rhs, rhs.previous(target)); }
			void splice(const_iterator pos, my_type& rhs, const_iterator f, const_iterator l)
			{
				splice_after(previous(pos), rhs, rhs.previous(f), rhs.previous(l));
			}
			

			template<class Pred>
			void sort(Pred pr)
			{
				if (node_traits::get_next(node_traits::get_next(get_root_node()))	
					!= get_root_node()) 
				{
					my_type carry;
					my_type counter[64]; 
					int fill = 0;
					const_iterator last_inserted;
					
					while(!empty())
					{
						last_inserted = cbegin();
						carry.splice_after(carry.cbefore_begin(), *this, this->cbefore_begin());
						int i = 0;
						while(i < fill && !counter[i].empty()) 
						{
							carry.swap(counter[i]);
							carry.merge(counter[i++], pr, &last_inserted);
						}
						
						const_iterator last_element(carry.previous(last_inserted, carry.end()));

						if(constant_time_size)
						{
							counter[i].splice_after_impl( counter[i].cbefore_begin(), carry
									, carry.cbefore_begin(), last_element
									, carry.size());
						}
						else
						{
							counter[i].splice_after( counter[i].cbefore_begin(), carry
								, carry.cbefore_begin(), last_element);
						}
						if(i == fill)
							++fill;
					}

					for (int i = 1; i < fill; ++i)
						counter[i].merge(counter[i-1], pr, &last_inserted);
					--fill;
					
					const_iterator last_element(counter[fill].previous(last_inserted, counter[fill].end()));
					if(constant_time_size)
					{
						splice_after_impl( cbefore_begin(), counter[fill], counter[fill].cbefore_begin()
							, last_element, counter[fill].size());
					}
					else
					{
						splice_after( cbefore_begin(), counter[fill], counter[fill].cbefore_begin()
							, last_element);
					}
				}
			}

			void sort()
			{
				sort(i3::less<value_type>());
			}
			
			template<class Pred>
			void merge(my_type& x, Pred pr, const_iterator* l)
			{
				const_iterator e(cend()), ex(x.cend()), bb(cbefore_begin()), bb_next;
				*l = e;

				while(!x.empty())
				{
					const_iterator ibx_next(x.cbefore_begin()), ibx(ibx_next++);
					while (++(bb_next = bb) != e && !pr(*ibx_next, *bb_next))
					{
						bb = bb_next;
					}
					if(bb_next == e)
					{
						splice_after(bb, x, l);
						break;
					}
					else
					{
						size_type n(0);
						do
						{
							ibx = ibx_next; ++n;
						} while(++(ibx_next = ibx) != ex && pr(*ibx_next, *bb_next));

						splice_after_impl(bb, x, x.before_begin(), ibx, n);
						*l = ibx;
					}
				}
			}

			template<class Pred>
			void merge(my_type& x, Pred pr)
			{
				const_iterator e(cend()), ex(x.cend()), bb(cbefore_begin()), bb_next;
			
				while(!x.empty())
				{
					const_iterator ibx_next(x.cbefore_begin()), ibx(ibx_next++);
					while (++(bb_next = bb) != e && !pr(*ibx_next, *bb_next))
					{
						bb = bb_next;
					}
					if(bb_next == e)
					{
						splice_after(bb, x);
						break;
					}
					else
					{
						size_type n(0);
						do
						{
							ibx = ibx_next; ++n;
						} while(++(ibx_next = ibx) != ex && pr(*ibx_next, *bb_next));

						splice_after_impl(bb, x, x.before_begin(), ibx, n);
					}
				}
			}

	
			void merge(my_type& rhs) {	merge(rhs, i3::less<value_type>());	}

			void reverse()
			{
				reverse_impl1(cache_last_type());
				reverse_impl2(linear_type());
			}

			void remove(const_reference val)
			{
				remove_if(detail::equal_to_value<const reference>(val)); 
			}
			
			template<class Pred>
			void remove_if(Pred pred)
			{
				const_iterator bcur(before_begin()), cur(begin()), e(end());

				while(cur != e)
				{
					if (pred(*cur))
					{
						cur = erase_after(bcur);
					}
					else
					{
						bcur = cur;
						++cur;
					}
				}

				set_last_node_dispatch(bcur.get_node(), cache_last_type());
			}

			void unique() { unique(i3::equal_to<value_type>()); }
			
			template<class Pred2>
			void unique(Pred2 pred)
			{
				const_iterator end_n(cend());
				const_iterator bcur(cbegin());

				if(bcur != end_n)
				{
					const_iterator cur(bcur);
					++cur;
					while(cur != end_n) 
					{
						if (pred(*bcur, *cur))
						{
							cur = this->erase_after(bcur);
						}
						else
						{
							bcur = cur;
							++cur;
						}
					}

					set_last_node_dispatch(bcur.get_node(), cache_last_type());
				}
			}

			static const_iterator static_value_to_iterator(const_reference val)
			{
				const node* n = value_traits::to_node_ptr(val);
				assert(linear || !node_algorithm::is_inited(n));
				return const_iterator(const_cast<node*>(n));
			}
			
			const_iterator value_to_iterator(const_reference val)
			{
				const node* n = value_traits::to_node_ptr(val);
				if (node_algorithm::is_inited(n) ) return cend();
				return const_iterator(const_cast<node*>(n));
			}

			const_iterator previous(const_iterator pos) const
			{
				return previous(cbefore_begin(), pos);
			}

			const_iterator previous(const_iterator prev_from, const_iterator pos) const
			{
				return previous_dispatch(prev_from, pos, cache_last_type());
			}

			static my_type& end_iterator_to_container(const_iterator e)
			{
				return end_iterator_to_container_impl(e, linear_type());
			}

		private:
			static my_type& end_iterator_to_container_impl(const_iterator e, true_type)
			{
				compile_assert_msg(0, _not_supported_if_linear_slist_);
			}

			static my_type& end_iterator_to_container_impl(const_iterator e, false_type)
			{
				node* n = e.get_node();
				typedef typename node_traits::head_node_type	head_node_type;
				head_holder_type* h = static_cast<head_holder_type*>( static_cast<head_node_type*>(n));
				head_plus_last* hpl = get_parent_from_member<head_plus_last, head_holder_type>(h, &head_plus_last::m_head);
				root_plus_size* r = get_parent_from_member<root_plus_size, head_plus_last>(hpl, &root_plus_size::m_root);
				my_type* mine = get_parent_from_member<my_type, root_plus_size>(r, &my_type::m_data);
				return *mine;
			}

			const_iterator  last_impl(false_type) const { compile_assert_msg(0, _not_supported_if_not_cache_last_); return const_iterator();  }
			const_iterator  last_impl(true_type) const { return const_iterator( const_cast<node*>(get_last_node()));  }

			const_reference back_impl(false_type) const { compile_assert_msg(0, _not_supported_if_not_cache_last_); return value_traits::to_value_reference( get_last_node() ); }
			const_reference back_impl(true_type) const { return value_traits::to_value_reference( get_last_node() ); }

			void push_back_impl(reference, false_type)
			{
				compile_assert_msg(0, _not_supported_if_not_cache_last_);
			}
			
			void push_back_impl(reference val, true_type)
			{
				node* to_insert = value_traits::to_node_ptr(val);				
				assert( node_algorithm::is_inited(to_insert));
				to_insert->init_smart_ptr(val);
				node_algorithm::link_after(get_last_node(), to_insert);
				set_last_node(to_insert);
				get_size_traits().increment();
			}


			void splice_after_impl(node* before_pos, my_type& rhs, node* before_f, node* before_l)
			{
				splice_after_impl_set_last_node2(before_pos, rhs, before_f, before_l, cache_last_type());
				node_algorithm::transfer_after(before_pos, before_f, before_l);				
			}

			void splice_after_impl_set_last_node2(node* before_pos, my_type& rhs, node* before_f, node* before_l, true_type)
			{
				if (before_f == before_l) return;

				if (before_pos == get_last_node() )
					set_last_node(before_l);
				if (this != &rhs && node_traits::get_next(before_l) == rhs.get_end_node() )
					rhs.set_last_node(before_f);
			}

			void splice_after_impl_set_last_node2(node* , my_type& , node* , node* , false_type){}


			void splice_after_impl(const_iterator before_pos, my_type& rhs, const_iterator before_f, const_iterator before_l, size_type n)
			{
				splice_after_impl(before_pos.get_node(), rhs, before_f.get_node(), before_l.get_node());		
				get_size_traits().increase(n);
				rhs.get_size_traits().decrease(n);
			}


			void splice_after_impl_set_last_node(my_type& rhs, node* prev_node, node* last_rhs_node, true_type)
			{
				rhs.set_last_node(rhs.get_root_node());
				if (node_traits::get_next(prev_node) == get_end_node() )
					set_last_node(last_rhs_node);
			}
			
			void splice_after_impl_set_last_node(my_type& , node* , node* , false_type){}


			void splice_after_impl_by_cts(const_iterator before_pos, my_type& rhs, const_iterator before_f, const_iterator before_l, true_type)
			{
				splice_after_impl(before_pos, rhs, before_f, before_l, i3::distance(before_f, before_l));
			}

			void splice_after_impl_by_cts(const_iterator before_pos, my_type& rhs, const_iterator before_f, const_iterator before_l, false_type)
			{
				splice_after_impl(before_pos.get_node(), rhs, before_f.get_node(), before_l.get_node());
			}



			void swap_impl1(smart_basic_slist& rhs, true_type)
			{
				bool other_was_empty = false;
				my_type* this_ptr = this;
				my_type* rhs_ptr  = &rhs;

				if(empty())
				{
					if(rhs.empty()) return;
					
					my_type* tmp = this_ptr;
					this_ptr  = rhs_ptr;
					rhs_ptr = tmp;
					other_was_empty = true;
				}
				else
				{
					other_was_empty = rhs.empty();
				}

				//Precondition: this is not empty
				node* other_old_last = rhs_ptr->get_last_node();
				node* other_bfirst = rhs_ptr->get_root_node();
				node* this_bfirst = this_ptr->get_root_node();
				node* this_old_last = this_ptr->get_last_node();

				//Move all nodes from this to other's beginning
				node_algorithm::transfer_after(other_bfirst, this_bfirst, this_old_last);
				rhs_ptr->set_last_node(this_old_last);

				if(other_was_empty)
				{
					this_ptr->set_last_node(this_bfirst);
				}
				else
				{
					//Move trailing nodes from other to this
					node_algorithm::transfer_after(this_bfirst, this_old_last, other_old_last);
					this_ptr->set_last_node(other_old_last);
				}
			}

			void swap_impl1(smart_basic_slist& rhs, false_type)
			{
				swap_impl2( get_root_node(), rhs.get_root_node(), linear_type());
			}
			
			void swap_impl2(node* lhs_node, node* rhs_node, true_type)
			{
				node_algorithm::swap_trailing_nodes(lhs_node, rhs_node);
			}
			void swap_impl2(node* lhs_node, node* rhs_node, false_type)
			{
				node_algorithm::swap_nodes(lhs_node, rhs_node);
			}

			void	swap_size_impl(my_type& rhs, true_type) 
			{
				size_type tmp = get_size_traits().get_size();	get_size_traits().set_size(rhs.get_size_traits().get_size());		rhs.get_size_traits().set_size(tmp);
			}
			void	swap_size_impl(my_type&, false_type) {}


			size_type size_impl(true_type) const { return get_size_traits().get_size(); }
			size_type size_impl(false_type) const { return node_algorithm::count(get_root_node()) - 1; }

			node*		get_end_node() { return get_end_node_impl(linear_type()); }
			const node* get_end_node() const { return get_end_node_impl(linear_type()); }
			
			node*		get_root_node() {  return m_data.m_root.m_head.get_node(); }
			const node*	get_root_node() const { return m_data.m_root.m_head.get_node(); }

			node*		get_last_node() { return  m_data.m_root.get_last(); }
			const node*	get_last_node() const { return m_data.m_root.get_last(); }
			void		set_last_node(node* n) { m_data.m_root.set_last(n); }
			
			size_traits&		get_size_traits() { return m_data; }
			const size_traits&	get_size_traits() const { return m_data; }

			void		set_default_constructed_state()
			{
				node_algorithm::init_head(get_root_node());
				get_size_traits().set_size(size_type(0));
				set_last_node_dispatch(get_root_node(), cache_last_type());
			}

			node*		get_end_node_impl(true_type) const { return nullptr; }
			node*		get_end_node_impl(false_type) { return get_root_node(); }
			const node*	get_end_node_impl(false_type) const { return get_root_node(); }
			
			void		set_last_node_if_empty(node* n, true_type)
			{
				if ( empty() )
					set_last_node(n);
			}
			void		set_last_node_if_empty(node*, false_type) {}

			void		set_last_node_dispatch(node* n, true_type) { set_last_node(n); }
			void		set_last_node_dispatch(node* n, false_type) {}

			void		set_last_node_if_last(node* prev, node* n, true_type)
			{
				if (get_last_node() == prev)
					set_last_node(n);
			}
			void		set_last_node_if_last(node* , node* , false_type) {}

			void		set_last_node_if_end(node* comp_n, node* n, true_type)
			{
				if (get_end_node() == comp_n)
					set_last_node(n);
			}
			void		set_last_node_if_end(node*, node*, false_type) {}


			void reverse_impl1(true_type)
			{
				if (!empty())
					set_last_node(node_traits::get_next(get_root_node()));
			}

			void reverse_impl1(false_type) {}
			
			void reverse_impl2(true_type)
			{
				node* new_first = node_algorithm::reverse(node_traits::get_next(get_root_node()));
				node_traits::set_next(get_root_node(), new_first);
			}

			void reverse_impl2(false_type)
			{
				node_algorithm::reverse(get_root_node());
			}

			const_iterator previous_dispatch(const_iterator prev_from, const_iterator pos, true_type) const
			{
				if (pos.get_node() == get_end_node() )
					return const_iterator(const_cast<node*>(get_last_node()));
				return const_iterator(node_algorithm::get_previous_node(prev_from.get_node(), pos.get_node()));
			}
			const_iterator previous_dispatch(const_iterator prev_from, const_iterator pos, false_type) const
			{
				return const_iterator(node_algorithm::get_previous_node(prev_from.get_node(), pos.get_node()));
			}
		



			struct head_plus_last : public last_holder_type
			{
				head_holder_type	m_head;
			};
			
			struct root_plus_size : public size_traits
			{
				head_plus_last	m_root;
			};
			
			root_plus_size	m_data;
		};

		namespace detail
		{
			template<class V, class L, class C1, class C2> inline
			bool smart_basic_slist_equal_impl(const smart_basic_slist<V,L,C1,C2>& Left, const smart_basic_slist<V,L,C1,C2>& Right, true_type)
			{
				return Left.size() == Right.size()	&& equal(Left.begin(), Left.end(), Right.begin());
			}

			template<class V, class L, class C1, class C2> inline
			bool smart_basic_slist_equal_impl(const smart_basic_slist<V,L,C1,C2>& Left, const smart_basic_slist<V,L,C1,C2>& Right, false_type)
			{
				typedef typename smart_basic_slist<V,L,C1,C2>::const_iterator	const_iterator;
				
				const_iterator end1= Left.end();
				const_iterator i1 = Left.begin();
				const_iterator i2 = Right.begin();
				const_iterator end2 = Right.end();
				while (i1 != end1 && i2 != end2 && *i1 == *i2) 
				{
					++i1;
					++i2;
				}
				return i1 == end1 && i2 == end2;
			}
		}

		template<class V, class L, class C1, class C2> inline
		void swap(const smart_basic_slist<V,L,C1,C2>& Left, const smart_basic_slist<V,L,C1,C2>& Right) { Left.swap(Right); }

		template<class V, class L, class C1, class C2> inline
		bool operator==(const smart_basic_slist<V,L,C1,C2>& Left, const smart_basic_slist<V,L,C1,C2>& Right)
		{	
			typedef typename smart_basic_slist<V,L,C1,C2>::constant_time_size_type	dispatch_type;
			return detail::smart_basic_slist_equal_impl(Left, Right, dispatch_type());
		}


		template<class V, class L, class C1, class C2> inline
		bool operator!=(const smart_basic_slist<V,L,C1,C2>& Left, const smart_basic_slist<V,L,C1,C2>& Right) {	return !(Left == Right);	}

		template<class V, class L, class C1, class C2> inline
		bool operator<(const smart_basic_slist<V,L,C1,C2>& Left, const smart_basic_slist<V,L,C1,C2>& Right) {	return lexicographical_compare(Left.begin(), Left.end(), Right.begin(), Right.end()); }

		template<class V, class L, class C1, class C2> inline
		bool operator>(const smart_basic_slist<V,L,C1,C2>& Left, const smart_basic_slist<V,L,C1,C2>& Right) {	return Right < Left; }

		template<class V, class L, class C1, class C2> inline
		bool operator<=(const smart_basic_slist<V,L,C1,C2>& Left, const smart_basic_slist<V,L,C1,C2>& Right) {	return !(Right < Left); }

		template<class V, class L, class C1, class C2> inline
		bool operator>=(const smart_basic_slist<V,L,C1,C2>& Left, const smart_basic_slist<V,L,C1,C2>& Right) {	return !(Left < Right);	}

		template<class SmartPtr, class O1 = mpl::na, class O2 = mpl::na, 
		 						 class O3 = mpl::na, class O4 = mpl::na>
		struct make_smart_slist
		{
			typedef i3::policy_selector<O1,O2,O3,O4>	selector;

			typedef typename selector::template 
				find_with_policy_tag<hook_policy_tag, mpl::identity<default_smart_slist_hook> >::type hook_policy;

			typedef typename hook_policy::type			proto_hook_type;
			typedef typename get_smart_value_traits<SmartPtr, proto_hook_type>::type value_traits;
			
			typedef typename selector::template
				find_with_policy_tag<linear_policy_tag, false_type>::type		linear_policy;
			typedef typename selector::template
				find_with_policy_tag<constant_time_size_policy_tag, false_type>::type constant_time_size_policy;
			typedef typename selector::template
				find_with_policy_tag<cache_last_policy_tag, false_type>::type   cache_last_policy;

			typedef typename linear_policy::type					linear;
			typedef typename constant_time_size_policy::type		constant_time_size;
			typedef typename cache_last_policy::type				cache_last;
			
			typedef smart_basic_slist<value_traits, linear, constant_time_size, cache_last>	type;
		};

		template<class T, class O1, class O2, class O3, class O4>
		class shared_slist : public make_smart_slist<i3::shared_ptr<T>, O1, O2, O3, O4>::type
		{
			I3_MOVABLE(shared_slist);
			typedef typename make_smart_slist<i3::shared_ptr<T>, O1, O2, O3, O4>::type	base_type;
			typedef shared_slist														this_type;
		public:
			typedef typename base_type::value_traits		value_traits;
			typedef typename base_type::iterator			iterator;
			typedef typename base_type::const_iterator		const_iterator;

			this_type()	{}
			template<class It>
			this_type(It b, It e) :  base_type(b, e) {}
			this_type(rv_ref(this_type) x) :  base_type(i3::move(static_cast<base_type&>(x))) {}

			this_type& operator=(rv_ref(this_type) x)
			{  return static_cast<this_type&>(base_type::operator=(i3::move(static_cast<base_type&>(x))));  }

			static this_type& end_iterator_to_container(const_iterator e)
			{  return static_cast<this_type&>(base_type::end_iterator_to_container(e));   }
		};

		template<class T, class O1, class O2, class O3, class O4>
		class intrusive_slist : public make_smart_slist<i3::intrusive_ptr<T>, O1, O2, O3, O4>::type
		{
			I3_MOVABLE(intrusive_slist);
			typedef typename make_smart_slist<i3::intrusive_ptr<T>, O1, O2, O3, O4>::type	base_type;
			typedef intrusive_slist															this_type;
		public:
			typedef typename base_type::value_traits		value_traits;
			typedef typename base_type::iterator			iterator;
			typedef typename base_type::const_iterator		const_iterator;

			this_type()	{}
			template<class It>
			this_type(It b, It e) :  base_type(b, e) {}
			this_type(rv_ref(this_type) x) :  base_type(i3::move(static_cast<base_type&>(x))) {}

			this_type& operator=(rv_ref(this_type) x)
			{  return static_cast<this_type&>(base_type::operator=(i3::move(static_cast<base_type&>(x))));  }

			static this_type& end_iterator_to_container(const_iterator e)
			{  return static_cast<this_type&>(base_type::end_iterator_to_container(e));   }
		};

		template<class T, class O1, class O2, class O3, class O4>
		class raw_slist : public make_smart_slist<T*, O1, O2, O3, O4>::type
		{
			I3_MOVABLE(raw_slist);
			typedef typename make_smart_slist<T*, O1, O2, O3, O4>::type	base_type;
			typedef raw_slist											this_type;
		public:
			typedef typename base_type::value_traits		value_traits;
			typedef typename base_type::iterator			iterator;
			typedef typename base_type::const_iterator		const_iterator;

			this_type()	{}
			template<class It>
			this_type(It b, It e) :  base_type(b, e) {}
			this_type(rv_ref(this_type) x) :  base_type(i3::move(static_cast<base_type&>(x))) {}

			this_type& operator=(rv_ref(this_type) x)
			{  return static_cast<this_type&>(base_type::operator=(i3::move(static_cast<base_type&>(x))));  }

			static this_type& end_iterator_to_container(const_iterator e)
			{  return static_cast<this_type&>(base_type::end_iterator_to_container(e));   }
		};



	}
}
