#pragma once

namespace i3
{
	namespace intrusive
	{
		
		template<class ValueTraits>
		class smart_const_slist_iterator
		{
		public:
			typedef ValueTraits								value_traits;
			typedef typename value_traits::node_traits		node_traits;
			typedef typename node_traits::node				node;

			typedef typename value_traits::value_type		value_type;
			typedef std::ptrdiff_t							difference_type;
			typedef const value_type*						pointer;
			typedef const value_type&						reference;
			typedef i3::forward_iterator_tag				iterator_category;
			
			smart_const_slist_iterator() : m_node(0) {}
			explicit smart_const_slist_iterator(node* n) : m_node(n) {}
			smart_const_slist_iterator& operator=(node* n) { m_node = n;  return *this; }

			smart_const_slist_iterator& operator++() 
			{
				m_node = node_traits::get_next(m_node);	return *this;
			}
			smart_const_slist_iterator operator++(int) 
			{
				smart_const_slist_iterator tmp(*this);
				m_node = node_traits::get_next(m_node);	return tmp;
			}
			friend bool operator==(const smart_const_slist_iterator& l, const smart_const_slist_iterator& r)
			{
				return l.m_node == r.m_node;
			}
			friend bool operator!=(const smart_const_slist_iterator& l, const smart_const_slist_iterator& r)
			{
				return !(l == r); 
			}

			reference operator*() const { return value_traits::to_value_reference(m_node); }
			pointer	  operator->() const {  return (&**this); }
			node*   get_node() const { return m_node; }

		private:
			node*	m_node;
		};
		
		
		
		
	}
}