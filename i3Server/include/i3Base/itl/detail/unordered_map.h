#pragma once

#include "../unordered_map_fwd.h"

#include "hash_table.h"

#include "../allocator.h"
#include "../memory.h"

#pragma pack(push, _CRT_PACKING)

namespace i3
{

	template <	class Key, class T, class Hash,	class Pred, class Alloc >
	class unordered_map : public hash_table_ext< HashTableNode_Unique< i3::pair<const Key, T>, Alloc>, Hash, Pred>
	{
		typedef hash_table_ext< HashTableNode_Unique< i3::pair<const Key, T>, Alloc>, Hash, Pred>	base_type;
	public:

		typedef Key									key_type;
		typedef i3::pair<const Key, T>				value_type;
		typedef T									mapped_type;
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

		explicit unordered_map( size_type n = default_initial_bucket_count, const hasher& hf = hasher(), const key_equal& eq = key_equal(),
			const allocator_type& a = allocator_type()) : base_type(n, hf, eq, a) { }

		explicit unordered_map(const allocator_type& a) : base_type(default_initial_bucket_count, hasher(), key_equal(), a) { }

		unordered_map(const unordered_map& other,const allocator_type& a) : base_type(other, a) {}
		unordered_map(const unordered_map& other) : base_type(other) {}

		template <class InputIterator>
		unordered_map(InputIterator f, InputIterator l)	: base_type(f, l, default_initial_bucket_count, hasher(), key_equal(), allocator_type()) { }

		template <class InputIterator>
		unordered_map(InputIterator f, InputIterator l, size_type n, const hasher &hf = hasher(), const key_equal &eq = key_equal(), 
			const allocator_type &a = allocator_type()) : base_type(f, l, n, hf, eq, a) { }

		mapped_type&	operator[](const key_type& key) {   return base_type::operator[](key).second; }
		mapped_type&			at(const key_type& key) {   return base_type::at(key).second; }
		const mapped_type&		at(const key_type& key) const { return base_type::at(key).second; }
	};

	template <class K, class T, class H, class P, class A>
	inline void swap(unordered_map<K, T, H, P, A> &m1, unordered_map<K, T, H, P, A> &m2)
	{
		m1.swap(m2);
	}


	template <	class Key, class T, class Hash,	class Pred, class Alloc >
	class unordered_multimap : public hash_table_ext< HashTableNode_Multi< i3::pair<const Key, T>, Alloc>, Hash, Pred >
	{
		typedef hash_table_ext< HashTableNode_Multi< i3::pair<const Key, T>, Alloc>, Hash, Pred>	base_type;
	public:
		typedef Key									key_type;
		typedef i3::pair<const Key, T>				value_type;
		typedef T									mapped_type;
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

		explicit unordered_multimap( size_type n = default_initial_bucket_count, const hasher& hf = hasher(), const key_equal& eq = key_equal(),
			const allocator_type& a = allocator_type()) : base_type(n, hf, eq, a) { }

		explicit unordered_multimap(const allocator_type& a) : base_type(default_initial_bucket_count, hasher(), key_equal(), a) { }

		unordered_multimap(const unordered_multimap& other,const allocator_type& a) : base_type(other, a) {}
		unordered_multimap(const unordered_multimap& other) : base_type(other) {}

		template <class InputIterator>
		unordered_multimap(InputIterator f, InputIterator l)	: base_type(f, l, default_initial_bucket_count, hasher(), key_equal(), allocator_type()) { }

		template <class InputIterator>
		unordered_multimap(InputIterator f, InputIterator l, size_type n, const hasher &hf = hasher(), const key_equal &eq = key_equal(), 
			const allocator_type &a = allocator_type()) : base_type(f, l, n, hf, eq, a) { }

	};

	template <class K, class T, class H, class P, class A>
	inline void swap(unordered_multimap<K, T, H, P, A> &m1, unordered_multimap<K, T, H, P, A> &m2)
	{
		m1.swap(m2);
	}










}



#pragma pack(pop)
