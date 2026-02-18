#pragma once

#include "map_fwd.h"
#include "../pool/pool_allocator.h"

#pragma pack(push, _CRT_PACKING)
#include "detail/rbtree.h"

namespace i3
{

	template<class Key, class T, class Pr , class Alloc >
	class map : public rbtree< i3::pair<const Key, T>, Pr, Alloc >
	{	
	public:
		typedef map<Key, T, Pr, Alloc>								my_type;
		typedef rbtree< i3::pair<const Key, T>, Pr, Alloc  >		base_type;

		typedef typename base_type::key_type		key_type;
		typedef T									mapped_type;
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


		map() { }
		explicit map(const key_compare& pr)	: base_type(pr)	{}
		explicit map(const key_compare& pr, const allocator_type& a) : base_type(pr, a){}

		template<class Iter> map(Iter f, Iter l) : base_type(f, l) {}
		template<class Iter> map(Iter f, Iter l, const key_compare& pr) : base_type(f, l, pr) {} 
		template<class Iter> map(Iter f, Iter l, const key_compare& pr, const allocator_type& a) : base_type(f, l, pr, a) {}


		mapped_type& operator[](const key_type& key_val)
		{	
			iterator where = this->lower_bound(key_val);
			if (where == this->end() || this->comp()(key_val, key( where.get_node()) ) )
				where = this->insert(where, value_type(key_val, mapped_type()));
			return (*where).second;
		}
	};

	template<class Key, class T, class Pr, class Alloc> inline
	void swap(map<Key, T, Pr, Alloc>& lhs, map<Key, T, Pr, Alloc>& rhs) {	lhs.swap(rhs); }


	template<class Key,	class T, class Pr , class Alloc>
	class multimap	: public multi_rbtree< i3::pair<const Key, T>, Pr, Alloc  >
	{	
	public:
		typedef multimap<Key, T, Pr, Alloc> my_type;
		typedef multi_rbtree<i3::pair<const Key, T>, Pr, Alloc > base_type;

		typedef typename base_type::key_type		key_type;
		typedef T									mapped_type;
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
	
		
		multimap() {}
		explicit multimap(const key_compare& pr)	: base_type(pr){}
		explicit multimap(const key_compare& pr, const allocator_type& a) : base_type(pr, a){}

		template<class Iter> multimap(Iter f, Iter l) : base_type(f, l) {}
		template<class Iter> multimap(Iter f, Iter l, const key_compare& pr) : base_type(f, l, pr) {} 
		template<class Iter> multimap(Iter f, Iter l, const key_compare& pr, const allocator_type& a) : base_type(f, l, pr, a) {}



	};

	template<class Key,	class T, class Pr, class Alloc> inline
	void swap(multimap<Key, T, Pr, Alloc>& lhs, multimap<Key, T, Pr, Alloc>& rhs) {	lhs.swap(rhs);	}



}


#pragma pack(pop)
