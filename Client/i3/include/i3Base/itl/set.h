#pragma once

#include "set_fwd.h"

#pragma pack(push, _CRT_PACKING)
#include "detail/rbtree.h"

namespace i3
{

	template<class Key, class Pr, class Alloc>
	class set : public rbtree< Key, Pr, Alloc >
	{	
	public:
		typedef set<Key, Pr, Alloc>					my_type;
		typedef rbtree< Key, Pr, Alloc >			base_type;

		typedef typename base_type::key_type		key_type;
		typedef typename base_type::value_type		value_type;
		typedef typename base_type::key_compare		key_compare;
		typedef typename base_type::value_compare	value_compare;
		typedef typename base_type::allocator_type	allocator_type;
		typedef typename allocator_type::reference	reference;
		typedef typename base_type::const_reference const_reference;
		typedef typename base_type::iterator		iterator;
		typedef typename base_type::const_iterator	const_iterator;
		typedef typename base_type::size_type		size_type;
		typedef typename base_type::difference_type difference_type;
		typedef typename allocator_type::pointer	pointer;
		typedef typename base_type::const_pointer	const_pointer;
		typedef typename base_type::reverse_iterator reverse_iterator;
		typedef typename base_type::const_reverse_iterator 	const_reverse_iterator;
	
		typedef typename base_type::insert_commit_data		insert_commit_data;

		set() {}
		explicit set(const key_compare& pr) : base_type(pr)	{}
		explicit set(const key_compare& pr, const allocator_type& a) : base_type(pr, a) {}

		template<class Iter> set(Iter f, Iter l) : base_type(f, l) {}
		template<class Iter> set(Iter f, Iter l, const key_compare& pr) : base_type(f, l, pr) {} 
		template<class Iter> set(Iter f, Iter l, const key_compare& pr, const allocator_type& a) : base_type(f, l, pr, a) {}

	};


	template<class Key, class Pr, class Alloc> 
	inline void swap(set<Key, Pr, Alloc>& lhs, set<Key, Pr, Alloc>& rhs){	lhs.swap(rhs); }

	
	template<class Key, class Pr, class Alloc>
	class multiset 	: public multi_rbtree< Key, Pr, Alloc >
	{	
	public:
		typedef multiset<Key, Pr, Alloc>			my_type;
		typedef multi_rbtree< Key, Pr, Alloc >		base_type;

		typedef typename base_type::key_type		key_type;
		typedef typename base_type::value_type		value_type;
		typedef typename base_type::key_compare		key_compare;
		typedef typename base_type::value_compare	value_compare;
		typedef typename base_type::allocator_type	allocator_type;
		typedef typename allocator_type::reference	reference;
		typedef typename base_type::const_reference const_reference;
		typedef typename base_type::iterator		iterator;
		typedef typename base_type::const_iterator	const_iterator;
		typedef typename base_type::size_type		size_type;
		typedef typename base_type::difference_type difference_type;
		typedef typename allocator_type::pointer	pointer;
		typedef typename base_type::const_pointer	const_pointer;
		typedef typename base_type::reverse_iterator reverse_iterator;
		typedef typename base_type::const_reverse_iterator 	const_reverse_iterator;
	

		multiset() { }
		explicit multiset(const key_compare& pr)	: base_type(pr) {}
		explicit multiset(const key_compare& pr, const allocator_type& a) : base_type(pr, a) {}

		template<class Iter> multiset(Iter f, Iter l) : base_type(f, l) {}
		template<class Iter> multiset(Iter f, Iter l, const key_compare& pr) : base_type(f, l, pr) {} 
		template<class Iter> multiset(Iter f, Iter l, const key_compare& pr, const allocator_type& a) : base_type(f, l, pr, a) {}

	};

	template<class Key, class Pr, class Alloc> inline
	void swap(multiset<Key, Pr, Alloc>& lhs, multiset<Key, Pr, Alloc>& rhs) {	lhs.swap(rhs); }



}

#pragma pack(pop)