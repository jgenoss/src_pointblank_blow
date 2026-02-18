#pragma once

#include "smart_list_fwd.h"

#include "detail/smart_list_iterator.h"
#include "detail/smart_head_holder.h"
#include "detail/size_holder.h"
#include "list_algorithm.h"

#include "../utility/policy_selector.h"
#include "option.h"
#include "detail/default_hook.h"
#include "detail/get_smart_value_traits.h"
#include "detail/smart_hook_traits.h"
#include "smart_list_hook.h"

#include "detail/equal_to_value.h"
#include "../utility/get_parent_from_member.h"

#include "../../smart_ptr/shared_ptr_fwd.h"
#include "../../smart_ptr/intrusive_ptr_fwd.h"

namespace i3
{
	namespace intrusive
	{
		// 비상수 참조 접근함수와 비상수반복자를 제거했음...
		// 비상수 참조허용하고 값변경시 --> 스마트포인터 삭제 -> 내부 노드데이터제거가 되므로 하면 안된다.

		template<class ValueTraits, class ConstantTimeSize>
		class smart_basic_list
		{
			I3_MOVABLE(smart_basic_list);
		public:
			typedef smart_basic_list					my_type;

			typedef ValueTraits							value_traits;
			typedef typename value_traits::value_type	value_type;

			typedef const value_type*					const_pointer;
			typedef value_type*							pointer;
			typedef const value_type&					const_reference;
			typedef value_type&							reference;

			typedef std::ptrdiff_t						difference_type;
			typedef std::size_t							size_type;
		
		
			typedef smart_const_list_iterator<value_traits>		const_iterator;
			typedef const_iterator								iterator;

			typedef i3::reverse_iterator<const_iterator>		const_reverse_iterator;
			typedef const_reverse_iterator						reverse_iterator;

			typedef typename value_traits::node_traits			node_traits;
			typedef typename node_traits::node					node;
			
			typedef list_algorithm<node_traits>					node_algorithm;
			typedef smart_head_holder<node_traits>				head_holder_type;

			static const bool constant_time_size = ConstantTimeSize::value;
			typedef integral_constant<bool, constant_time_size>	constant_time_size_type;

			typedef size_holder<constant_time_size, size_type>			size_traits;

			smart_basic_list()
			{
				get_size_traits().set_size(size_type(0));
				node_algorithm::init_head( get_root_node());
			}
		
			template<class It>
			smart_basic_list(It f, It l)
			{
				get_size_traits().set_size(size_type(0));
				node_algorithm::init_head( get_root_node());
				insert(cend(), f, l);
			}

			smart_basic_list(rv_ref(smart_basic_list) rhs) 
			{
				get_size_traits().set_size(size_type(0));
				node_algorithm::init_head( get_root_node());
				swap(rhs);
			}
		
			smart_basic_list& operator=(rv_ref(smart_basic_list) rhs) {	swap(rhs);	return *this; }

			~smart_basic_list()
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

			void push_back(reference val)
			{
				node* to_insert = value_traits::to_node_ptr(val);

				assert( node_algorithm::is_inited(to_insert));

				to_insert->init_smart_ptr(val);
				node_algorithm::link_before(get_root_node(), to_insert);		
				get_size_traits().increment();
			}
			
			void push_front(reference val)
			{
				node* to_insert = value_traits::to_node_ptr(val);
				assert( node_algorithm::is_inited(to_insert));
				to_insert->init_smart_ptr(val);
				node_algorithm::link_before(node_traits::get_next(get_root_node()), to_insert);		
				get_size_traits().increment();
			}

			void pop_back()
			{
				node* to_erase = node_traits::get_prev(get_root_node());
				node_algorithm::unlink(to_erase);
				get_size_traits().decrement();
				node_algorithm::init(to_erase);
				to_erase->destroy_smart_ptr();
			}

			void pop_front()
			{
				node* to_erase = node_traits::get_next(get_root_node());
				node_algorithm::unlink(to_erase);
				get_size_traits().decrement();
				node_algorithm::init(to_erase);
				to_erase->destroy_smart_ptr();
			}

		
			const_reference front() const {	return value_traits::to_value_reference(node_traits::get_next(get_root_node())); }
			const_reference back() const {	return value_traits::to_value_reference(node_traits::get_prev(get_root_node())); }

			const_iterator begin() const { return cbegin(); }
			const_iterator cbegin() const { return const_iterator(  node_traits::get_next(get_root_node()) ); }
			const_iterator end() const { return cend(); }
			const_iterator cend() const { return const_iterator(const_cast<node*>(get_root_node())); }

			const_reverse_iterator rbegin() const { return crbegin(); }
			const_reverse_iterator crbegin() const { return const_reverse_iterator(end()); }
			const_reverse_iterator rend() const { return crend(); }
			const_reverse_iterator crend() const { return const_reverse_iterator(begin()); }

			size_type size() const 	{	return priv_size(constant_time_size_type());	}
			bool	  empty() const { return node_algorithm::unique(get_root_node()); }

			void swap(smart_basic_list& rhs)
			{
				node_algorithm::swap_nodes(get_root_node(), rhs.get_root_node());
				priv_swap_size(rhs, constant_time_size_type());
			}

			iterator erase(const_iterator it)
			{
				node* to_erase = it.get_node();
				++it;
				node_algorithm::unlink(to_erase);
				get_size_traits().decrement();
				node_algorithm::init(to_erase);
				to_erase->destroy_smart_ptr();
				return iterator(it.get_node()); 
			}

			iterator erase(const_iterator f, const_iterator l)
			{
				node* beg_node = f.get_node();
				node* end_node = l.get_node();

				node_algorithm::unlink(beg_node, end_node);

				while(beg_node != end_node)
				{
					node* to_erase = beg_node;
					beg_node = node_traits::get_next(beg_node);
					node_algorithm::init(to_erase);
					get_size_traits().decrement();
					to_erase->destroy_smart_ptr();
				}

				return iterator(l);
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
				node_algorithm::init_head(get_root_node());
				get_size_traits().set_size(0);
			}

			iterator insert(const_iterator pos, reference val)
			{
				node* to_insert = value_traits::to_node_ptr(val);
				assert( node_algorithm::is_inited(to_insert));
				to_insert->init_smart_ptr(val);

				node_algorithm::link_before(pos.get_node(), to_insert);
				get_size_traits().increment();
				return iterator(to_insert);
			}

			template<class It>
			void	insert(const_iterator pos, It f, It l)
			{
				for ( ; f != l ; ++f)
					insert(pos, const_cast<reference>(*f));
			}

			template<class It>
			void assign(It f, It l)
			{
				clear();
				insert(cend(), f, l);
			}
			
			void splice(const_iterator pos, smart_basic_list& rhs)
			{
				if (rhs.empty()) return;

				node_algorithm::transfer(pos.get_node(), rhs.begin().get_node(), rhs.end().get_node());

				size_traits& rhs_size_traits = rhs.get_size_traits();
				get_size_traits().increase(rhs_size_traits.get_size());
				rhs_size_traits.set_size(size_type(0));
			}
			
			void splice(const_iterator pos, smart_basic_list& rhs, const_iterator first)
			{
				node_algorithm::transfer(pos.get_node(), first.get_node());
				rhs.get_size_traits().decrement();
				get_size_traits().increment();
			}
			
			void splice(const_iterator pos, smart_basic_list& rhs, const_iterator f, const_iterator l)
			{
				splice_impl(pos, rhs, f, l, constant_time_size_type());
			}

			void sort() {  sort(i3::less<value_type>()); }
			
			template<class Predicate>
			void sort(Predicate p)
			{
				if(node_traits::get_next(get_root_node()) != node_traits::get_prev(get_root_node()))
				{
					my_type carry;
					my_type counter[64]; 
					int fill = 0;
					while(!empty())
					{
						carry.splice(carry.cbegin(), *this, this->cbegin());
						int i = 0;
						while(i < fill && !counter[i].empty()) 
						{
							counter[i].merge(carry, p);
							carry.swap(counter[i++]);
						}
						carry.swap(counter[i]);
						if(i == fill)
							++fill;
					}
					
					for (int i = 1; i < fill; ++i)
						counter[i].merge(counter[i-1], p);

					swap(counter[fill-1]);
				}
			}

			void merge(my_type& rhs) {  merge(rhs, i3::less<value_type>()); }

			template<class Predicate>
			void merge(my_type& x, Predicate p)
			{
				const_iterator e(cend()), ex(x.cend());
				const_iterator b(cbegin());

				while(!x.empty())
				{
					const_iterator ix(x.cbegin());
					while (b != e && !p(*ix, *b)){	++b; }

					if(b == e)
					{
						//Now transfer the rest to the end of the container
						splice(e, x);
						break;
					}
					else
					{
						size_type n(0);
						do
						{
							++ix; ++n;
						} while(ix != ex && p(*ix, *b));

						splice_impl(b, x, x.begin(), ix, n);
					}
				}
			}

			void reverse() {  node_algorithm::reverse(get_root_node());   }
			
			void remove(const_reference val) {	remove_if( detail::equal_to_value(val) ); }

			template<class Pred>
			void remove_if(Pred pred)
			{
				const_iterator cur(cbegin());
				const_iterator last(cend());

				while(cur != last) 
				{
					if(pred(*cur))
						cur = erase(cur);
					else
						++cur;
				}
			}

			void unique() { unique(i3::equal_to<value_type>());  }

			template<class Pred2>
			void unique(Pred2 pred)
			{
				const_iterator itend(cend());
				const_iterator cur(cbegin());

				if(cur != itend)
				{
					const_iterator after(cur);
					++after;
					while(after != itend)
					{
						if(pred(*cur, *after))
						{
							after = erase(after);
						}
						else
						{
							cur = after;
							++after;
						}
					}
				}
			}

			static const_iterator static_value_to_iterator(const_reference val)
			{
				const node* n = value_traits::to_node_ptr(val);
				assert(!node_algorithm::is_inited(n));
				return const_iterator(const_cast<node*>(n));
			}
			
			const_iterator value_to_iterator(const_reference val) const
			{
				const node* n = value_traits::to_node_ptr(val);
				if (node_algorithm::is_inited(n) ) return cend();
				return const_iterator(const_cast<node*>(n));
			}


			static my_type& end_iterator_to_container(const_iterator e)
			{
				node* n = e.get_node();
				typedef typename node_traits::head_node_type	head_node_type;
				head_holder_type* h = static_cast<head_holder_type*>( static_cast<head_node_type*>(n));
				root_plus_size* r = get_parent_from_member<root_plus_size, head_holder_type>(h, &root_plus_size::m_head);
				my_type* mine = get_parent_from_member<my_type, root_plus_size>(r, &my_type::m_data);
				return *mine;
			}

		private:
		
			void splice_impl(const_iterator pos, smart_basic_list& rhs, const_iterator f, const_iterator l, size_type n)
			{
				node_algorithm::transfer(pos.get_node(), f.get_node(), l.get_node());
				get_size_traits().increase(n);
				rhs.get_size_traits().decrease(n);
			}


			void splice_impl(const_iterator pos, smart_basic_list& rhs, const_iterator f, const_iterator l, true_type)
			{
				difference_type n = i3::distance(f, l);
				node_algorithm::transfer(pos.get_node(), f.get_node(), l.get_node());
				get_size_traits().increase(n);
				rhs.get_size_traits().decrease(n);
			}

			void splice_impl(const_iterator pos, smart_basic_list& rhs, const_iterator f, const_iterator l, false_type)
			{
				node_algorithm::transfer(pos.get_node(), f.get_node(), l.get_node());
			}

			size_type priv_size(true_type) const { return get_size_traits().get_size(); }
			size_type priv_size(false_type) const { return node_algorithm::count(get_root_node()) - 1; }

			void	  priv_swap_size(smart_basic_list& rhs, true_type) 
			{
				size_type tmp = get_size_traits().get_size();	get_size_traits().set_size(rhs.get_size_traits().get_size());		rhs.get_size_traits().set_size(tmp);
			}
			void	priv_swap_size(smart_basic_list&, false_type) {}


			
			node* get_root_node() { return m_data.m_head.get_node(); }
			const node* get_root_node() const { return m_data.m_head.get_node(); }

			size_traits&			get_size_traits() { return m_data; }
			const size_traits&		get_size_traits() const { return m_data; }

			struct root_plus_size : public size_traits
			{
				head_holder_type	m_head;
			};

			root_plus_size		m_data;
		};
		
		/// 연산자 정의 (non constant_time_size의 list의 경우, equal에 대한 다른 정의가 필요함)
			
		template<class V, class C> inline
		void swap(smart_basic_list<V,C>& Left, smart_basic_list<V,C>& Right) {	Left.swap(Right);  }
		

		namespace detail
		{
			template<class V, class C> inline
			bool smart_basic_list_equal_impl(const smart_basic_list<V,C>& Left, const smart_basic_list<V,C>& Right, true_type)
			{
				return Left.size() == Right.size()	&& equal(Left.begin(), Left.end(), Right.begin());
			}
			
			template<class V, class C> inline
			bool smart_basic_list_equal_impl(const smart_basic_list<V,C>& Left, const smart_basic_list<V,C>& Right, false_type)
			{
				typedef typename smart_basic_list<V,C>::const_iterator	const_iterator;

				const_iterator end1 = Left.end();

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


		template<class V, class C> inline
		bool operator==(const smart_basic_list<V,C>& Left, const smart_basic_list<V,C>& Right)
		{	
			typedef typename smart_basic_list<V,C>::constant_time_size_type	dispatch_type;
			return detail::smart_basic_list_equal_impl(Left, Right, dispatch_type());
		}
		
		
		template<class V, class C> inline
		bool operator!=(const smart_basic_list<V,C>& Left, const smart_basic_list<V,C>& Right) {	return !(Left == Right);	}

		template<class V, class C> inline
		bool operator<(const smart_basic_list<V,C>& Left,	const smart_basic_list<V,C>& Right) {	return lexicographical_compare(Left.begin(), Left.end(), Right.begin(), Right.end()); }

		template<class V, class C> inline
		bool operator>(const smart_basic_list<V,C>& Left,	const smart_basic_list<V,C>& Right) {	return Right < Left; }

		template<class V, class C> inline
		bool operator<=(const smart_basic_list<V,C>& Left, const smart_basic_list<V,C>& Right) {	return !(Right < Left); }

		template<class V, class C> inline
		bool operator>=(const smart_basic_list<V,C>& Left, const smart_basic_list<V,C>& Right) {	return !(Left < Right);	}

		
		template<class SmartPtr, class O1 = mpl::na, class O2 = mpl::na>
		struct make_smart_list
		{
			typedef i3::policy_selector<O1, O2>		selector;
			typedef typename selector::template
				find_with_policy_tag<hook_policy_tag, mpl::identity<default_smart_list_hook> >::type hook_policy;

			typedef typename hook_policy::type		proto_hook_type;
			typedef typename get_smart_value_traits<SmartPtr, proto_hook_type>::type value_traits;
			
			typedef typename selector::template
				find_with_policy_tag<constant_time_size_policy_tag, true_type>::type constant_time_size_policy;

			typedef typename constant_time_size_policy::type constant_time_size;

			typedef smart_basic_list<value_traits, constant_time_size>		type;
		};


		template<class T, class O1, class O2>
		class shared_list : public make_smart_list<i3::shared_ptr<T>, O1, O2>::type
		{
			I3_MOVABLE(shared_list);
			typedef typename make_smart_list<i3::shared_ptr<T>, O1, O2>::type	base_type;
			typedef shared_list													this_type;

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
		
		template<class T, class O1, class O2>
		class intrusive_list : public make_smart_list<i3::intrusive_ptr<T>, O1, O2>::type
		{
			I3_MOVABLE(intrusive_list);
			typedef typename make_smart_list<i3::intrusive_ptr<T>, O1, O2>::type	base_type;
			typedef intrusive_list													this_type;

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
		
		template<class T, class O1, class O2>
		class raw_list : public make_smart_list<T*, O1, O2>::type
		{
			I3_MOVABLE(raw_list);
			typedef typename make_smart_list<T*, O1, O2>::type	base_type;
			typedef raw_list									this_type;

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