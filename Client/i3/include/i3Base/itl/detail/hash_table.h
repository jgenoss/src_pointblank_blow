#pragma once 

#define _CRT_SECURE_NO_DEPRECATE
#define _SCL_SECURE_NO_WARNINGS	

//#include <functional>
#include "../iterator.h"
#include "../pair.h"
#include "../hash.h"
//#include <algorithm>
#include "../algorithm/lower_bound.h"
#include "../algorithm/upper_bound.h"

#include <limits>
#include <assert.h>

#include "../compressed_pair.h"

#pragma pack(push, _CRT_PACKING)
#pragma warning(push)
#pragma warning(disable : 4512)

namespace i3
{
	
	static const std::size_t default_initial_bucket_count = 53ul;		// 257 -> 131 -> 79 -> 53으로  감소시킴..
	static const float		minimum_max_load_factor = 1e-3f;
	namespace detail
	{
		typedef std::size_t			ignored_type;

		template<class T = ignored_type >
		struct prime_list 
		{
			static const std::size_t		value[];
			static const std::ptrdiff_t		length;
		};

		template<class T>
		const std::size_t prime_list<T>::value[] = 
		{
			5ul, 11ul, 17ul, 29ul, 37ul, 53ul, 79ul, 
			131ul, 257ul, 521ul, 1031ul, 1543ul, 
			2053ul, 3079ul, 6151ul, 12289ul, 24593ul, 
			49157ul, 98317ul, 196613ul, 393241ul, 786433ul, 
			1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul, 
			50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul, 
			1610612741ul, 3221225473ul, 4294967291ul 
		};

		template<class T>
		const std::ptrdiff_t	prime_list<T>::length = sizeof( prime_list<T>::value) / sizeof( std::size_t );
		/*
		const std::size_t prime_list::value[] = 
		{
			5ul, 11ul, 17ul, 29ul, 37ul, 
			53ul, 67ul, 79ul, 97ul, 131ul,
			193ul, 257ul, 389ul, 521ul, 769ul, 
			1031ul, 1543ul, 2053ul, 3079ul, 6151ul, 
			12289ul, 24593ul, 49157ul, 98317ul, 196613ul, 
			393241ul, 786433ul, 1572869ul, 3145739ul, 6291469ul, 
			12582917ul, 25165843ul, 50331653ul, 100663319ul, 201326611ul, 
			402653189ul, 805306457ul, 1610612741ul, 3221225473ul, 4294967291ul 
		};
		*/
		


		inline std::size_t	next_prime(std::size_t n) // 다음것..
		{
			const std::size_t* begin = detail::prime_list<>::value;		
			const std::size_t* end   = begin + detail::prime_list<>::length;
			const std::size_t* bound = i3::lower_bound(begin, end, n);
			if ( bound == end ) --bound;
			return *bound;
		}

		inline std::size_t	prev_prime(std::size_t n) // 현재것을 포함..
		{
			const std::size_t* begin = detail::prime_list<>::value;		
			const std::size_t* end   = begin + detail::prime_list<>::length;
			const std::size_t* bound = i3::upper_bound(begin, end, n);

			if (bound != begin)
				--bound;
			return *bound;
		}

		inline std::size_t double_to_size_t(double f)
		{
			return f >= static_cast<double>((std::numeric_limits<std::size_t>::max)()) 
				? (std::numeric_limits<std::size_t>::max)()
				: static_cast<std::size_t>(f);
		}

		struct unordered_insert_commit_data
		{
			std::size_t	hash;
		};
	}

	inline std::size_t		calculate_bucket_size(std::size_t n)
	{
		return detail::next_prime(n);
	}

//	template<class Value, bool isMulti, class Alloc> class HashTableNode;

	template<class Value, class Alloc>
	class HashTableNode_Unique
	{
	protected:
		typedef			Value												value_type;
		typedef typename Alloc::template rebind<value_type>::other			allocator_type;	
		typedef			typename Alloc::size_type							size_type;
			
		struct Node
		{	
			Node(const Value& val ) : myValue(val) {}
			template<class K, class D> Node(const K& key, const D& data) : myValue(key, data) {}
			Value myValue;
			Node* pNext;
		};
		typedef			typename Alloc::template rebind<Node>::other		node_allocator_type;

		struct Bucket {		Node*	pHead;	};		// 이걸 배열에 담음...
		
		static inline size_type node_count(Node* pNode)
		{
			size_type count = 0;
			for ( ; pNode ; pNode = pNode->pNext, ++count );
			return count;
		}
		static inline size_type node_count(Node* pNode1, Node* pNode2)
		{
			size_type count = 0;
			for ( ; pNode1 != pNode2 ; pNode1 = pNode1->pNext, ++count );
			return count;
		}

		static inline Node*&	next_group(Node* pNode) { return pNode->pNext; }
		static inline size_type	group_count(Node*) { return 1; }		// 무조건 1개..
		
		static  inline void					link_node( Node* pSrc_Node, Bucket* pDest_Bucket )
		{
			pSrc_Node->pNext = pDest_Bucket->pHead;
			pDest_Bucket->pHead = pSrc_Node;
		}
		static inline void					link_group( Node* pSrc_Node, Bucket* pDest_Bucket )
		{
			link_node(pSrc_Node, pDest_Bucket);
		}
		static inline size_type unlink_group(Node** ppNode)
		{
			*ppNode = (*ppNode)->pNext;
			return 1;
		}

		static inline Node**					get_for_erase( Node* pNode, Bucket* pBucket )
		{
			Node** ppResult = &pBucket->pHead;
			while ( *ppResult != pNode ) ppResult = &(*ppResult)->pNext;
			return ppResult;
		}
		
		static inline void			unlink_node( Node* pNode, Bucket* pBucket)		// 유효한 이터레이터로부터..(2개 모두 정상값)
		{
			Node** ppNode = get_for_erase(pNode, pBucket);
			*ppNode = (*ppNode)->pNext;
		}

		static inline void split_group(Node*){}
		static inline void split_group(Node*, Node*){}

		static inline void	delete_nodes(Node* pBegin, Node* pEnd, node_allocator_type& n_alloc)			// 유닉/멀티 공통..동일함..
		{
			while (pBegin != pEnd)
			{
				Node* pNode = pBegin;
				pBegin = pBegin->pNext;
				n_alloc.destroy(pNode);
				n_alloc.deallocate(pNode, 1);
			}
		}

		static inline void	delete_group(Node* first_node, node_allocator_type& n_alloc)
		{
			n_alloc.destroy(first_node);
			n_alloc.deallocate(first_node, 1);
		}

		static inline size_type		copy_group(Node* it, Bucket* dst, node_allocator_type& n_alloc)
		{
			Node* pResult = n_alloc.allocate(1); 
			new (pResult) Node( it->myValue );
			link_node(pResult, dst);
			return 1;
		}
		
	};

	template<class Value, class Alloc>
	class HashTableNode_Multi
	{
	protected:
		typedef			Value												value_type;
		typedef typename Alloc::template rebind<value_type>::other			allocator_type;	
		typedef			typename Alloc::size_type							size_type;

		struct Node
		{
			Node(const Value& val ) : myValue(val) {}
			template<class K, class D> Node(const K& key, const D& data) : myValue(key, data) {}
			Value myValue;	Node* pNext;	Node* pPrev_InGroup;
		};
		typedef			typename Alloc::template rebind<Node>::other		node_allocator_type;

		struct Bucket {	Node*	pHead;	};		// 이걸 배열에 담음...

		static inline Node*& next_group(Node* pNode) { return pNode->pPrev_InGroup->pNext; }

		static inline size_type node_count(Node* pNode)
		{
			size_type count = 0;
			for ( ; pNode ; pNode = pNode->pNext, ++count );
			return count;
		}
		static inline size_type node_count(Node* pNode1, Node* pNode2)
		{
			size_type count = 0;
			for ( ; pNode1 != pNode2 ; pNode1 = pNode1->pNext, ++count );
			return count;
		}

		static inline size_type	group_count(Node* pNodeGrpBegin ) 
		{ 
			return node_count(pNodeGrpBegin, next_group(pNodeGrpBegin) );
		}		
		
		static inline void link_node( Node* pNewNode, Node* pWhereBegin )
		{
			pNewNode->pNext = pWhereBegin->pPrev_InGroup->pNext;
			pNewNode->pPrev_InGroup = pWhereBegin->pPrev_InGroup;
			pWhereBegin->pPrev_InGroup->pNext = pNewNode;
			pWhereBegin->pPrev_InGroup = pNewNode;
		}

		static  inline void link_node( Node* pSrc_Node, Bucket* pDest_Bucket )
		{
			pSrc_Node->pNext = pDest_Bucket->pHead;
			pSrc_Node->pPrev_InGroup = pSrc_Node;			// 자기 자신을 가르키게함.
			pDest_Bucket->pHead = pSrc_Node;
		}

		static inline void link_group( Node* pSrc_Begin, Bucket* pDest_Bucket)
		{
			Node* pLastNode = pSrc_Begin->pPrev_InGroup;
			pLastNode->pNext = pDest_Bucket->pHead;
			pDest_Bucket->pHead = pSrc_Begin;				
		}
		
		static inline size_type unlink_group(Node** ppNode)
		{
			size_type count = group_count(*ppNode);
			*ppNode = next_group(*pos);			
			return count;
		}

		static inline Node**	get_for_erase(	Node* pNode, Bucket* pBucket) 
		{
			Node** ppResult = &pNode->pPrev_InGroup->pNext;

			if (*ppResult == pNode ) 

				return ppResult;
			ppResult = &pBucket->pHead;
			while ( *ppResult != pNode ) ppResult = &(*ppResult)->pPrev_InGroup->pNext;
			return ppResult;
		}
		
		static inline void			unlink_node( Node* pNode, Bucket* pBucket)		// 유효한 이터레이터로부터..(2개 모두 정상값)
		{
			Node** ppNode	= get_for_erase(pNode, pBucket);
			pNode			= *ppNode;
			Node*  pNext	= pNode->pNext;

			if (  pNode->pPrev_InGroup != *ppNode )
			{
				if ( pNext && pNext->pPrev_InGroup == *ppNode )
					pNext->pPrev_InGroup = pNode->pPrev_InGroup;
				else
				{
					Node* it = pNode->pPrev_InGroup;
					while ( it->pPrev_InGroup != *ppNode ) it = it->pPrev_InGroup;
					it->pPrev_InGroup = pNode->pPrev_InGroup;
				}
			}
			*ppNode = pNext;

		}
		
		static inline Node* split_group(Node* split)
		{
			// If split is at the beginning of the group then there's
			// nothing to split.
			if( split->pPrev_InGroup->pNext != split)
				return nullptr;

			// Find the start of the group.
			Node* start = split;
			do 
			{
				start = start->pPrev_InGroup;
			} while( start->pPrev_InGroup->pNext == start);

			Node* last = start->pPrev_InGroup;
			start->pPrev_InGroup = split->pPrev_InGroup;
			split->pPrev_InGroup = last;

			return start;
		}

		static inline void split_group(Node* split1, Node* split2)
		{
			Node* begin1 = split_group(split1);
			Node* begin2 = split_group(split2);

			if( begin1 && split1 == begin2) 
			{
				Node* end1 = begin1->pPrev_InGroup;
				begin1->pPrev_InGroup = begin2->pPrev_InGroup;
				begin2->pPrev_InGroup = end1;
			}
		}



		static inline void		delete_nodes(Node* pBegin, Node* pEnd, node_allocator_type& n_alloc)			// 유닉/멀티 공통..동일함..
		{
			while (pBegin != pEnd)
			{
				Node* pNode = pBegin;
				pBegin = pBegin->pNext;
				n_alloc.destroy(pNode);
				n_alloc.deallocate(pNode, 1);
			}
		}

		static inline void		delete_group(Node* first_node, node_allocator_type& n_alloc)
		{
			delete_nodes( first_node, next_group( first_node ), n_alloc );
		}
		
		static inline size_type	copy_group(Node* it, Bucket* dst, node_allocator_type& n_alloc)
		{
			Node* end = next_group(it);
			
			Node* pNode = n_alloc.allocate(1);
			new (pNode) Node(it->myValue);
			
			link_node( pNode, dst);
			size_type count = 1;
			
			Node* pNodeNext;
			for ( it = it->pNext; it != end ; it = it->pNext, ++count )
			{
				pNodeNext = n_alloc.allocate(1);
				new (pNode) Node(it->myValue);
				link_node( pNodeNext, pNode );
			}
			return count;
		}

	};



	template <class HashTableNode>
	class HashTableValue : public HashTableNode
	{
	public:	
		typedef			 HashTableNode													base_type;

		typedef	typename HashTableNode::Node											Node;
		typedef	typename HashTableNode::Bucket											Bucket;

		typedef	typename HashTableNode::value_type										value_type;
		typedef	typename HashTableNode::allocator_type									allocator_type;
		typedef typename HashTableNode::node_allocator_type								node_allocator_type;

		typedef typename allocator_type::difference_type								difference_type;
		typedef typename allocator_type::const_pointer									const_pointer;
		typedef typename allocator_type::const_reference								const_reference;

		class const_local_iterator : public i3::iterator< forward_iterator_tag, value_type, difference_type, const_pointer, const_reference>
		{
		public:
			typedef const_pointer pointer;
			typedef const_reference reference;
			
			const_local_iterator() : pNode(0) {}					 // construct with null node pointer
			const_local_iterator(Node* pNode) : pNode(pNode) {}// construct with node pointer _Pnode
			const_reference operator*() const {	return pNode->myValue; } // return designated value
			const_pointer operator->() const {	return (&**this);  } // return pointer to class object
			const_local_iterator& operator++() {	pNode = pNode->pNext;	return (*this);	}// preincrement
			const_local_iterator operator++(int) { const_local_iterator _Tmp = *this;  ++*this; return (_Tmp);	}// postincrement

			bool operator==(const const_local_iterator& _Right) const {	return (pNode == _Right.pNode);	}// test for iterator equality
			bool operator!=(const const_local_iterator& _Right) const {	return (!(*this == _Right));	} // test for iterator inequality

			Node* get_node() const {	return (pNode);	}
		private:
			Node* pNode;	
		};

		class const_iterator : public i3::iterator< forward_iterator_tag, value_type, difference_type, const_pointer, const_reference>
		{	
		public:
			typedef const_pointer pointer;
			typedef const_reference reference;

			const_iterator() : pBucket(0), pNode(0) {}
			const_iterator(Bucket* pBucket) : pBucket(pBucket), pNode(pBucket->pHead) {}
			const_iterator(Bucket* pBucket,	Node* pNode) : pBucket(pBucket), pNode(pNode) {}

			const_reference operator*() const {	return pNode->myValue; } // return designated value
			const_pointer operator->() const {	return (&**this);  } // return pointer to class object
			const_iterator& operator++() {	increment();	return (*this);	}// preincrement
			const_iterator operator++(int) { const_iterator _Tmp = *this;  ++*this; return (_Tmp);	}// postincrement
		
			bool operator==(const const_iterator& _Right) const {	return (pNode == _Right.pNode);	}// test for iterator equality
			bool operator!=(const const_iterator& _Right) const {	return (!(*this == _Right));	} // test for iterator inequality
			
			
			void increment()
			{	// move to node with next larger value
				
				pNode = pNode->pNext;
				while (!pNode)
				{
					++pBucket;	
					pNode = pBucket->pHead;
				}
			}

			void increment_group()
			{
				pNode =	base_type::next_group(pNode);
				
				while (!pNode)
				{
					++pBucket;
					pNode = pBucket->pHead;
				}
			}

			Node* get_node() const {	return pNode;	}
			Bucket*	get_bucket() const { return pBucket; }

		private:
			Bucket* pBucket;
			Node* pNode;	
		};

		class local_iterator : public const_local_iterator
		{
		public:
			typedef typename allocator_type::pointer			pointer;
			typedef typename allocator_type::reference			reference;

			local_iterator()	{ } // construct with null node pointer
			local_iterator(Node* pNode) : const_local_iterator(pNode){} // construct with node pointer _Pnode
			reference operator*() const { return ((reference)**(const_local_iterator *)this); } // return designated value
			pointer operator->() const { return (&**this);	} // return pointer to class object
			local_iterator& operator++() {	++(*(const_local_iterator *)this);	return (*this);	}// preincrement
			local_iterator operator++(int) {	local_iterator _Tmp = *this;	++*this;	return (_Tmp);	}// postincrement
		};

		class iterator	: public const_iterator
		{	
		public:
			typedef typename allocator_type::pointer			pointer;
			typedef typename allocator_type::reference			reference;

			iterator()	{ } 
			iterator(Bucket* pBucket) : const_iterator(pBucket) {}
			iterator(Bucket* pBucket, Node* pNode) : const_iterator(pBucket, pNode) {}
			
			reference operator*() const { return ((reference)**(const_iterator *)this); } 
			pointer operator->() const { return (&**this);	} 
			iterator& operator++() {	++(*(const_iterator *)this);	return (*this);	}
			iterator operator++(int) {	iterator _Tmp = *this;	++*this;	return (_Tmp);	}
		};
		
	};

	template <class HashTableNode>
	class hash_table_key : public HashTableValue< HashTableNode>
	{
	public:
		
		typedef			typename HashTableNode::value_type		key_type;
	protected:

		static __forceinline const value_type&	key(Node* pNode) { return pNode->myValue; }
		static __forceinline const value_type&	key(const value_type& val) { return val; }
		static __forceinline Node* make_init_node(const key_type& key, node_allocator_type& n_alloc) 
		{ 
			Node* pNode = n_alloc.allocate(1);
			new (pNode) Node( key );
			return pNode;  
		}

	};

	template <	template<class,class> class HashTableNode, class Key, class T, class Alloc>
	class hash_table_key< HashTableNode< i3::pair<const Key, T>, Alloc> > 
		: public HashTableValue< HashTableNode< i3::pair<const Key, T>, Alloc> >
	{
	public:
		typedef			Key						key_type;
	protected:
		static __forceinline const Key&		key(Node* pNode) { return pNode->myValue.first; }
		static __forceinline const Key&		key(const value_type& val) { return val.first; }
		static __forceinline Node*		make_init_node(const key_type& key, node_allocator_type& n_alloc) 
		{ 
			Node* pNode = n_alloc.allocate(1);
			new (pNode) Node( key, T() );
			return pNode;
		}
	};

////////////////////////////////////////////////////////////////////////

	template < class HashTableNode, class Hash, class Pred>
	class hash_table : public hash_table_key<HashTableNode >
	{
	public:
		typedef				hash_table_key<HashTableNode >								base_type;

		typedef				typename base_type::iterator								iterator;
		typedef				typename base_type::const_iterator							const_iterator;
		typedef				typename base_type::local_iterator							local_iterator;
		typedef				typename base_type::const_local_iterator					const_local_iterator;

		typedef				Hash														hasher;
		typedef				Pred														key_equal;
		typedef				typename allocator_type::size_type							size_type;
		typedef				typename allocator_type::template rebind<Bucket>::other		bucket_allocator_type;
		

		allocator_type		get_allocator() const {   return node_alloc(); }			
		bool				empty() const	{ return num_size() == 0; }					
		size_type			max_size() const { return node_alloc().max_size(); }		
		size_type			size() const { return num_size(); }							

		iterator			begin()	{ return iterator( bucket_begin() ); }				
		const_iterator		begin() const { return const_iterator( bucket_begin() ); }	
		iterator			end()  { return iterator( buckets() + static_cast<difference_type>(num_bucket()) );	}
		const_iterator		end() const {  return iterator( buckets() + static_cast<difference_type>(num_bucket()) ); }
			
		
		local_iterator			begin(size_type idx) { return local_iterator( ( buckets() + static_cast<difference_type>(idx))->pHead ); }
		const_local_iterator	begin(size_type idx) const { return local_iterator( ( buckets() + static_cast<difference_type>(idx))->pHead ); }
		local_iterator			end(size_type idx) { return local_iterator(); }
		const_local_iterator	end(size_type idx) const { return const_local_iterator(); }


		hasher				hash_function() const { return hash_fun(); }	
		key_equal			key_eq() const { return	equal_fun(); }			
		size_type			bucket_count() const  { return num_bucket(); }	
		size_type			max_bucket_count() const { return detail::prev_prime( node_alloc().max_size() - 1 ); }
		size_type			bucket_size(size_type idx) { return node_count( buckets()[idx].pHead );	}

		template<class AnyKeyType>
		size_type			bucket(const AnyKeyType& key) const { return hash_fun()(key) % num_bucket(); }
			
		template<class AnyKeyType, class AnyHashFun>
		size_type			bucket(const AnyKeyType& key, AnyHashFun any_hash_fun) const { return any_hash_fun(key) % num_bucket(); }


		float				load_factor() const {    return static_cast<float>(num_size()) / static_cast<float>(num_bucket()); }
		void				max_load_factor(float z) {  m_max_load_factor = (std::max)(z, minimum_max_load_factor);	calculate_max_load(); }

		float				max_load_factor() const { return m_max_load_factor; }
		
		hash_table(size_type numBucket, const hasher& hf, const key_equal& eq,	const allocator_type& a)
			:	m_node_alloc_and_buckets(a), m_bucket_alloc_and_bucket_begin(a), m_hash_function_and_num_bucket(hf, numBucket),
				m_equal_function_and_num_size(eq, 0), m_max_load_factor(1.f)
		{
			allocate_new_buckets( buckets(), bucket_begin(), num_bucket() );	
			calculate_max_load(); 
		}
		
		template < class InputIterator>
		hash_table(InputIterator f, InputIterator l, size_type numBucket, const hasher& hf, const key_equal& eq, const allocator_type& a) :
			m_node_alloc_and_buckets(a), m_bucket_alloc_and_bucket_begin(a), 
			m_hash_function_and_num_bucket(hf, get_bucket_size_iterator_range(f, l, numBucket) ),
			m_equal_function_and_num_size(eq, 0), m_max_load_factor(1.f)
		{
			allocate_new_buckets( buckets(), bucket_begin(), num_bucket() );	
			calculate_max_load();
		}

		
		hash_table(const hash_table& rhs) : 
			m_hash_function_and_num_bucket(rhs.hash_fun(), rhs.num_bucket()), m_equal_function_and_num_size( rhs.equal_fun(), rhs.num_size()),
			m_max_load_factor(rhs.m_max_load_factor), m_max_load(rhs.m_max_load)
		{
			allocate_new_buckets( buckets(), bucket_begin(), num_bucket() );	
			rhs.copy_buckets_to(buckets(),  bucket_begin(), num_bucket(), *this);	
		}

		hash_table(const hash_table& rhs, const allocator_type& a) : 
			m_node_alloc_and_buckets(a), m_bucket_alloc_and_bucket_begin(a), 
			m_hash_function_and_num_bucket(rhs.hash_fun(), rhs.num_bucket()), m_equal_function_and_num_size( rhs.equal_fun(), rhs.num_size()),
			m_max_load_factor(rhs.m_max_load_factor), m_max_load(rhs.m_max_load)
		{
			allocate_new_buckets( buckets(), bucket_begin(), num_bucket() );	
			rhs.copy_buckets_to(buckets(),  bucket_begin(), num_bucket(), *this); 	
		}


		hash_table&			operator=(const hash_table& rhs)
		{
			if ( this == &rhs) 
				return *this;
			
			clear();
			m_max_load_factor = rhs.m_max_load_factor;
			calculate_max_load();

			if ( rhs.num_size() >= m_max_load )
				rehash_impl( min_buckets_for_size(rhs.num_size()));
			
			rhs.copy_buckets_to( buckets(), bucket_begin(), num_bucket(), *this);

			num_size() = rhs.num_size();
			hash_fun() = rhs.hash_fun();
			equal_fun() = rhs.equal_fun();
			return *this;
		}
		
		~hash_table()
		{
			if (buckets())
			{
				Bucket* end   = buckets() + static_cast<difference_type>( num_bucket() );
				for ( Bucket* begin = bucket_begin() ; begin != end ; ++begin )
					clear_bucket(begin);
				
				bucket_alloc().deallocate( buckets() , num_bucket() + 1 );
			}
		}

		iterator			erase(const_iterator pos)
		{
			assert(pos != end());
			const_iterator next = pos;
			next.increment();

			unlink_node(pos.get_node(),pos.get_bucket());

			--num_size();

			node_alloc().destroy(pos.get_node());
			node_alloc().deallocate(pos.get_node(), 1);

			// r has been invalidated but its bucket is still valid
			recompute_begin_bucket_erase( pos.get_bucket(), next.get_bucket() );		// 이터레이터 증가후이므로 안전하다고 생각..

			return iterator(next.get_bucket(), next.get_node());
		}

		template<class AnyKeyType>
		size_type erase_other(const AnyKeyType& key)
		{
			Bucket* pBucket = get_bucket(key);
			Node**	ppNode  = find_for_erase(pBucket, key);	

			if (!*ppNode)
				return 0;

			Node* pNode = *ppNode;
			size_type count = unlink_group(ppNode);
			num_size() -= count;
			delete_group( pNode, node_alloc() );

			recompute_begin_bucket_erase(pBucket);
			return count;
		}
		
		template<class AnyKeyType, class AnyHashFun, class AnyEqualFun>
		size_type erase_other(const AnyKeyType& key, AnyHashFun any_hash_fun, AnyEqualFun any_equal_fun)
		{
			Bucket* pBucket = get_bucket(key, any_hash_fun);
			Node**	ppNode  = find_for_erase(pBucket, key, any_equal_fun);	

			if (!*ppNode)
				return 0;

			Node* pNode = *ppNode;
			size_type count = unlink_group(ppNode);
			num_size() -= count;
			delete_group( pNode, node_alloc() );

			recompute_begin_bucket_erase(pBucket);
			return count;			
		}

		size_type erase(const key_type& key)
		{
			Bucket* pBucket = get_bucket(key);
			Node**	ppNode  = find_for_erase(pBucket, key);	

			if (!*ppNode)
				return 0;

			Node* pNode = *ppNode;
			size_type count = unlink_group(ppNode);
			num_size() -= count;
			delete_group( pNode, node_alloc() );

			recompute_begin_bucket_erase(pBucket);
			return count;
		}

		iterator erase(const_iterator first, const_iterator last)
		{
			if ( first == last ) 
				return iterator(last.get_bucket(),last.get_node());

			if ( first.get_bucket() == last.get_bucket()() )	// 같은 버킷내에서..
			{
				num_size() -= unlink_nodes(first, last);		// 삭제된 갯수를 리턴하게 조정..
				delete_nodes(first.get_node(), last.get_node(), node_alloc() );
			}
			else
			{
				num_size() -= unlink_nodes(first);
				delete_to_bucket_end( first.get_node() );

				Bucket* i = first.get_bucket();
				for( ++i ; i != last.get_bucket() ; ++i )
				{
					num_size() -= node_count(i->pHead);
					clear_bucket(i);
				}

				if ( last != end() )
				{
					Node* pNode = last.get_bucket()->pHead;
					num_size() -= unlink_nodes( last.get_bucket(), last );
					delete_nodes( pNode, last.get_node(), node_alloc());
				}

				recompute_begin_bucket_erase(first.get_bucket(), last.get_bucket() );

			}
			return iterator(last.get_bucket(),last.get_node());
		}

		void				clear()
		{
			Bucket* begin = bucket_begin();		
			Bucket* end = buckets() + static_cast<difference_type>(num_bucket());
			
			num_size() = 0;
			bucket_begin() = end;

			while(begin != end) {
				clear_bucket(begin);
				++begin;
			}
		}

		void				swap(hash_table& other)
		{
			if ( this == &other) return;
			
			if ( node_alloc() == other.node_alloc() )		// 기본 얼로케이터만 같다면...
			{
				i3::swap( buckets(), other.buckets() );		// 포인터배열의 포인터 교환
				i3::swap( num_bucket(), other.num_bucket());
				i3::swap( bucket_begin(), other.bucket_begin());
				i3::swap( num_size(), other.num_size());
			}
			else
			{
				Bucket* pThisBucket;	Bucket* pThisBucket_Begin;	size_type numBucketThis = other.min_buckets_for_size(other.num_size());
				allocate_new_buckets( pThisBucket, pThisBucket_Begin, numBucketThis );
				other.copy_buckets_to(pThisBucket, pThisBucket_Begin, numBucketThis, *this);
				
				Bucket* pThatBucket;	Bucket* pThatBucket_Begin;  size_type numBucketThat = min_buckets_for_size(num_size());
				other.allocate_new_buckets( pThatBucket, pThatBucket_Begin, numBucketThat);
				copy_buckets_to(pThatBucket, pThatBucket_Begin, numBucketThat, other);
				
				change_into_new_buckets(pThisBucket, pThisBucket_Begin, numBucketThis);
				other.change_into_new_buckets(pThatBucket, pThatBucket_Begin, numBucketThat);
				
				i3::swap( num_size(), other.num_size());
			}

			i3::swap( m_max_load_factor, other.m_max_load_factor);
			
			i3::swap( hash_fun(), other.hash_fun());
			i3::swap( equal_fun(), other.equal_fun());
			
			calculate_max_load();
			other.calculate_max_load();
		}
		
		iterator			find(const key_type& key) 
		{
			Bucket* pBucket = get_bucket(key);			Node*	it = find_node(pBucket, key);
			return (it) ? iterator(pBucket, it) : end();
		}
		const_iterator		find(const key_type& key) const
		{
			Bucket* pBucket = get_bucket(key);			Node*	it = find_node(pBucket, key);
			return (it) ? const_iterator(pBucket, it) : end();
		}
		
		template<class AnyKeyType>
		iterator			find_other(const AnyKeyType& key) 
		{
			Bucket* pBucket = get_bucket(key);			Node*	it = find_node(pBucket, key);
			return (it) ? iterator(pBucket, it) : end();
		}
		template<class AnyKeyType>
		const_iterator		find_other(const AnyKeyType& key) const
		{
			Bucket* pBucket = get_bucket(key);			Node*	it = find_node(pBucket, key);
			return (it) ? const_iterator(pBucket, it) : end();
		}

		template<class AnyKeyType, class AnyHashFun, class AnyEqualFun>
		iterator			find_other(const AnyKeyType& key, AnyHashFun any_hash_fun, AnyEqualFun any_equal_fun)
		{
			Bucket* pBucket = get_bucket(key, any_hash_fun);	Node*	it = find_node(pBucket, key, any_equal_fun);
			return (it) ? iterator(pBucket, it) : end();
		}
		template<class AnyKeyType, class AnyHashFun, class AnyEqualFun>
		const_iterator		find_other(const AnyKeyType& key, AnyHashFun any_hash_fun, AnyEqualFun any_equal_fun) const
		{
			Bucket* pBucket = get_bucket(key, any_hash_fun);	Node*	it = find_node(pBucket, key, any_equal_fun);
			return (it) ? const_iterator(pBucket, it) : end();
		}

		size_type count(const key_type& key) const
		{
			Node* it = find_node(key);
			return (it) ? group_count(it) : 0;
		}
		template<class AnyKeyType>
		size_type count_other(const AnyKeyType& key) const
		{
			Node* it = find_node(key);
			return (it) ? group_count(it) : 0;
		}
		template<class AnyKeyType, class AnyHashFun, class AnyEqualFun>
		size_type count_other(const AnyKeyType& key, AnyHashFun any_hash_fun, AnyEqualFun any_equal_fun) const
		{
			Node* it = find_node(key, any_hash_fun, any_equal_fun);
			return (it) ? group_count(it) : 0;
		}

		i3::pair<iterator, iterator> equal_range(const key_type& key)
		{
			Bucket* pBucket = get_bucket(key);
			Node*   it = find_node(pBucket, key);
			if (it)
			{
				iterator first(pBucket, it);		iterator second(first);
				second.increment_group();
				return i3::pair<iterator, iterator>(first, second);
			}
			return i3::pair<iterator, iterator>( end(), end() );
		}

		i3::pair<const_iterator, const_iterator> equal_range(const key_type& key) const
		{
			Bucket* pBucket = get_bucket(key);
			Node*   it = find_node(pBucket, key);
			if (it)
			{
				const_iterator first(pBucket, it);	const_iterator second(first);
				second.increment_group();
				return i3::pair<const_iterator, const_iterator>(first, second);
			}
			return i3::pair<const_iterator, const_iterator>( end(), end() );
		}
		
		template<class AnyKeyType>
		i3::pair<iterator, iterator> equal_range_other(const AnyKeyType& key)
		{
			Bucket* pBucket = get_bucket(key);
			Node*   it = find_node(pBucket, key);
			if (it)
			{
				iterator first(pBucket, it);		iterator second(first);
				second.increment_group();
				return i3::pair<iterator, iterator>(first, second);
			}
			return i3::pair<iterator, iterator>( end(), end() );
		}
		
		template<class AnyKeyType>
		i3::pair<const_iterator, const_iterator> equal_range_other(const AnyKeyType& key) const
		{
			Bucket* pBucket = get_bucket(key);
			Node*   it = find_node(pBucket, key);
			if (it)
			{
				const_iterator first(pBucket, it);	const_iterator second(first);
				second.increment_group();
				return i3::pair<const_iterator, const_iterator>(first, second);
			}
			return i3::pair<const_iterator, const_iterator>( end(), end() );
		}

		template<class AnyKeyType, class AnyHashFun, class AnyEqualFun>
		i3::pair<iterator, iterator> equal_range_other(const AnyKeyType& key, AnyHashFun any_hash_fun, AnyEqualFun any_equal_fun)
		{
			Bucket* pBucket = get_bucket(key, any_hash_fun);
			Node*   it = find_node(pBucket, key, any_equal_fun);
			if (it)
			{
				iterator first(pBucket, it);		iterator second(first);
				second.increment_group();
				return i3::pair<iterator, iterator>(first, second);
			}
			return i3::pair<iterator, iterator>( end(), end() );
		}

		template<class AnyKeyType, class AnyHashFun, class AnyEqualFun>
		i3::pair<const_iterator, const_iterator> equal_range_other(const AnyKeyType& key, AnyHashFun any_hash_fun, AnyEqualFun any_equal_fun) const
		{
			Bucket* pBucket = get_bucket(key, any_hash_fun);
			Node*   it = find_node(pBucket, key, any_equal_fun);
			if (it)
			{
				const_iterator first(pBucket, it);	const_iterator second(first);
				second.increment_group();
				return i3::pair<const_iterator, const_iterator>(first, second);
			}
			return i3::pair<const_iterator, const_iterator>( end(), end() );
		}

		void			rehash( size_type n)
		{
			size_type min_size = min_buckets_for_size(size());
			rehash_impl( min_size > n ? min_size : n);
		}
		
	protected:

		template<class AnyKeyType>
		size_type			hash_function(const AnyKeyType& v) const { return hash_fun()(v); }

		template<class AnyKeyType1, class AnyKeyType2>
		bool				key_function(const AnyKeyType1& l, const AnyKeyType2& r) const { return equal_fun()(l,r); }

		size_type			bucket_from_hash_value(size_type hash_val) const {	return hash_val %  num_bucket(); }

		template<class AnyKeyType>
		Bucket*				get_bucket(const AnyKeyType& key) const { return buckets() + static_cast<difference_type>(bucket(key)); }
		
		template<class AnyKeyType, class AnyHashFun>
		Bucket*				get_bucket(const AnyKeyType& key, AnyHashFun any_hash_fun) const {  return buckets() + static_cast<difference_type>(bucket(key, any_hash_fun));  }

		Bucket*				get_bucket_from_hash_value(size_type hash_val) const 
			{ return buckets() + static_cast<difference_type>(bucket_from_hash_value(hash_val) ); }

		template<class AnyKeyType>
		Node*				find_node(const AnyKeyType& k) const {	return find_node( get_bucket(k), k ); }

		template<class AnyKeyType, class AnyHashFun, class AnyEqualFun>
		Node*				find_node(const AnyKeyType& k, AnyHashFun any_hash_fun, AnyEqualFun any_equal_fun) const { return find_node( get_bucket(k, any_hash_fun), k, any_equal_fun); }

		template<class AnyKeyType>
		Node*				find_node(Bucket* pBucket, const AnyKeyType& k) const
		{
			Node* pNode = pBucket->pHead;
			while(  pNode &&  !equal_fun()( k, key(pNode) ) )		// pNode가 널이거나, 찾으면 스돕..
				pNode = next_group(pNode);
			return pNode;
		}

		template<class AnyKeyType, class AnyEqualFun>
		Node*				find_node(Bucket* pBucket, const AnyKeyType& k, AnyEqualFun any_equal_fun) const 
		{
			Node* pNode = pBucket->pHead;
			while(  pNode &&  !any_equal_fun( k, key(pNode) ) )		// pNode가 널이거나, 찾으면 스돕..
				pNode = next_group(pNode);
			return pNode;	
		}
		

		Node* 				buy_node( const value_type& _Val )
		{	
			Node* _Wherenode = node_alloc().allocate(1);
			return new (_Wherenode) Node(_Val);
		} // allocate a node with pointers, value, and color
		bool				reserve_for_insert(size_type n)
		{
			if ( n < m_max_load || n > max_size() ) 
				return false;

			size_type s = size();
			s = s + (s >> 1);				// 절반을 늘려봄..
			s = s > n ? s : n;				// 잘 안되면 n값대로 키움..
			rehash_impl(min_buckets_for_size(s));

			return true;
		}

		void	recompute_begin_bucket_insert( Bucket* pBucket)
		{
			if (pBucket < bucket_begin())
				bucket_begin() = pBucket;
		}
		
		void	increment_size(){	++num_size(); }
		
		size_type max_load() const { return m_max_load;  }

		value_type&			operator[](const key_type& key)
		{
			size_type	hashvalue	= hash_fun()(key);
			Bucket*		pBucket		= buckets() + hashvalue % num_bucket();
			Node*		pPos		= find_node(pBucket, key);
			
			if (!pPos)
			{
				pPos = make_init_node(key, node_alloc());		
				
				if (reserve_for_insert( size() + 1) )
					pBucket = buckets() + hashvalue % num_bucket();
				
				link_node( pPos, pBucket);  
			}

			return pPos->myValue;
		}
////		
		value_type&			at(const key_type& key) const
		{
			Bucket* pBucket = get_bucket(key);
			Node*	it = find_node(pBucket, key);
			return it->myValue;
		}

	private:	

		template<typename I>
		size_type			get_bucket_size_iterator_range(I i, I j, size_type numBucket )
		{
			return (std::max)( static_cast<size_type>( i3::distance(i, j)) + 1, numBucket );
		}
	

	
		void				calculate_max_load()
		{
			m_max_load = detail::double_to_size_t(ceil( (double) m_max_load_factor * (double) num_bucket() ) )  ;
		}
		

		template<class AnyKeyType>
		Node**				find_for_erase(Bucket* pBucket, const AnyKeyType& k) const
		{
			Node** ppNode = &pBucket->pHead;

			while( *ppNode && !equal_fun()( k, key(*ppNode) ) )
				ppNode = &next_group(*ppNode);

			return ppNode;
		}
		template<class AnyKeyType, class AnyEqualFun>
		Node**				find_for_erase(Bucket* pBucket, const AnyKeyType& k, AnyEqualFun any_equal_fun) const
		{
			Node** ppNode = &pBucket->pHead;

			while( *ppNode && !any_equal_fun( k, key(*ppNode) ) )
				ppNode = &next_group(*ppNode);

			return ppNode;
		}

		size_type			min_buckets_for_size( size_type n) const
		{
			return detail::double_to_size_t( floor( n / (double) m_max_load_factor) ) + 1;
		}
		
		void				rehash_impl(size_type n)
		{
			n = detail::next_prime(n);
			if ( n == num_bucket())
				return;
						 
			move_buckets( n );
			calculate_max_load();
		}
		
		void			allocate_new_buckets( Bucket*& pNewBuckets, Bucket*& pNewBucket_Begin, size_type numBucket_New ) 
		{
			pNewBuckets	= bucket_alloc().allocate( numBucket_New +  1 );
			pNewBucket_Begin = pNewBuckets + numBucket_New;	
			memset( pNewBuckets, 0 ,sizeof(Bucket) * numBucket_New  );

			pNewBucket_Begin->pHead = reinterpret_cast<Node*>(pNewBucket_Begin);
		
		}

		void			change_into_new_buckets( Bucket* pNewBuckets, Bucket* pNewBucket_Begin, size_type numBucket_New )
		{
			bucket_alloc().deallocate( buckets(), num_bucket() + 1);
			num_bucket() = numBucket_New;
			buckets()	= pNewBuckets;
			bucket_begin() = pNewBucket_Begin;
		}

		void				move_buckets(size_type numBucket_New)
		{
			Bucket*		pNewBuckets	;		// 버켓수보다 하나 더..// POD이므로 생성자호출을 생략했다.
			Bucket*		pNewBucket_Begin ;	
			allocate_new_buckets(pNewBuckets, pNewBucket_Begin, numBucket_New);

			Bucket*  pSrcEnd = buckets() + num_bucket();

			for ( Bucket*  pSrcBucket = bucket_begin(); pSrcBucket != pSrcEnd ; ++pSrcBucket )
			{
				Node* pNode = pSrcBucket->pHead;

				while ( pNode )
				{
					Bucket*		pDestBucket = pNewBuckets + hash_fun()( key(pNode) ) % numBucket_New;	// 새 버킷..
					Node*		pNext = next_group(pNode);

					link_group( pNode, pDestBucket );			// 여기서 링크가 바뀌는데..
					
					if ( pDestBucket < pNewBucket_Begin ) 
						pNewBucket_Begin = pDestBucket;
					
					pNode =	pNext; //next_group(pNode);					// 바뀐 링크를 갖고 자꾸 찾음..
				}
			}
			
			change_into_new_buckets( pNewBuckets, pNewBucket_Begin, numBucket_New);

			// num_size()의 변동은 없을 것이라고 생각..
		}
			
		void	copy_buckets_to(Bucket*& pNewBuckets, Bucket*& pNewBucket_Begin,size_type numBucket_New, hash_table& other) const
		{
			
//			other.allocate_new_buckets(pNewBuckets, pNewBucket_Begin, numBucket_New);

			Bucket*  pSrcEnd = buckets() + num_bucket();
			
			for ( Bucket* i = bucket_begin() ; i != pSrcEnd ; ++i )
			{
				for ( Node* it = i->pHead ; it ; it = next_group(it) )
				{
					Bucket* pDestBucket = pNewBuckets + hash_fun()( key( it ) ) % numBucket_New;
					
					other.copy_group( it, pDestBucket, other.node_alloc() );
					
					if ( pDestBucket < pNewBucket_Begin )
						pNewBucket_Begin = pDestBucket;
				}
			}
		}

		void	recompute_begin_bucket_erase(Bucket* pBucket)
		{
			if (pBucket == bucket_begin())
			{
				if ( num_size() != 0 )
					while ( bucket_begin()->pHead == nullptr)
						++bucket_begin();
				else
					bucket_begin() = buckets() + static_cast<difference_type>(num_bucket());
			}

		}

		void	recompute_begin_bucket_erase(Bucket* pBucket1, Bucket* pBucket2)
		{
			if ( pBucket1 == bucket_begin() && pBucket1->pHead == nullptr)
				bucket_begin() = pBucket2;
		}
	
		size_type unlink_nodes( const_iterator n)
		{
			Node** ppNode = get_for_erase( n.get_node(), n.get_bucket() );
			split_group(*ppNode);
			*ppNode = nullptr;				// 인자노드로부터 끝까지 지우므로..
			return node_count(n.get_node());
		}

		size_type unlink_nodes(const_iterator begin, const_iterator end)
		{
			assert(begin.get_bucket() == end.get_bucket() );

			size_type numDel = node_count(begin.get_node(), end.get_node() );

			Node** ppNode = get_for_erase( begin.get_node(), begin.get_bucket() );
			split_group(*ppNode, end.get_node() );
			*ppNode = end.get_node() ;

			return numDel;
		}

		size_type unlink_nodes(Bucket* base, const_iterator end)		// 같은 버킷 시작부터 끝까지..
		{
			split_group(end.get_node());

			Node* ptr(base->pHead);
			base->pHead = end.get_node();
				
			return node_count( ptr, end.get_node() );
		}

		void	delete_to_bucket_end(Node* begin)
		{
			while (begin)
			{
				Node* pNode = begin;
				begin = begin->pNext;
				node_alloc().destroy(pNode);
				node_alloc().deallocate(pNode, 1);
			}
		}
		
		void	clear_bucket(Bucket* b)
		{
			Node* first_node = b->pHead;
			b->pHead = nullptr;
			delete_to_bucket_end(first_node);
		}
		
		// EBO 최적화를 위해 아래와 같이 배치..하고 멤버함수를 만들것..
		compressed_pair<node_allocator_type, Bucket*>	m_node_alloc_and_buckets;
		compressed_pair<bucket_allocator_type, Bucket*>	m_bucket_alloc_and_bucket_begin;
		compressed_pair<hasher, size_type>				m_hash_function_and_num_bucket;
		compressed_pair<key_equal, size_type>			m_equal_function_and_num_size;
		
		__forceinline const node_allocator_type&	node_alloc() const { return m_node_alloc_and_buckets.first(); }
		__forceinline node_allocator_type&			node_alloc() { return m_node_alloc_and_buckets.first(); }
		__forceinline Bucket* const &				buckets() const { return m_node_alloc_and_buckets.second(); }
		__forceinline Bucket*&						buckets()		{ return m_node_alloc_and_buckets.second(); }
		
		__forceinline const bucket_allocator_type&	bucket_alloc() const { return m_bucket_alloc_and_bucket_begin.first(); }
		__forceinline bucket_allocator_type&		bucket_alloc() { return m_bucket_alloc_and_bucket_begin.first(); }
		__forceinline Bucket* const &				bucket_begin() const { return m_bucket_alloc_and_bucket_begin.second(); }
		__forceinline Bucket*&						bucket_begin() { return m_bucket_alloc_and_bucket_begin.second(); }
		
		__forceinline const hasher&					hash_fun() const  {  return m_hash_function_and_num_bucket.first();  }
		__forceinline hasher&						hash_fun()		{  return m_hash_function_and_num_bucket.first();  }
		__forceinline const size_t&					num_bucket() const { return m_hash_function_and_num_bucket.second(); }
		__forceinline size_t&						num_bucket()		{ return m_hash_function_and_num_bucket.second(); }

		__forceinline const key_equal&				equal_fun() const	{ return m_equal_function_and_num_size.first(); }
		__forceinline key_equal&					equal_fun()			{ return m_equal_function_and_num_size.first(); }
		__forceinline const size_t&					num_size() const { return m_equal_function_and_num_size.second(); }
		__forceinline size_t&						num_size()		{ return m_equal_function_and_num_size.second(); }

//      다음의 멤버함수들이 폐기되고, EBO최적화로 변경..
//		node_allocator_type			m_node_allocator;
//		bucket_allocator_type		m_bucket_allocator;
//		hasher						m_hash_function;
//		key_equal					m_key_function;
//		Bucket*						m_Buckets;
//		Bucket*						m_pBucket_Begin;
//		size_type					m_numBucket;
//		size_type					m_numSize;
	
		float						m_max_load_factor;
		size_type					m_max_load;
		
	};

	
	template < class HashTableNode, class Hash, class Pred >
	class hash_table_ext;
	
	template < class Value , class Alloc, class Hash, class Pred >
	class hash_table_ext< HashTableNode_Unique<Value, Alloc>, Hash, Pred > : 
		public hash_table< HashTableNode_Unique<Value, Alloc>, Hash, Pred >
	{
		typedef hash_table< HashTableNode_Unique<Value, Alloc>, Hash, Pred > base_type;
	public:
		typedef detail::unordered_insert_commit_data						insert_commit_data;	

		hash_table_ext( size_type numBucket, const hasher& hf, const key_equal& eq, const allocator_type& a) 
			: base_type(numBucket, hf, eq, a) {}
		
		hash_table_ext(const hash_table_ext& other, const allocator_type& a) : base_type(other, a) {}

		template < class InputIterator>
		hash_table_ext(InputIterator f, InputIterator l, size_type numBucket, 
					const hasher& hf, const key_equal& eq, const allocator_type& a) : 
			base_type(f, l, numBucket, hf, eq, a) 
		{
			insert(f, l);
		}

		i3::pair<iterator, bool> insert_check(const key_type& k, insert_commit_data& out_data)
		{
			out_data.hash = hash_function(k);
			Bucket* pBucket = get_bucket_from_hash_value(out_data.hash);
			Node* pNode = find_node(pBucket, k);
			if (pNode)
				return i3::pair<iterator, bool>(iterator(pBucket, pNode), false);
			return i3::pair<iterator, bool>( end(), true);
		}
		
		template<class AnyKeyType>
		i3::pair<iterator, bool> insert_check_other(const AnyKeyType& k, insert_commit_data& out_data)
		{
			out_data.hash = hash_function(k);
			Bucket* pBucket = get_bucket_from_hash_value(out_data.hash);
			Node* pNode = find_node(pBucket, k);
			if (pNode)
				return i3::pair<iterator, bool>(iterator(pBucket, pNode), false);
			return i3::pair<iterator, bool>( end(), true);
		}

		template<class AnyKeyType, class AnyHashFun, class AnyEqualFun>
		i3::pair<iterator, bool> insert_check_other(const AnyKeyType& k, AnyHashFun any_hash_fun, AnyEqualFun any_equal_fun, insert_commit_data& out_data)
		{	
			out_data.hash = any_hash_fun(k);
			Bucket* pBucket = get_bucket_from_hash_value(out_data.hash);
			Node* pNode = find_node(pBucket, k, any_equal_fun);
			if (pNode)
				return i3::pair<iterator, bool>(iterator(pBucket, pNode), false);
			return i3::pair<iterator, bool>( end(), true);
		}
		
		iterator insert_commit(const value_type& val, const insert_commit_data& in_data)
		{
			Node* pNode = buy_node(val);
			reserve_for_insert( size() + 1 );
			Bucket* pBucket = get_bucket_from_hash_value(in_data.hash);
			link_node(pNode, pBucket);
			recompute_begin_bucket_insert( pBucket );
			increment_size();
			return iterator(pBucket, pNode);
		}

		i3::pair<iterator, bool> insert(const value_type& val)
		{
			const key_type& k = key( val);
			size_type	hash_val = hash_function(k);
			Bucket*		pBucket = get_bucket_from_hash_value(hash_val);
			Node*		pNode   = find_node(pBucket, k);
			
			if ( pNode )
				return i3::pair< iterator, bool>( iterator( pBucket, pNode ), false );
			
			pNode = buy_node( val );		
			// 새로 넣기..
			
			if (  reserve_for_insert( size() + 1 ) )
				pBucket = get_bucket_from_hash_value(hash_val);

			link_node(pNode, pBucket);
			
			recompute_begin_bucket_insert( pBucket );

			increment_size();
			
			return 	i3::pair< iterator, bool>( iterator(pBucket, pNode), true);		
		}

		const_iterator insert(const_iterator hint, const value_type& val)		// 힌트버전
		{	
			if ( hint != end() && key_function( key(val), key(*hint) )  )	// 같으면 그냥 토함..
				return hint;
			return insert(val).first;
		}
		
		iterator insert(iterator hint, const value_type& val)		// 힌트버전
		{	
			if ( hint != end() && key_function( key(val), key(*hint) )  )	// 같으면 그냥 토함..
				return hint;
			return insert(val).first;
		}
		
		template <typename InputIterator>
		void	insert(InputIterator i, InputIterator j)
		{

			for (; i != j; ++i) 
			{
				// No side effects in this initial code
				size_type hash_val = hash_function(key(*i));
				Bucket* bucket = get_bucket_from_hash_value(hash_val);
				Node* pos = find_node(bucket, key(*i));

				if (!pos )		// 못찾았다면...이미 있는 경우 인서트 스킵..
				{
					// Doesn't already exist, add to bucket.
					// Side effects only in this block.
					Node* pNewNode = buy_node(*i);
					
					if( size() + 1 >= max_load() ) 
					{
						if ( reserve_for_insert( size() + static_cast<size_type> (i3::distance(i, j) ) ) )
							bucket = get_bucket_from_hash_value(hash_val);
					}
					
					link_node( pNewNode, bucket);  

					recompute_begin_bucket_insert(bucket);

					increment_size();
					
				}
			}
		}
		
		
	};
	
	template < class Value , class Alloc, class Hash, class Pred>
	class hash_table_ext< HashTableNode_Multi<Value, Alloc>, Hash, Pred> : 
		public hash_table< HashTableNode_Multi<Value, Alloc>, Hash, Pred >
	{
		typedef hash_table< HashTableNode_Multi<Value, Alloc>, Hash, Pred >	base_type;

	public:
		hash_table_ext( size_type numBucket, const hasher& hf, const key_equal& eq, const allocator_type& a) 
			: base_type(numBucket, hf, eq, a) {}
		
		hash_table_ext(const hash_table_ext& other, const allocator_type& a) : base_type(other, a) {}

		template < class InputIterator>
		hash_table_ext(InputIterator f, InputIterator l, size_type numBucket, 
			const hasher& hf, const key_equal& eq, const allocator_type& a) : base_type(f, l, numBucket, hf, eq, a) 
		{
			insert(f, l);
		}

		iterator insert(const value_type& val)			// 유닉맵과는 리턴값이 다름..
		{
			Node* pNode = buy_node(val);
			return insert_impl(pNode);
		}

		iterator insert(const_iterator hint, const value_type& val) {	return insert_hint_impl( hint, buy_node(val) );   }
		iterator insert( iterator hint, const value_type& val)	{	return insert_hint_impl( hint, buy_node(val) );   }
		
		template <class InputIterator> 
		void insert(InputIterator first, InputIterator last)
		{
			typename i3::iterator_traits<InputIterator>::iterator_category tag;
			insert_for_range(first, last, tag);
		}

	

	private:

	template <class InputIterator> 
		void insert_for_range(InputIterator first, InputIterator last, i3::forward_iterator_tag )
		{
			size_type distance = static_cast<size_type>(i3::distance(first, last) );
			if(distance == 1) 
			{
				insert(*first);
			}
			else 
			{
				reserve_for_insert(size() + distance);

				for (; first != last; ++first ) 
				{
					Node* pNewNode = buy_node(*first);
					
					key_type const& k = key(pNewNode);
					Bucket* bucket = get_bucket(k);
										
					Node* position = find_node(bucket, k);

					if( position)
						link_node( pNewNode, position);
					else
					{
						link_node( pNewNode, bucket);
						
						recompute_begin_bucket_insert(bucket);
					}
					increment_size();
				}
			}
					
		}

		template <class InputIterator> 
		void insert_for_range(InputIterator first, InputIterator last, i3::input_iterator_tag )
		{
			for (; first != last; ++first) 
				insert(*first);
		}

		iterator					insert_impl(Node* pNewNode)
		{
			const key_type& k = key( pNewNode );
			size_type	hash_val = hash_function(k);
			Bucket*		pBucket = get_bucket_from_hash_value(hash_val);

			Node*		pNode   = find_node(pBucket, k);

			if (  reserve_for_insert( size() + 1 ) )
				pBucket = get_bucket_from_hash_value(hash_val);


			if ( pNode )
			{
				link_node( pNewNode, pNode );		// 비긴포지션이 바뀌진 않을 것임..(같은노드중에 뒤로 붙음)
			}
			else
			{
				link_node( pNewNode, pBucket);		// 버킷중에서 앞으로 붙음...

				recompute_begin_bucket_insert(pBucket);
			}

			increment_size(); //			1개 증가를 여기에 등록..

			return iterator( pBucket, pNewNode );
		}

		iterator	insert_hint_impl(const const_iterator& hint, Node* pNewNode)
		{
			if ( hint == end() || !key_function( key(pNewNode), key(*hint )  ) )		// 힌트가 무효화..
				return insert_impl(pNewNode);

			Node* pNode = hint.get_node();
			while ( pNode->pPrev_InGroup->pNext == pNode )
				pNode = pNode->pPrev_InGroup;						// 맨 앞노드로 전진..

			Bucket* pBucket;
			if ( reserve_for_insert(size() + 1) )
				pBucket = get_bucket(key(pNewNode));
			else
				pBucket = hint.get_bucket();

			link_node( pNewNode, pNode );
			increment_size();

			return iterator( pBucket, pNewNode );
		}



	};



}

#pragma warning(pop)

#pragma pack(pop)