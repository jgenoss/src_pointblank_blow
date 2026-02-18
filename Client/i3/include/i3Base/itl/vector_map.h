#pragma once

#include "vector_map_fwd.h"

#pragma pack(push, _CRT_PACKING)

#include "detail/sorted_vector.h"

namespace i3
{
	
	template<class Key, class T, class Pr, class Alloc >
	class vector_map : public sorted_vector< i3::pair< Key, T>, Pr, Alloc >
	{	
	public:
		typedef vector_map<Key, T, Pr, Alloc>							my_type;
		typedef sorted_vector< i3::pair< Key, T>, Pr, Alloc  >			base_type;

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

		typedef typename base_type::insert_commit_data	insert_commit_data;

		vector_map() { }
		explicit vector_map(const key_compare& pr)	: base_type(pr)	{}
		explicit vector_map(const key_compare& pr, const allocator_type& a) : base_type(pr, a){}

		template<class Iter> vector_map(Iter f, Iter l) : base_type(f, l) {}
		template<class Iter> vector_map(Iter f, Iter l, const key_compare& pr) : base_type(f, l, pr) {} 
		template<class Iter> vector_map(Iter f, Iter l, const key_compare& pr, const allocator_type& a) : base_type(f, l, pr, a) {}
		
		using base_type::erase;
		void			erase(const key_type* f, const key_type* l) { while( f != l ) erase(*f++);  }


		mapped_type& operator[](const key_type& key_val)
		{	
			iterator where = this->lower_bound(key_val);
			if (where == this->end() || this->comp()(key_val, key(*where) ) )
			where = this->insert(where, value_type(key_val, mapped_type()));
			return (*where).second;
		}
	};

	template<class Key, class T, class Pr, class Alloc> inline
	void swap(vector_map<Key, T, Pr, Alloc>& lhs, vector_map<Key, T, Pr, Alloc>& rhs) {	lhs.swap(rhs); }


	template<class Key,	class T, class Pr, class Alloc>
	class vector_multimap	: public multi_sorted_vector< i3::pair< Key, T>, Pr, Alloc  >
	{	
	public:
		typedef vector_multimap<Key, T, Pr, Alloc>						my_type;
		typedef multi_sorted_vector<i3::pair< Key, T>, Pr, Alloc >		base_type;

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


		vector_multimap() {}
		explicit vector_multimap(const key_compare& pr)	: base_type(pr){}
		explicit vector_multimap(const key_compare& pr, const allocator_type& a) : base_type(pr, a){}

		template<class Iter> vector_multimap(Iter f, Iter l) : base_type(f, l) {}
		template<class Iter> vector_multimap(Iter f, Iter l, const key_compare& pr) : base_type(f, l, pr) {} 
		template<class Iter> vector_multimap(Iter f, Iter l, const key_compare& pr, const allocator_type& a) : base_type(f, l, pr, a) {}

		using base_type::erase;
		void			erase(const key_type* f, const key_type* l) { while( f != l ) erase(*f++);  }

	};

	template<class Key,	class T, class Pr, class Alloc> inline
	void swap(vector_multimap<Key, T, Pr, Alloc>& lhs, vector_multimap<Key, T, Pr, Alloc>& rhs) {	lhs.swap(rhs);	}


}

#pragma pack(pop)
