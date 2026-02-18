#pragma once

#include "../compressed_pair.h"

#pragma pack(push, _CRT_PACKING)

#pragma push_macro("new")

#undef new

// internal debugging code to verify red-black properties of tree:
// 1) Every node is either red or black
// 2) Every leaf (NIL) is black
// 3) If a node is red, both its children are black
// 4) Every simple path from a node to a descendant leaf node contains 
//    the same number of black nodes

#include "rbtree_impl.h"

namespace i3 
{
	/////////////////////////

	template< class Value, class Pr, class Alloc >														
	class rbtree_base : public rbtree_key<Value, Pr, Alloc>
	{	// map/set , unique/multi, ordered tree..
	public:
		typedef rbtree_key<Value, Pr, Alloc>													base_type;
		typedef typename base_type::iterator													iterator;
		typedef typename base_type::const_iterator												const_iterator;
		typedef typename base_type::reverse_iterator											reverse_iterator;
		typedef typename base_type::const_reverse_iterator										const_reverse_iterator;



		typedef rbtree_base<  Value, Pr, Alloc >												my_type;
		typedef typename allocator_type::size_type												size_type;
		typedef typename allocator_type::template rebind<node>::other							node_allocator_type;

		typedef i3::pair<iterator, bool>						pair_ib;
		typedef i3::pair<iterator, iterator>					pair_ii;
		typedef i3::pair<const_iterator, const_iterator>		pair_cc;
		typedef i3::pair<node*, node*>							pair_nn;


		rbtree_base() : m_comp_and_size(0) { init_impl(); }
		explicit rbtree_base(const key_compare& pr) : m_comp_and_size(pr, 0) {	init_impl(); }
		explicit rbtree_base(const key_compare& pr, const allocator_type& a) : m_alloc_and_head(a), m_comp_and_size(pr, 0) { init_impl();	}
		rbtree_base(const my_type& rhs) :	m_alloc_and_head( rhs.node_alloc() ), m_comp_and_size(rhs.comp(), 0) {	init_impl();	copy_impl(rhs);	}
		~rbtree_base() {	erase_root_impl(root());	this->node_alloc().deallocate(head(), 1);	}

		my_type& operator=(const my_type& rhs)
		{	// replace contents from rhs
			if (this != &rhs)
			{	// worth doing
				erase_begin_end_impl();
				this->comp() = rhs.comp();
				copy_impl(rhs);
			}
			return *this;
		}

		iterator		begin() {	return (iterator(left_most()));	}
		const_iterator	begin() const {	return (const_iterator(left_most()));	}
		iterator		end()	{	return (iterator(head()));	}
		const_iterator	end() const	{	return (const_iterator(head()));  }
		reverse_iterator		rbegin() {	return (reverse_iterator(end()));	}
		const_reverse_iterator	rbegin() const	{	return (const_reverse_iterator(end()));	}
		reverse_iterator		rend()	{	return (reverse_iterator(begin()));	}
		const_reverse_iterator rend() const {	return (const_reverse_iterator(begin())); }
		size_type	size() const { return (size_ref());	}	// return length of sequence
		size_type	max_size() const { return ( node_alloc().max_size());  } // return maximum possible length of sequence
		bool		empty() const {	return (size() == 0);	} // return true only if sequence is empty
		allocator_type get_allocator() const {	return	node_alloc(); 	} // return allocator object for values
		key_compare key_comp() const {	return (this->comp()); }	// return object for comparing keys
		value_compare value_comp() const {	return (value_compare(key_comp()));	} // return object for comparing values


		iterator erase(const_iterator where)
		{	// erase element at where

			node* erased_node = where.get_node();	// node to erase
			++where;	// save successor iterator for return

			node* fix_node;	// the node to recolor as needed
		
			node* n = erased_node;

			if (n->left->is_nil)
				fix_node = n->right;	// must stitch up right subtree
			else if (n->right->is_nil)
				fix_node = n->left;	// must stitch up left subtree
			else
			{	// two subtrees, must lift successor node to replace erased
				n = where.get_node();	// n is successor node
				fix_node = n->right;	// fix_node is its only subtree
			}

			node* fix_node_parent;	// parent of fix_node (which may be nil)

			if (n == erased_node)
			{	// at most one subtree, relink it
				fix_node_parent = erased_node->parent;
				if (!fix_node->is_nil)
					fix_node->parent = fix_node_parent;	// link up

				if (root() == erased_node)
					root() = fix_node;	// link down from root
				else if (fix_node_parent->left == erased_node)
					fix_node_parent->left = fix_node;	// link down to left
				else
					fix_node_parent->right = fix_node;	// link down to right

				if (left_most() == erased_node)
					left_most() = fix_node->is_nil
					? fix_node_parent	// smallest is parent of erased node
					: min_node(fix_node);	// smallest in relinked subtree

				if (right_most() == erased_node)
					right_most() = fix_node->is_nil
					? fix_node_parent	// largest is parent of erased node
					: max_node(fix_node);	// largest in relinked subtree
			}
			else
			{	// erased has two subtrees, n is successor to erased
				erased_node->left->parent = n;	// parent(left(erased_node)) = n;	// link left up
				n->left = erased_node->left;	// link successor down

				if (n == erased_node->right)
					fix_node_parent = n;	// successor is next to erased
				else
				{	// successor further down, link in place of erased
					fix_node_parent = n->parent;	// parent is successor's
					if (!fix_node->is_nil)
						fix_node->parent = fix_node_parent;	// link fix up
					fix_node_parent->left = fix_node;	// link fix down
					n->right = erased_node->right;	// link successor down
					erased_node->right->parent = n;	// link right up
				}

				if (root() == erased_node)
					root() = n;	// link down from root
				else if (erased_node->parent->left  == erased_node)
					erased_node->parent->left = n;	// link down to left
				else
					erased_node->parent->right = n;	// link down to right

				n->parent = erased_node->parent;	// link successor up
				i3::swap(n->color, erased_node->color);	// recolor it
			}

			if ( erased_node->color == rb_black)
			{	// erasing black link, must recolor/rebalance tree
				for (; fix_node != root() && fix_node->color == rb_black; fix_node_parent = fix_node->parent)
					if (fix_node == fix_node_parent->left )
					{	// fixup left subtree
						n = fix_node_parent->right;
						if ( n->color == rb_red)
						{	// rotate red up from right subtree
							n->color = rb_black;
							fix_node_parent->color = rb_red;
							l_rotate(fix_node_parent);
							n = fix_node_parent->right;
						}

						if (n->is_nil)
							fix_node = fix_node_parent;	// shouldn't happen
						else if ( n->left->color == rb_black && n->right->color == rb_black)
						{	// redden right subtree with black children
							n->color = rb_red;
							fix_node = fix_node_parent;
						}
						else
						{	// must rearrange right subtree
							if ( n->right->color == rb_black)
							{	// rotate red up from left sub-subtree
								n->left->color = rb_black;
								n->color = rb_red;
								r_rotate(n);
								n = fix_node_parent->right;
							}

							n->color = fix_node_parent->color;
							fix_node_parent->color = rb_black;
							n->right->color = rb_black;
							l_rotate(fix_node_parent);
							break;	// tree now recolored/rebalanced
						}
					}
					else
					{	// fixup right subtree
						n = fix_node_parent->left;
						if (n->color == rb_red)
						{	// rotate red up from left subtree
							n->color = rb_black;
							fix_node_parent->color = rb_red;
							r_rotate(fix_node_parent);
							n = fix_node_parent->left;
						}
						if (n->is_nil)
							fix_node = fix_node_parent;	// shouldn't happen
						else if (n->right->color == rb_black && n->left->color == rb_black)
						{	// redden left subtree with black children
							n->color = rb_red;
							fix_node = fix_node_parent;
						}
						else
						{	// must rearrange left subtree
							if ( n->left->color == rb_black)
							{	// rotate red up from right sub-subtree
								n->right->color = rb_black;
								n->color = rb_red;
								l_rotate(n);
								n = fix_node_parent->left;
							}

							n->color = fix_node_parent->color;
							fix_node_parent->color = rb_black;
							n->left->color = rb_black;
							r_rotate(fix_node_parent);
							break;	// tree now recolored/rebalanced
						}
					}

					fix_node->color = rb_black;	// ensure stopping node is black
			}

			this->node_alloc().destroy(erased_node);	// destroy, free erased node
			this->node_alloc().deallocate(erased_node, 1);

			--size_ref();

			return make_iter(where);	// return successor iterator
		}

		iterator erase(const_iterator f, const_iterator l)
		{	// erase [f, l)
			if (f == begin() && l == end())
			{	// erase all
				clear();
				return begin();
			}
			else
			{	// partial erase, one at a time
				while (f != l)
					erase(f++);
				return make_iter(f);
			}
		}


		void clear()
		{	// erase all
			erase_root_impl(root());
			root() = head(), size_ref() = 0;
			left_most() = head(), right_most() = head();
		}

		iterator		find(const key_type& k) {  return iterator( find_impl(k) );  }
		const_iterator	find(const key_type& k) const { return const_iterator( find_impl(k) ); }

		template<class OtherKey>
		iterator		find_other(const OtherKey& k) {  return iterator( find_impl(k) );  }
		template<class OtherKey>
		const_iterator	find_other(const OtherKey& k) const { return const_iterator( find_impl(k) ); }

		template<class OtherKey, class AnyComp>
		iterator		find_other(const OtherKey& k, AnyComp any_comp) {  return iterator( find_impl(k, any_comp) );  }
		template<class OtherKey, class AnyComp>
		const_iterator	find_other(const OtherKey& k, AnyComp any_comp) const { return const_iterator( find_impl(k, any_comp) ); }
		

		iterator		lower_bound(const key_type& key_val) {	return iterator(lower_bound_impl(key_val));  }
		const_iterator	lower_bound(const key_type& key_val) const {	return const_iterator(lower_bound_impl(key_val));	}
		iterator		upper_bound(const key_type& key_val)	{	return iterator(upper_bound_impl(key_val));	}
		const_iterator	upper_bound(const key_type& key_val) const	{	return const_iterator(upper_bound_impl(key_val));	}
	
		template<class OtherKey>
		iterator		lower_bound_other(const OtherKey& key_val) {	return iterator(lower_bound_impl(key_val));  }
		template<class OtherKey>
		const_iterator	lower_bound_other(const OtherKey& key_val) const {	return const_iterator(lower_bound_impl(key_val));	}
		template<class OtherKey>
		iterator		upper_bound_other(const OtherKey& key_val)	{	return iterator(upper_bound_impl(key_val));	}
		template<class OtherKey>
		const_iterator	upper_bound_other(const OtherKey& key_val) const	{	return const_iterator(upper_bound_impl(key_val));	}

		template<class OtherKey, class AnyComp>
		iterator		lower_bound_other(const OtherKey& key_val, AnyComp any_comp) {	return iterator(lower_bound_impl(key_val, any_comp));  }
		template<class OtherKey, class AnyComp>
		const_iterator	lower_bound_other(const OtherKey& key_val, AnyComp any_comp) const {	return const_iterator(lower_bound_impl(key_val, any_comp));	}
		template<class OtherKey, class AnyComp>
		iterator		upper_bound_other(const OtherKey& key_val, AnyComp any_comp)	{	return iterator(upper_bound_impl(key_val, any_comp));	}
		template<class OtherKey, class AnyComp>
		const_iterator	upper_bound_other(const OtherKey& key_val, AnyComp any_comp) const	{	return const_iterator(upper_bound_impl(key_val, any_comp));	}

		void swap(my_type& rhs)
		{	// exchange contents with rhs
			if (this == &rhs)
				;
			else if (node_alloc() == rhs.node_alloc())
			{	// same allocator, swap control information

				i3::swap(this->comp(), rhs.comp());
				i3::swap(head(), rhs.head());
				i3::swap(size_ref(), rhs.size_ref());
			}
			else
			{	// different allocator, do multiple assigns
				my_type _Tmp = *this; *this = rhs, rhs = _Tmp;
			}
		}

	protected:
		
		node*& left_most() const {	return head()->left;   }// return leftmost node in nonmutable tree
		node*& right_most() const {	return head()->right;  }
		node*& root() const  {	return head()->parent; }

		void		erase_begin_end_impl() { clear(); }

		iterator make_iter(const_iterator where) const {	return iterator(where.get_node() );	}

		void copy_impl(const my_type& rhs)
		{	// copy entire tree from rhs
			root() = copy_impl(rhs.root(), head());
			size_ref() = rhs.size();
			if (!root()->is_nil )
			{	// nonempty tree, look for new smallest and largest
				left_most() = min_node(root());
				right_most() = max_node(root());
			}
			else
				left_most() = head(), right_most() = head();	// empty tree
		}

		node* copy_impl(node* root_node, node* where_node)
		{	// copy entire subtree, recursively
			node* new_root = head();	// point at nil node

			if (!root_node->is_nil)
			{	// copy a node, then any subtrees
				node* n = buy_node(head(), where_node, head(), root_node->val, root_node->color);
				if ( new_root->is_nil )
					new_root = n;	// memorize new root

				n->left  = copy_impl(root_node->left, n);
				n->right = copy_impl(root_node->right, n);
			}

			return new_root;	// return newly constructed tree
		}

		template<class AnyKeyType>
		node* find_impl(const AnyKeyType& k) const
		{
			node* n = lower_bound_impl(k);
			return n == head() || comp()( k, key(n) ) ? head() : n;
		}
		template<class AnyKeyType, class AnyComp>
		node* find_impl(const AnyKeyType& k, AnyComp any_comp) const
		{
			node* n = lower_bound_impl(k, any_comp);
			return n == head() || any_comp( k, key(n) ) ? head() : n;			
		}


		template<class AnyKeyType>
		node* lower_bound_impl(const AnyKeyType& k) const
		{	// find leftmost node not less than key_val
			node* n = root();
			node* where_node = head();	// end() if search fails

			while (!n->is_nil)
				if ( this->comp()( key(n), k) )
					n = n->right;	// descend right subtree
				else
				{	// n not less than key_val, remember it
					where_node = n;
					n = n->left;	// descend left subtree
				}

			return where_node;	// return best remembered candidate
		}
		
		template<class AnyKeyType, class AnyComp>
		node* lower_bound_impl(const AnyKeyType& k, AnyComp any_comp) const
		{	// find leftmost node not less than key_val
			node* n = root();
			node* where_node = head();	// end() if search fails

			while (!n->is_nil)
				if ( any_comp( key(n), k) )
					n = n->right;	// descend right subtree
				else
				{	// n not less than key_val, remember it
					where_node = n;
					n = n->left;	// descend left subtree
				}

			return where_node;	// return best remembered candidate
		}

		template<class AnyKeyType>
		node* upper_bound_impl(const AnyKeyType& k) const
		{	// find leftmost node greater than key_val
			node* n = root();
			node* where_node = head();	// end() if search fails

			while (!n->is_nil)
				if (this->comp()(k, key(n) ) )
				{	// n greater than key_val, remember it
					where_node = n;
					n = n->left;	// descend left subtree
				}
				else
					n = n->right;	// descend right subtree

			return where_node;	// return best remembered candidate
		}

		template<class AnyKeyType, class AnyComp>
		node* upper_bound_impl(const AnyKeyType& k, AnyComp any_comp) const
		{	// find leftmost node greater than key_val
			node* n = root();
			node* where_node = head();	// end() if search fails

			while (!n->is_nil)
				if (any_comp(k, key(n) ) )
				{	// n greater than key_val, remember it
					where_node = n;
					n = n->left;	// descend left subtree
				}
				else
					n = n->right;	// descend right subtree

			return where_node;	// return best remembered candidate
		}

		void erase_root_impl(node* root_node)
		{	// free entire subtree, recursively
			for (node* n = root_node; !n->is_nil; root_node = n)
			{	// free subtrees, then node
				erase_root_impl(n->right);
				n = n->left;
				this->node_alloc().destroy(root_node);	// destroy, free erased node
				this->node_alloc().deallocate(root_node, 1);
			}
		}

		void init_impl()
		{	// create head/nil node and make tree empty
			head() = this->node_alloc().allocate(1);
			left_most() = head();
			root()  = head();
			right_most() = head();
			head()->color = rb_black;
			head()->is_nil = true;
		}

		iterator insert_impl(bool add_left, node* where_node, const value_type& val)
		{	// add node with value next to where_node, to left if _Addnode
			//			if (max_size() - 1 <= size_ref())
			//				_THROW(length_error, "map/set<T> too long");

			node* new_node = buy_node(head(), where_node, head(), val, rb_red);

			++size_ref();
			if (where_node == head())
			{	// first node in tree, just set head values
				root() = new_node;
				left_most() = new_node, right_most() = new_node;
			}
			else if (add_left)
			{	// add to left of where_node
				where_node->left = new_node;
				if (where_node == left_most())
					left_most() = new_node;
			}
			else
			{	// add to right of where_node
				where_node->right = new_node;
				if (where_node == right_most())
					right_most() = new_node;
			}

			for (node* n = new_node; n->parent->color == rb_red; )
				if (n->parent == n->parent->parent->left )
				{	// fixup red-red in left subtree
					where_node = n->parent->parent->right;
					if (where_node->color == rb_red)
					{	// parent has two red children, blacken both
						n->parent->color = rb_black;
						where_node->color = rb_black;
						n->parent->parent->color = rb_red;
						n = n->parent->parent;
					}
					else
					{	// parent has red and black children
						if (n == n->parent->right)
						{	// rotate right child to left
							n = n->parent;
							l_rotate(n);
						}
						n->parent->color = rb_black;	// propagate red up
						n->parent->parent->color = rb_red;
						r_rotate(n->parent->parent);
					}
				}
				else
				{	// fixup red-red in right subtree
					where_node = n->parent->parent->left;
					if ( where_node->color == rb_red)
					{	// parent has two red children, blacken both
						n->parent->color = rb_black;
						where_node->color = rb_black;
						n->parent->parent->color = rb_red;
						n = n->parent->parent;
					}
					else
					{	// parent has red and black children
						if (n == n->parent->left )
						{	// rotate left child to right
							n = n->parent;
							r_rotate(n);
						}
						n->parent->color = rb_black;	// propagate red up
						n->parent->parent->color = rb_red;
						l_rotate(n->parent->parent);
					}
				}

				root()->color = rb_black;	// root is always black
				return iterator(new_node);
		}


		void l_rotate(node* where_node)
		{	// promote right node to root of subtree
			node* n = where_node->right;
			where_node->right = n->left;

			if (!n->left->is_nil)
				n->left->parent = where_node;
			n->parent = where_node->parent;

			if (where_node == root())
				root() = n;
			else if (where_node == where_node->parent->left)
				where_node->parent->left = n;
			else
				where_node->parent->right = n;

			n->left = where_node;
			where_node->parent = n;
		}
		void r_rotate(node* where_node)
		{	// promote left node to root of subtree
			node* n = where_node->left;
			where_node->left = n->right;

			if (!n->right->is_nil)
				n->right->parent = where_node;
			n->parent = where_node->parent;

			if (where_node == root())
				root() = n;
			else if (where_node == where_node->parent->right)
				where_node->parent->right = n;
			else
				where_node->parent->left = n;

			n->right = where_node;
			where_node->parent = n;
		}

		node* buy_node(node* l, node* pa, node* r, const value_type& val, char clr)
		{	// allocate a node with pointers, value, and color
			node* where_node = this->node_alloc().allocate(1);
			return new (where_node) node(l, pa, r, val, clr);
		}


//      EBO최적화 --> 12 -> 8바이트 사이즈.. (64비트기준으로는 24 -> 16바이트로)
		compressed_pair<node_allocator_type, node*>				m_alloc_and_head;
		compressed_pair<key_compare, size_type>					m_comp_and_size;
		
		__forceinline const node_allocator_type&		node_alloc() const		{ return m_alloc_and_head.first(); }
		__forceinline node_allocator_type&				node_alloc()			{ return m_alloc_and_head.first(); }
		__forceinline node* const &						head() const			{ return m_alloc_and_head.second(); }
		__forceinline node*&							head()					{ return m_alloc_and_head.second(); }
		
		__forceinline const key_compare&				comp() const			{ return m_comp_and_size.first(); }
		__forceinline key_compare&						comp()					{ return m_comp_and_size.first(); }
		__forceinline const size_t&						size_ref() const		{ return m_comp_and_size.second(); }
		__forceinline size_t&							size_ref()				{ return m_comp_and_size.second(); }

//		key_compare												comp;	// the comparator predicate for keys
//		node_allocator_type										_Alnod;	// allocator object for nodes
//		node*													_Myhead;	// pointer to head node
//		size_type												_Mysize;	// number of elements
	};


	template<class Value, class Pr, class Alloc>
	class rbtree : public rbtree_base< Value, Pr, Alloc>
	{
	public:			

		typedef rbtree_base<Value, Pr, Alloc>		base_type;
		
		typedef detail::rbtree_insert_commit_data<node*>	insert_commit_data;

		enum { is_multi = false };	
		
		rbtree() {}
		explicit rbtree(const key_compare& pr) : rbtree_base(pr) {}
		explicit rbtree(const key_compare& pr, const allocator_type& a) : rbtree_base(pr, a){}

		rbtree(const value_type* f, const value_type* l, const key_compare& pr, const allocator_type& a)	: rbtree_base(pr, a)	{	insert(f, l);	}
		template<class Iter> rbtree(Iter f, Iter l) { insert(f, l); }
		template<class Iter> rbtree(Iter f, Iter l, const key_compare& pr) : rbtree_base(pr) { insert(f, l); }
		template<class Iter> rbtree(Iter f, Iter l, const key_compare& pr, const allocator_type& a) : rbtree_base(pr, a) { insert(f, l); }
		

		rbtree(const my_type& rhs) : rbtree_base(rhs) { }

		template<class Iter>
		void insert(Iter f, Iter l)	{	for (; f != l; ++f) insert(*f);	}

		pair_ib insert_check(const key_type& k, insert_commit_data& out_data)
		{
			return insert_check_other(k, comp(), out_data);
		}
		template<class AnyKeyType>
		pair_ib insert_check_other(const AnyKeyType& k, insert_commit_data& out_data)
		{
			return insert_check_other(k, comp(), out_data);
		}

		template<class AnyKeyType, class AnyComp>
		pair_ib insert_check_other(const AnyKeyType& k, AnyComp any_comp, insert_commit_data& out_data)
		{
			node* try_node = root();
			node* where_node = head();
			bool add_left = true;	// add to left of head if tree empty
			while (!try_node->is_nil)
			{	// look for leaf to insert before (add_left) or after
				where_node = try_node;
				add_left = any_comp( k, key(try_node) );

				try_node = add_left ? try_node->left : try_node->right;
			}
			
			out_data.m_node = where_node;	out_data.m_link_left = add_left;

			iterator where = iterator(where_node);
			if (!add_left)
				;	// need to test if insert after is okay
			else if (where == begin())
			{
				return pair_ib(end(), true);
			}
			else
				--where;

			if ( any_comp(	key(where.get_node()), k ) )
			{
				return pair_ib(end(), true);
			}
			
			return (pair_ib(where, false));		// 키값이 일치...
		}

		iterator insert_commit(const value_type& val, const insert_commit_data& in_data)
		{
			return insert_impl(in_data.m_link_left, in_data.m_node, val);
		}

		pair_ib insert(const value_type& val)
		{	// try to insert node with value val
			node* try_node = root();
			node* where_node = head();
			bool add_left = true;	// add to left of head if tree empty
			while (!try_node->is_nil)
			{	// look for leaf to insert before (add_left) or after
				where_node = try_node;
				add_left = this->comp()( key(val), key(try_node) );

				try_node = add_left ? try_node->left : try_node->right;
			}

			// insert only if unique
			iterator where = iterator(where_node);
			if (!add_left)
				;	// need to test if insert after is okay
			else if (where == begin())
				return (pair_ib(insert_impl(true, where_node, val), true));
			else
				--where;	// need to test if insert before is okay

			if (this->comp()(	key(where.get_node()), key(val) ) )
				return (pair_ib(insert_impl(add_left, where_node, val), true));
			else
				return (pair_ib(where, false));		// 키값이 일치...
		}

		iterator insert(const_iterator where, const value_type& val) // try to insert node with value val using where as a hint
		{	
			const_iterator next_iter;

			if (size() == 0)
				return (insert_impl(true, head(), val));	// insert into empty tree
			else
			{	// insert only if unique
				if (where == begin()) // insert at beginning if before first element
				{	
					if ( this->comp()( key(val), key(where.get_node())  ) )
						return (insert_impl(true, where.get_node(), val));
				}
				else if (where == end() )
				{	// insert at end if after last element
					if ( this->comp()(key(right_most()) , key(val) ) )
						return (insert_impl(false, right_most(), val));
				}
				else if ( this->comp()( key(val), key(where.get_node())  )
					&&  this->comp()( key( (--(next_iter = where)).get_node() ), key(val) ) )
				{	// insert before where
					if ( next_iter.get_node()->right->is_nil )
						return (insert_impl(false, next_iter.get_node(), val));
					else
						return (insert_impl(true, where.get_node(), val));
				}
				else if ( this->comp()( key(where.get_node()), key(val) )
					&& (++(next_iter = where) == end()	|| this->comp()( key(val), key(next_iter.get_node())  ) ) )
				{	// insert after where
					if ( where.get_node()->right->is_nil )
						return (insert_impl(false, where.get_node(), val));
					else
						return (insert_impl(true, next_iter.get_node(), val));
				}
			}

			return insert(val).first;	// try usual insert if all else fails
		}
		
		using base_type::erase;

		template<class AnyKeyType>
		size_type erase_other(const AnyKeyType& k)
		{
			iterator i( find_other(k));
			if (i == end()) return 0;
			erase(i);
			return 1;
		}
		
		template<class AnyKeyType, class AnyComp>
		size_type erase_other(const AnyKeyType& k, AnyComp any_comp)
		{
			iterator i( find_other(k, any_comp));
			if (i == end()) return 0;
			erase(i);
			return 1;
		}

		size_type erase(const key_type& k)			// lower_bound로 최적화된 버전..
		{	// erase and count all that match key_val
			iterator i( find(k) );
			if ( i == end() ) return 0;
			erase(i);
			return  1;
		}

		void		erase(const key_type* f, const key_type* l) {	while (f != l) erase(*f++);	}	// erase all that match array of keys [f, l)
		size_type	count(const key_type& k) const {	return find_impl(k) != head(); }
		template<class OtherKey>
		size_type	count_other(const OtherKey& k) const {	return find_impl(k) != head(); }
		
		template<class OtherKey, class AnyComp>
		size_type	count_other(const OtherKey& k, AnyComp any_comp) const {	return find_impl(k, any_comp) != head(); }


		pair_ii equal_range(const key_type& key_val){ return pair_ii( equal_range_impl(key_val) );	} //	return (pair_ii(lower_bound(key_val), upper_bound(key_val)));
		pair_cc equal_range(const key_type& key_val) const { return pair_cc( equal_range_impl(key_val) ); } //	return (pair_cc(lower_bound(key_val), upper_bound(key_val)));

		template<class OtherKey>
		pair_ii equal_range_other(const OtherKey& key_val){ return pair_ii( equal_range_impl(key_val) );	} //	return (pair_ii(lower_bound(key_val), upper_bound(key_val)));
		template<class OtherKey>
		pair_cc equal_range_other(const OtherKey& key_val) const { return pair_cc( equal_range_impl(key_val) ); } //	return (pair_cc(lower_bound(key_val), upper_bound(key_val)));

		template<class OtherKey, class AnyComp>
		pair_ii equal_range_other(const OtherKey& key_val, AnyComp any_comp){ return pair_ii( equal_range_impl(key_val, any_comp) );	} //	return (pair_ii(lower_bound(key_val), upper_bound(key_val)));
		template<class OtherKey, class AnyComp>
		pair_cc equal_range_other(const OtherKey& key_val, AnyComp any_comp) const { return pair_cc( equal_range_impl(key_val, any_comp) ); } //	return (pair_cc(lower_bound(key_val), upper_bound(key_val)));

	private:

		template<class AnyKeyType>
		pair_nn equal_range_impl(const AnyKeyType& k) const
		{	// find leftmost node not less than key_val
			node* n1 = lower_bound_impl(k);
			node* n2 = n1;		
			if ( n1 != head() && !comp()( k, key(n1)))
			{
					// 아래는 반복자 하나증가하는 함수에서 발췌..
				if (!n2->right->is_nil )
					n2 = min_node(n2->right);	// ==> smallest of right subtree
				else
				{	// climb looking for right subtree
					node* n;
					while (! ((n = n2->parent )->is_nil )	&& n2 == n->right ) n2 = n;	// ==> parent while right subtree
						n2 = n;																	// ==> parent (head if end())
				}
			}
			
			return pair_nn(n1, n2);
		}

		template<class AnyKeyType, class AnyComp>
		pair_nn equal_range_impl(const AnyKeyType& k, AnyComp any_comp) const
		{	// find leftmost node not less than key_val
			node* n1 = lower_bound_impl(k, any_comp);
			node* n2 = n1;		
			if ( n1 != head() && !any_comp( k, key(n1)))
			{
				// 아래는 반복자 하나증가하는 함수에서 발췌..
				if (!n2->right->is_nil )
					n2 = min_node(n2->right);	// ==> smallest of right subtree
				else
				{	// climb looking for right subtree
					node* n;
					while (! ((n = n2->parent )->is_nil )	&& n2 == n->right ) n2 = n;	// ==> parent while right subtree
					n2 = n;																	// ==> parent (head if end())
				}
			}

			return pair_nn(n1, n2);
		}


	};

	template<class Value, class Pr, class Alloc>
	class multi_rbtree : public rbtree_base<Value, Pr, Alloc>
	{
	public:
		typedef rbtree_base<Value, Pr, Alloc>		base_type;

		enum { is_multi = true };	

		multi_rbtree() {}
		explicit multi_rbtree(const key_compare& pr) : rbtree_base(pr) {}
		explicit multi_rbtree(const key_compare& pr, const allocator_type& a) : rbtree_base(pr, a){}

		multi_rbtree(const value_type* f, const value_type* l, const key_compare& pr, const allocator_type& a) : rbtree_base(pr, a)	{	insert(f, l);	}
		template<class Iter> multi_rbtree(Iter f, Iter l) { insert(f, l); }
		template<class Iter> multi_rbtree(Iter f, Iter l, const key_compare& pr) : rbtree_base(pr) { insert(f, l); }
		template<class Iter> multi_rbtree(Iter f, Iter l, const key_compare& pr, const allocator_type& a) : rbtree_base(pr, a) { insert(f, l); }

		multi_rbtree(const my_type& rhs) : rbtree_base(rhs){}

		template<class Iter>
		void insert(Iter f, Iter l) {	for (; f != l; ++f)	insert(*f);	}

		iterator insert(const value_type& val)
		{	// try to insert node with value val
			node* try_node = root();
			node* where_node = head();
			bool add_left = true;	// add to left of head if tree empty
			while (!try_node->is_nil)
			{	// look for leaf to insert before (add_left) or after
				where_node = try_node;
				add_left = this->comp()( key(val), key(try_node) );

				try_node = add_left ? try_node->left : try_node->right;
			}

			return insert_impl(add_left, where_node, val);
		}

		iterator insert(const_iterator where, const value_type& val) // try to insert node with value val using where as a hint
		{	
			const_iterator next_iter;

			if (size() == 0)
				return (insert_impl(true, head(), val));	// insert into empty tree
			else 
			{	
				// insert even if duplicate
				if (where == begin())
				{	// insert at beginning if before first element
					if (! this->comp()(  key(where.get_node()), key(val)  ) )
						return (insert_impl(true, where.get_node(), val));
				}
				else 
					if (where == end())
					{	// insert at end if after last element
						if (! this->comp()( key(val), key(right_most())  ) )
							return (insert_impl(false, right_most(), val));
					}
					else 
						if (! this->comp()( key(where.get_node()), key(val) )
							&& ! this->comp()( key(val), key( (--(next_iter = where)).get_node())   ) )
						{	// insert before where
							if (next_iter.get_node()->right->is_nil)
								return (insert_impl(false, next_iter.get_node(), val));
							else
								return (insert_impl(true, where.get_node(), val));
						}
						else if (! this->comp()( key(val), key(where.get_node())  )
							&& (++(next_iter = where) == end() || !this->comp()( key(next_iter.get_node()) , key(val) ) ) )
						{	// insert after where
							if ( where.get_node()->right->is_nil)
								return (insert_impl(false, where.get_node(), val));
							else
								return (insert_impl(true, next_iter.get_node(), val));
						}
			}

			return insert(val);	// try usual insert if all else fails
		}

		using base_type::erase;

		template<class AnyKeyType>
		size_type erase_other(const AnyKeyType& k)
		{
			pair_ii where = equal_range_other(k);
			size_type num = static_cast<size_type>(i3::distance( where.first, where.second ));
			erase(where.first, where.second);
			return num;
		}
		
		template<class AnyKeyType, class AnyComp>
		size_type erase_other(const AnyKeyType& k, AnyComp any_comp)
		{
			pair_ii where = equal_range_other(k, any_comp);
			size_type num = static_cast<size_type>(i3::distance( where.first, where.second ));
			erase(where.first, where.second);
			return num;
		}

		size_type erase(const key_type& k)
		{	// erase and count all that match key_val
			pair_ii where = equal_range(k);
			size_type num = static_cast<size_type>(i3::distance( where.first, where.second ));
			erase(where.first, where.second);
			return num;
		}
		void erase(const key_type* f, const key_type* l) {	while (f != l) erase(*f++);	}	

		size_type count(const key_type& k) const 
		{		
			pair_cc res = equal_range(k);
			return	static_cast<size_type>( i3::distance(res.first, res.second) );	
		}
		template<class OtherKey>
		size_type count_other(const OtherKey& k) const 
		{		
			pair_cc res = equal_range_other(k);
			return	static_cast<size_type>( i3::distance(res.first, res.second) );	
		}

		template<class OtherKey, class AnyComp>
		size_type count_other(const OtherKey& k, AnyComp any_comp) const 
		{		
			pair_cc res = equal_range_other(k, any_comp);
			return	static_cast<size_type>( i3::distance(res.first, res.second) );	
		}		

		pair_ii equal_range(const key_type& key_val){ return pair_ii( equal_range_impl(key_val) );	} //	return (pair_ii(lower_bound(key_val), upper_bound(key_val)));
		pair_cc equal_range(const key_type& key_val) const { return pair_cc( equal_range_impl(key_val) ); } //	return (pair_cc(lower_bound(key_val), upper_bound(key_val)));

		template<class OtherKey>
		pair_ii equal_range_other(const key_type& key_val){ return pair_ii( equal_range_impl(key_val) );	} //	return (pair_ii(lower_bound(key_val), upper_bound(key_val)));

		template<class OtherKey>
		pair_cc equal_range_other(const key_type& key_val) const { return pair_cc( equal_range_impl(key_val) ); } //	return (pair_cc(lower_bound(key_val), upper_bound(key_val)));

		template<class OtherKey, class AnyComp>
		pair_ii equal_range_other(const key_type& key_val, AnyComp any_comp){ return pair_ii( equal_range_impl(key_val, any_comp) );	} //	return (pair_ii(lower_bound(key_val), upper_bound(key_val)));

		template<class OtherKey, class AnyComp>
		pair_cc equal_range_other(const key_type& key_val, AnyComp any_comp) const { return pair_cc( equal_range_impl(key_val, any_comp) ); } //	return (pair_cc(lower_bound(key_val), upper_bound(key_val)));


	private:
		
		template<class AnyKeyType>
		pair_nn equal_range_impl(const AnyKeyType& k) const
		{	// find leftmost node not less than key_val
			node* n = root();
			node* low_node = head();	// end() if search fails
			node* high_node = head();	// end() if search fails

			while (!n->is_nil)
			{
				if ( (this->comp()( key(n), k)))
					n = n->right;	// descend right subtree
				else
				{	// n not less than key_val, remember it
					if (high_node->is_nil	&& this->comp()( k, key(n) ))
						high_node = n;	// n greater, remember it
					low_node = n;
					n = n->left;	// descend left subtree
				}
			}

			n = high_node->is_nil ? root() : high_node->left;	// continue scan for upper bound
			while (!n->is_nil )
			{
				if ( this->comp()( k, key(n) ))
				{	// n greater than key_val, remember it
					high_node = n;
					n = n->left;	// descend left subtree
				}
				else
					n = n->right;	// descend right subtree
			}

			return pair_nn(low_node, high_node );
		}

		template<class AnyKeyType, class AnyComp>
		pair_nn equal_range_impl(const AnyKeyType& k, AnyComp any_comp) const
		{	// find leftmost node not less than key_val
			node* n = root();
			node* low_node = head();	// end() if search fails
			node* high_node = head();	// end() if search fails

			while (!n->is_nil)
			{
				if ( (any_comp( key(n), k)))
					n = n->right;	// descend right subtree
				else
				{	// n not less than key_val, remember it
					if (high_node->is_nil	&& any_comp( k, key(n) ))
						high_node = n;	// n greater, remember it
					low_node = n;
					n = n->left;	// descend left subtree
				}
			}

			n = high_node->is_nil ? root() : high_node->left;	// continue scan for upper bound
			while (!n->is_nil )
			{
				if ( any_comp( k, key(n) ))
				{	// n greater than key_val, remember it
					high_node = n;
					n = n->left;	// descend left subtree
				}
				else
					n = n->right;	// descend right subtree
			}

			return pair_nn(low_node, high_node );
		}


	};


	
}

#include "rbtree_operator.h"

#pragma pop_macro("new")

#pragma pack(pop)
