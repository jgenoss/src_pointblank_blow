#pragma once

#pragma pack(push, _CRT_PACKING)

#include "detail/sorted_vector.h"

namespace i3
{
	
	template<class Key, class Pr = less<Key>, class Alloc = i3::pool_allocator<Key> >
	class vector_set : public sorted_vector< Key, Pr, Alloc >
	{	
	public:
		typedef vector_set<Key, Pr, Alloc>				my_type;
		typedef sorted_vector< Key, Pr, Alloc >			base_type;

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


		vector_set() {}
		explicit vector_set(const key_compare& pr) : base_type(pr)	{}
		explicit vector_set(const key_compare& pr, const allocator_type& a) : base_type(pr, a) {}

		template<class Iter> vector_set(Iter f, Iter l) : base_type(f, l) {}
		template<class Iter> vector_set(Iter f, Iter l, const key_compare& pr) : base_type(f, l, pr) {} 
		template<class Iter> vector_set(Iter f, Iter l, const key_compare& pr, const allocator_type& a) : base_type(f, l, pr, a) {}

	};


	template<class Key, class Pr, class Alloc> 
	inline void swap(vector_set<Key, Pr, Alloc>& lhs, vector_set<Key, Pr, Alloc>& rhs){	lhs.swap(rhs); }


	template<class Key, class Pr = less<Key>, class Alloc = i3::pool_allocator<Key> >
	class vector_multiset 	: public multi_sorted_vector< Key, Pr, Alloc >
	{	
	public:
		typedef vector_multiset<Key, Pr, Alloc>			my_type;
		typedef multi_sorted_vector< Key, Pr, Alloc >	base_type;

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


		vector_multiset() { }
		explicit vector_multiset(const key_compare& pr)	: base_type(pr) {}
		explicit vector_multiset(const key_compare& pr, const allocator_type& a) : base_type(pr, a) {}

		template<class Iter> vector_multiset(Iter f, Iter l) : base_type(f, l) {}
		template<class Iter> vector_multiset(Iter f, Iter l, const key_compare& pr) : base_type(f, l, pr) {} 
		template<class Iter> vector_multiset(Iter f, Iter l, const key_compare& pr, const allocator_type& a) : base_type(f, l, pr, a) {}

	};

	template<class Key, class Pr, class Alloc> inline
	void swap(vector_multiset<Key, Pr, Alloc>& lhs, vector_multiset<Key, Pr, Alloc>& rhs) {	lhs.swap(rhs); }

	
	
}

#pragma pack(pop)