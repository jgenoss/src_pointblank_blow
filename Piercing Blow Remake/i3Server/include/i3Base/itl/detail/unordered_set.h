#pragma once

#include "../unordered_set_fwd.h"

#include "hash_table.h"


#pragma pack(push, _CRT_PACKING)

namespace i3
{
	

	template <	class T, class Hash, class Pred, class Alloc >
	class unordered_set : public hash_table_ext< HashTableNode_Unique< T, Alloc>, Hash, Pred>
	{
		typedef hash_table_ext< HashTableNode_Unique< T, Alloc>, Hash, Pred>	base_type;
	public:
		typedef T									key_type;
		typedef T									value_type;
		typedef Hash								hasher;
		typedef Pred								key_equal;
		typedef Alloc								allocator_type;

		typedef typename allocator_type::pointer			pointer;
		typedef typename allocator_type::const_pointer		const_pointer;
		typedef typename allocator_type::reference			reference;
		typedef typename allocator_type::const_reference	const_reference;
		typedef typename base_type::size_type				size_type;
		typedef typename base_type::difference_type			difference_type;

		typedef typename base_type::iterator				iterator;
		typedef typename base_type::const_iterator			const_iterator;
		typedef typename base_type::local_iterator			local_iterator;
		typedef typename base_type::const_local_iterator	const_local_iterator;

		static std::size_t		calculate_bucket_size(std::size_t n) {  return i3::calculate_bucket_size(n);   }

		explicit unordered_set( size_type n = default_initial_bucket_count, const hasher& hf = hasher(), const key_equal& eq = key_equal(),
			const allocator_type& a = allocator_type()) : base_type(n, hf, eq, a) { }

		explicit unordered_set(const allocator_type& a) : base_type(default_initial_bucket_count, hasher(), key_equal(), a) { }

		unordered_set(const unordered_set& other,const allocator_type& a) : base_type(other, a) {}
		unordered_set(const unordered_set& other) : base_type(other) {}

		template <class InputIterator>
		unordered_set(InputIterator f, InputIterator l)	: base_type(f, l, default_initial_bucket_count, hasher(), key_equal(), allocator_type()) { }

		template <class InputIterator>
		unordered_set(InputIterator f, InputIterator l, size_type n, const hasher &hf = hasher(), const key_equal &eq = key_equal(), 
			const allocator_type &a = allocator_type()) : base_type(f, l, n, hf, eq, a) { }

	};

	template <class T, class H, class P, class A> inline 
	void swap(unordered_set<T, H, P, A> &m1, unordered_set<T, H, P, A> &m2)
	{
		m1.swap(m2);
	}

	template <	class T, class Hash, class Pred, class Alloc >
	class unordered_multiset : public hash_table_ext< HashTableNode_Multi< T, Alloc>, Hash, Pred>
	{
		typedef hash_table_ext< HashTableNode_Multi< T, Alloc>, Hash, Pred>	base_type;
	public:
		typedef T									key_type;
		typedef T									value_type;
		typedef Hash								hasher;
		typedef Pred								key_equal;
		typedef Alloc								allocator_type;

		typedef typename allocator_type::pointer			pointer;
		typedef typename allocator_type::const_pointer		const_pointer;
		typedef typename allocator_type::reference			reference;
		typedef typename allocator_type::const_reference	const_reference;
		typedef typename base_type::size_type				size_type;
		typedef typename base_type::difference_type			difference_type;

		typedef typename base_type::iterator				iterator;
		typedef typename base_type::const_iterator			const_iterator;
		typedef typename base_type::local_iterator			local_iterator;
		typedef typename base_type::const_local_iterator	const_local_iterator;

		static std::size_t		calculate_bucket_size(std::size_t n) {  return i3::calculate_bucket_size(n);   }

		explicit unordered_multiset( size_type n = default_initial_bucket_count, const hasher& hf = hasher(), const key_equal& eq = key_equal(),
			const allocator_type& a = allocator_type()) : base_type(n, hf, eq, a) { }

		explicit unordered_multiset(const allocator_type& a) : base_type(default_initial_bucket_count, hasher(), key_equal(), a) { }

		unordered_multiset(const unordered_multiset& other,const allocator_type& a) : base_type(other, a) {}
		unordered_multiset(const unordered_multiset& other) : base_type(other) {}

		template <class InputIterator>
		unordered_multiset(InputIterator f, InputIterator l)	: base_type(f, l, default_initial_bucket_count, hasher(), key_equal(), allocator_type()) { }

		template <class InputIterator>
		unordered_multiset(InputIterator f, InputIterator l, size_type n, const hasher &hf = hasher(), const key_equal &eq = key_equal(), 
			const allocator_type &a = allocator_type()) : base_type(f, l, n, hf, eq, a) { }

	};

	template <class T, class H, class P, class A> inline 
	void swap(unordered_multiset<T, H, P, A> &m1, unordered_multiset<T, H, P, A> &m2)
	{
		m1.swap(m2);
	}


	
	
	
	
}

#pragma pack(pop)