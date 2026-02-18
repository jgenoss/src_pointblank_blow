#pragma once

//#include <xmemory>
//#include <xutility>
#include "../iterator.h"
#include "../functional.h"

#include "../pair.h"

#pragma warning(push)
#pragma warning(disable : 4512)

namespace i3
{
	namespace detail
	{
		template<class NodePtr>
		struct rbtree_insert_commit_data
		{
			rbtree_insert_commit_data() : m_node(), m_link_left(false) {}
			NodePtr			m_node;
			bool			m_link_left;
		};
	}

	template <class Value, class Alloc>
	class rbtree_value
	{
	protected:
		enum { rb_red, rb_black };
		struct node
		{	
			node(node* l, node* pa, node* r, const Value& v, char clr) : left(l), parent(pa), right(r), val(v), color(clr), is_nil(false){}
			node* left;			// left subtree, or smallest element if head
			node* parent;		// parent, or root of tree if head
			node* right;		// right subtree, or largest element if head
			Value val;			// the stored value, unused if head
			char  color;		// rb_red or rb_black, rb_black if head
			char  is_nil;		// true only if head (also nil) node		// 비어있거나, 최상단 노드인경우만...
		};

		static __forceinline node*  max_node(node* n) {	while (!n->right->is_nil) n = n->right;		return n; }
		static __forceinline node*	min_node(node* n) {	while (!n->left->is_nil) n = n->left;		return n; }

	public:	
		typedef				Value																value_type;
		typedef typename Alloc::template rebind<value_type>::other								allocator_type;	
		typedef typename allocator_type::difference_type										difference_type;
		typedef typename allocator_type::const_pointer											const_pointer;
		typedef typename allocator_type::const_reference										const_reference;

		class const_iterator : public i3::iterator< bidirectional_iterator_tag, value_type, difference_type, const_pointer, const_reference>
		{	
		public:

			typedef const_pointer pointer;
			typedef const_reference reference;

			const_iterator() : m_node(0) {}
			const_iterator(node* n)	: m_node(n) {}
			const_reference operator*() const {	return m_node->val; } 
			const_pointer operator->() const {	return (&**this);  } 
			const_iterator& operator++() {	inc();	return (*this);	}
			const_iterator operator++(int) { const_iterator _Tmp = *this;  ++*this; return (_Tmp);	}
			const_iterator& operator--() {	dec();	return (*this);	}
			const_iterator operator--(int) { const_iterator _Tmp = *this;  --*this;	return (_Tmp);	}
			bool operator==(const const_iterator& rhs) const {	return (m_node == rhs.m_node);	}
			bool operator!=(const const_iterator& rhs) const {	return (!(*this == rhs));	} 
			node* get_node() const {	return (m_node);	}

			void dec()
			{	// move to node with next smaller value
				if (m_node->is_nil)
					m_node = m_node->right;			// end() ==> rightmost
				else if (!m_node->left->is_nil)
					m_node = max_node(m_node->left);	// ==> largest of left subtree
				else
				{	// climb looking for left subtree
					node* n;
					while (! ( (n = m_node->parent)->is_nil ) && m_node == n->left)
						m_node = n;	// ==> parent while left subtree
					m_node = n;	// ==> parent if not head
				}
			}

			void inc()
			{	// move to node with next larger value
				if (! m_node->right->is_nil )
					m_node = min_node(m_node->right);	// ==> smallest of right subtree
				else
				{	// climb looking for right subtree
					node* n;
					while (! ( (n = m_node->parent)->is_nil ) && m_node == n->right )
						m_node = n;	// ==> parent while right subtree
					m_node = n;	// ==> parent (head if end())
				}
			}
		private:
			node* m_node;	
		};

		class iterator	: public const_iterator
		{	
		public:
			typedef typename allocator_type::pointer			pointer;
			typedef typename allocator_type::reference			reference;

			iterator(){} 
			iterator(node* n) : const_iterator(n){} 
			reference operator*() const { return ((reference)**(const_iterator *)this); } 
			pointer operator->() const { return (&**this);	} 
			iterator& operator++() {	++(*(const_iterator *)this);	return (*this);	}
			iterator operator++(int) {	iterator _Tmp = *this;	++*this;	return (_Tmp);	}
			iterator& operator--()	{	--(*(const_iterator *)this); return (*this);	}
			iterator operator--(int) {	iterator _Tmp = *this;	--*this; return (_Tmp);	} 
		};

		typedef i3::reverse_iterator<iterator> reverse_iterator;
		typedef i3::reverse_iterator<const_iterator> const_reverse_iterator;
	};
		
	
	template <class Value, class Pr, class Alloc>
	class rbtree_key : public rbtree_value<Value, Alloc>
	{
	public:
		typedef			Value					key_type;
		typedef			Pr						key_compare;
		typedef			Pr						value_compare;
	protected:
		static __forceinline const value_type&		key(node* n) { return n->val; }
		static __forceinline const value_type&		key(const value_type& val) { return val; }
	};
	
	template <class Key, class T, class Pr, class Alloc>
	class rbtree_key< pair<const Key, T>, Pr, Alloc > : public rbtree_value< pair<const Key, T>, Alloc >
	{
	public:
		typedef			Key						key_type;
		typedef			Pr						key_compare;
		class value_compare	: public i3::binary_function<value_type, value_type, bool>
		{	// functor for comparing two element values
		public:
			value_compare(const key_compare& pr) : m_comp(pr)	{}
			bool operator()(const value_type& lhs, const value_type& rhs) const {	return m_comp(lhs.first, rhs.first); }
		private:
			key_compare m_comp;	// the comparator predicate for keys
		};
	protected:
		static __forceinline const Key&		key(node* n) { return n->val.first; }
		static __forceinline const Key&		key(const value_type& val) { return val.first; }
	};
	
	
}

#pragma warning(pop)