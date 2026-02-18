#pragma once

#include "list_fwd.h"

#include "../pool/pool_allocator.h"
#include "memory.h"
#include "compressed_pair.h"
#include "algorithm/equal.h"
#include "algorithm/lexicographical_compare.h"

#pragma pack(push, _CRT_PACKING)

#pragma push_macro("new")
#undef new

namespace i3
{

	template<class T, class Alloc>
	class list 
	{
	public:
		typedef list<T, Alloc>									my_type;

		typedef typename Alloc::template rebind<T>::other		allocator_type;
		typedef typename allocator_type::size_type				size_type;
		typedef typename allocator_type::difference_type		difference_type;
		typedef typename allocator_type::pointer				pointer;
		typedef typename allocator_type::const_pointer			const_pointer;
		typedef typename allocator_type::reference				reference;
		typedef typename allocator_type::const_reference		const_reference;
		typedef typename allocator_type::value_type				value_type;
		
		#pragma warning(push)
		#pragma warning(disable:4512)
		struct node 
		{	
			node( node* Next,node* Prev,const T& val) : pNext(Next), pPrev(Prev), Val(val) {}
			node* pNext;	node* pPrev;	T	  Val;	
		};
		#pragma warning(pop)

		typedef typename Alloc::template rebind<node>::other	node_allocator_type;	

		class const_iterator : public i3::iterator< bidirectional_iterator_tag, value_type, difference_type, const_pointer, const_reference>
		{	
		public:
			typedef const_pointer	pointer;
			typedef const_reference reference;
			const_iterator() : ptr(0){}
			explicit const_iterator(node* pnode) : ptr(pnode) {}

			const_reference operator*() const  { return ptr->Val; }
			const_pointer	operator->() const { return (&**this); }

			const_iterator& operator++() {	ptr = ptr->pNext;	return *this; }
			const_iterator	operator++(int)	{	const_iterator _Tmp = *this;	++*this;	return _Tmp;  }
			const_iterator& operator--() {	ptr = ptr->pPrev;	return *this; }
			const_iterator operator--(int) {  const_iterator _Tmp = *this;  --*this; return _Tmp; }
			bool operator==(const const_iterator& Right) const 	{	return (ptr == Right.ptr);	}
			bool operator!=(const const_iterator& Right) const	{	return (!(*this == Right));	}
			node* get_node() const {	return ptr;	}

		private:
			node* ptr;	
		};
		
		class iterator : public const_iterator
		{	
		public:
			typedef typename allocator_type::pointer			pointer;
			typedef typename allocator_type::reference			reference;

			iterator(){}
			explicit iterator(node* pnode)	: const_iterator(pnode)	{}
			reference operator*() const {	return ((reference)**(const_iterator *)this);	}
			pointer operator->() const	{	return (&**this);	}
			iterator& operator++()	{	++(*(const_iterator *)this);	return (*this);	}
			iterator operator++(int){	iterator _Tmp = *this;	++*this;	return _Tmp;	}
			iterator& operator--()	{	--(*(const_iterator *)this);	return (*this);	}
			iterator operator--(int){	iterator _Tmp = *this;	--*this;	return _Tmp;	}
		};
		
		typedef i3::reverse_iterator<iterator>					reverse_iterator;
		typedef i3::reverse_iterator<const_iterator>			const_reverse_iterator;	
		
		list() : m_alloc_and_head(buy_node()), m_size(0) { }
		explicit list(const Alloc& Al) : m_alloc_and_head(Al, buy_node()), m_size(0) {}
		explicit list(size_type count) : m_alloc_and_head(buy_node()), m_size(0) { construct_n(count, T() );	}
		list(size_type count, const T& Val) : m_alloc_and_head(buy_node()), m_size(0) { construct_n(count, Val); }
		list(size_type count, const T& Val, const Alloc& Al) : m_alloc_and_head(Al, buy_node()), m_size(0) {	construct_n(count, Val); }
		list(const my_type& Right) : m_alloc_and_head(Right.node_alloc(), buy_node()), m_size(0) { insert(begin(), Right.begin(), Right.end()); }
		template<class Iter>
		list(Iter First, Iter Last) : m_alloc_and_head(buy_node()), m_size(0) {	construct(First, Last, typename i3::iterator_traits<Iter>::iterator_category() );	}
		template<class Iter>
		list(Iter First, Iter Last, const Alloc& Al) : m_alloc_and_head(Al, buy_node()), m_size(0) {	construct(First, Last, typename i3::iterator_traits<Iter>::iterator_category() );	}

		~list() 
		{  
			node* next = 0;
			for (node* pnode = head()->pNext ; pnode != head(); pnode = next )
			{	
				next = pnode->pNext;
				node_alloc().destroy(pnode);		node_alloc().deallocate(pnode, 1);
			}
			node_alloc().deallocate(head(), 1);
		}
		
		my_type&	operator=(const my_type& Right) {  if (this != &Right) assign(Right.begin(), Right.end() );  return *this;  }


		iterator		begin() {	return iterator(head()->pNext);	}		
		const_iterator	begin() const {	return const_iterator(head()->pNext ); }
		iterator		end() {	return iterator(head()); }
		const_iterator	end() const {	return const_iterator(head());	}

		reverse_iterator rbegin() {	return reverse_iterator(end()); }
		const_reverse_iterator rbegin() const {	return const_reverse_iterator(end()); }
		reverse_iterator rend()	{	return reverse_iterator(begin()); }
		const_reverse_iterator rend() const {	return const_reverse_iterator(begin()); }
		
		void			resize(size_type new_size) { resize(new_size, T()); }
		void			resize(size_type new_size, T Val) 
		{
			if (m_size < new_size )
				insert_impl_n(end(), new_size - m_size, Val);
			else
				while ( new_size < m_size ) pop_back();
		}
		
		size_type		size() const { return m_size; }
		size_type		max_size() const { return node_alloc().max_size();  }
		bool			empty() const {	return m_size == 0;	}
		allocator_type	get_allocator() const {	return node_alloc(); }

		reference		front()			{	return *begin();	}
		const_reference front() const	{	return *begin();	}
		reference		back()			{	return *(--end());	}
		const_reference back() const	{	return *(--end());  }
		void			push_front(const T& Val) {	insert_impl(begin(), Val);	}
		void			pop_front() {	erase(begin());	}
		void			push_back(const T& Val) {	insert_impl(end(), Val);  }
		void			pop_back() {	erase(--end()); }

		template<class Iter>
		void assign(Iter First, Iter Last) {  assign_impl(First, Last, typename i3::iterator_traits<Iter>::iterator_category() );  }
		void assign(size_type count, const T& Val) {	assign_impl_n(count, Val);	}

		iterator insert(const_iterator where, const T& Val) {	insert_impl(where, Val);	return cast_to_iterator(--where);	}
		void	insert(const_iterator where, size_type count, const T& Val)	{	insert_impl_n(where, count, Val);	}
		template<class Iter>
		void insert(const_iterator where, Iter First, Iter Last) {	insert_impl(where, First, Last, typename i3::iterator_traits<Iter>::iterator_category() ); }
		
		iterator erase(const_iterator where)
		{	
			node* pnode = where.get_node(); 
			++where;

			pnode->pPrev->pNext = pnode->pNext;
			pnode->pNext->pPrev = pnode->pPrev;
			node_alloc().destroy(pnode);
			node_alloc().deallocate(pnode, 1);
			--m_size;

			return cast_to_iterator(where);
		}

		iterator erase(const_iterator First, const_iterator Last)
		{	
			if (First == begin() && Last == end())
			{	
				clear();	return (end());
			}
			while (First != Last)
				First = erase(First);
			return cast_to_iterator(Last);
		}

		void			clear()
		{	
			if ( empty() ) return;
			node* pnode = head()->pNext; 
			head()->pNext = head();
			head()->pPrev = head(); 
			m_size = 0;
			
			node* next = 0;

			for (; pnode != head() ; pnode = next )
			{	
				next = pnode->pNext;
				node_alloc().destroy(pnode);
				node_alloc().deallocate(pnode, 1);
			}
		}

		void swap(my_type& Right)
		{	
			if (this == &Right) return;
			
			if (this->node_alloc() == Right.node_alloc())
			{	
				i3::swap(head(), Right.head());
				i3::swap(m_size, Right.m_size);
			}
			else
			{	
				iterator where = begin();
				splice(where, Right);
				Right.splice(Right.begin(), *this, where, end() );
			}
		}

		void splice(const_iterator where, my_type& Right)// splice all of _Right at _Where
		{	
			if (this != &Right && !Right.empty())// worth splicing, do it
			{	
				splice_impl(where, Right, Right.begin(), Right.end(), Right.m_size);
			}
		}

		void splice(const_iterator where, my_type& Right, const_iterator First)
		{	// splice _Right [_First, _First + 1) at _Where

			if (First != Right.end())
			{	// element exists, try splice
				const_iterator Last = First;
				++Last;
				if (this != &Right	|| (where != First && where != Last))
					splice_impl(where, Right, First, Last, 1);
			}
		}

		void splice(const_iterator where, my_type& Right, const_iterator First, const_iterator Last)
		{	// splice _Right [_First, _Last) at _Where
			if (First != Last && (this != &Right || where != Last))
			{	// worth splicing, do it
				size_type count = 0;
				if (this == &Right)
					;	// just rearrange this list
				else if (First == Right.begin() && Last == Right.end())
					count = Right.m_size;	// splice in whole list
				else
				{	// count nodes and check for knot
					const_iterator Next = First;
					for (; Next != Last; ++Next, ++count);
				}
				splice_impl(where, Right, First, Last, count);
			}
		}
		
		void remove(const T& Val_arg)
		{	// erase each element matching _Val
			const T Val = Val_arg;	// in case it's removed along the way
			iterator Last = end();
			for (iterator First = begin(); First != Last; )
				if (*First == Val)
					First = erase(First);
				else
					++First;
		}

		template<class Pr1>
		void remove_if(Pr1 Pred)
		{	// erase each element satisfying _Pr1
			iterator Last = end();
			for (iterator First = begin(); First != Last; )
				if (Pred(*First))
					First = erase(First);
				else
					++First;
		}
		
		void unique()
		{	// erase each element matching previous
			if (2 <= m_size)
			{	// worth doing
				iterator First = begin();
				iterator After = First;
				for (++After; After != end(); )
					if (*First == *After)
						After = erase(After);
					else
						First = After++;
			}
		}
		
		template<class Pr2>
		void unique(Pr2 Pred)
		{	// erase each element satisfying _Pred with previous
			if (2 <= m_size)
			{	// worth doing
				iterator First = begin();
				iterator After = First;
				for (++After; After != end(); )
					if (Pred(*First, *After))
						After = erase(After);
					else
						First = After++;
			}
		}
		
		void merge(my_type& Right)
		{	// merge in elements from _Right, both ordered by operator<
			if (&Right != this)
			{	// safe to merge, do it
				iterator First1 = begin(), Last1 = end();
				iterator First2 = Right.begin(), Last2 = Right.end();

				while (First1 != Last1 && First2 != Last2)
					if ( *First2 < *First1 )
					{	// splice in an element from _Right
						iterator Mid2 = First2;
						splice_impl(First1, Right, First2, ++Mid2, 1);
						First2 = Mid2;
					}
					else
						++First1;

				if (First2 != Last2)
					splice_impl(Last1, Right, First2, Last2, Right.m_size);	// splice remainder of _Right
			}
		}

		template<class Pr3>
		void merge(my_type& Right, Pr3 Pred)
		{	// merge in elements from _Right, both ordered by _Pred
			if (&Right != this)
			{	// safe to merge, do it
				iterator First1 = begin(), Last1 = end();
				iterator First2 = Right.begin(), Last2 = Right.end();
				
				while (First1 != Last1 && First2 != Last2)
					if ( Pred( *First2, *First1) )
					{	// splice in an element from _Right
						iterator Mid2 = First2;
						splice_impl(First1, Right, First2, ++Mid2, 1);
						First2 = Mid2;
					}
					else
						++First1;

				if (First2 != Last2)
					splice_impl(Last1, Right, First2, Last2, Right.m_size);	// splice remainder of _Right
			}
		}

		void sort()
		{	// order sequence, using operator<
			if (2 <= m_size)
			{	// worth sorting, do it
				const size_t MAXBINS = 25;
				my_type Templist(get_allocator()), Binlist[MAXBINS + 1];
				size_t Maxbin = 0;

				while (!empty())
				{	// sort another element, using bins
					Templist.splice_impl(Templist.begin(), *this, begin(), ++begin(), 1 );	// don't invalidate iterators

					size_t Bin;
					for (Bin = 0; Bin < Maxbin && !Binlist[Bin].empty(); ++Bin)
					{	// merge into ever larger bins
						Binlist[Bin].merge(Templist);
						Binlist[Bin].swap(Templist);
					}

					if (Bin == MAXBINS)
						Binlist[Bin - 1].merge(Templist);
					else
					{	// spill to new bin, while they last
						Binlist[Bin].swap(Templist);
						if (Bin == Maxbin)
							++Maxbin;
					}
				}

				for (size_t Bin = 1; Bin < Maxbin; ++Bin)
					Binlist[Bin].merge(Binlist[Bin - 1]);	// merge up
				splice(begin(), Binlist[Maxbin - 1]);	// result in last bin
			}
		}

		template<class Pr3>
		void sort(Pr3 Pred)
		{	// order sequence, using _Pred
			if (2 <= m_size)
			{	// worth sorting, do it
				const size_t MAXBINS = 25;
				my_type Templist(get_allocator()), Binlist[MAXBINS + 1];
				size_t Maxbin = 0;

				while (!empty())
				{	// sort another element, using bins
					Templist.splice_impl(Templist.begin(), *this, begin(),	++begin(), 1);	// don't invalidate iterators

					size_t Bin;
					for (Bin = 0; Bin < Maxbin && !Binlist[Bin].empty(); ++Bin)
					{	// merge into ever larger bins
						Binlist[Bin].merge(Templist, Pred);
						Binlist[Bin].swap(Templist);
					}

					if (Bin == MAXBINS)
						Binlist[Bin - 1].merge(Templist, Pred);
					else
					{	// spill to new bin, while they last
						Binlist[Bin].swap(Templist);
						if (Bin == Maxbin)
							++Maxbin;
					}
				}

				for (size_t Bin = 1; Bin < Maxbin; ++Bin)
					Binlist[Bin].merge(Binlist[Bin - 1], Pred);	// merge up
				splice(begin(), Binlist[Maxbin - 1]);	// result in last bin
			}
		}

		void reverse()
		{	// reverse sequence
			if (2 <= m_size)
			{	// worth doing
				iterator Last = end();
				for (iterator Next = ++begin(); Next != Last; )
				{	// move next element to beginning
					iterator Before = Next;
					splice_impl(begin(), *this, Before, ++Next, 1);
				}
			}
		}

	private:
		iterator		cast_to_iterator(const_iterator where) { return iterator( where.get_node() );  }
		
		node*	buy_node() {	node* pnode = node_alloc().allocate(1);		pnode->pNext = pnode;	pnode->pPrev = pnode;	return pnode;	}

		node*	buy_node(node* Next, node* Prev, const T& Val)
		{	
			return new ( node_alloc().allocate(1) ) node( Next, Prev, Val);
		}

		void	construct_n(size_type count, const T& Val) {	insert_impl_n(begin(), count, Val);	}

		template<class Iter>
		void construct(Iter count, Iter Val, integral_iterator_tag) {	construct_n((size_type)count, (T)Val); }

		template<class Iter>
		void construct(Iter First, Iter Last, input_iterator_tag tag) {	 insert_impl(begin(), First, Last, tag);	}

			
		void insert_impl_n(const_iterator where, size_type count, const T& Val)
		{	
			for (; 0 < count; --count)
				insert_impl(where, Val);
		}
		void insert_impl(const_iterator where, const T& Val)
		{	
			node* pnode = where.get_node(); 	node* newnode = buy_node(pnode, pnode->pPrev, Val);
			++m_size;		pnode->pPrev = newnode;		newnode->pPrev->pNext = newnode;
		}
		
		template<class Iter>
		void insert_impl(const_iterator where, Iter Count, Iter Val, integral_iterator_tag) {	insert_impl_n(where, (size_type)Count, (T)Val); }

		template<class Iter>
		void insert_impl(const_iterator where, Iter First, Iter Last, input_iterator_tag) {	 for (; First != Last; ++First ) insert_impl(where, *First); }

		template<class Iter>
		void assign_impl(Iter count, Iter Val, integral_iterator_tag) {	 assign_impl_n((size_type)count, (T)Val);	}

		template<class Iter>
		void assign_impl(Iter First, Iter Last, input_iterator_tag tag) 
		{	
			iterator my_f = begin();	iterator my_l = end();
			
			for ( ; my_f != my_l && First != Last ; ++my_f, ++First)
				*my_f = *First;

			if ( First == Last )
				erase( my_f, my_l);
			else 
				insert( my_l, First, Last);
			// 구현 변경..(2014.08.07.수빈) ms스타일 버리고, gcc스타일로..
			// clear();	insert_impl(begin(), First, Last, tag);			==> 구버전 코드
		}

		void assign_impl_n(size_type count, const T& Val)
		{	
			iterator my_f = begin();	iterator my_l = end();
			
			for ( ; my_f != my_l && count > 0 ; ++my_f, --count)
				*my_f = Val;
			
			if ( count > 0 )
				insert(my_l, count, Val);
			else
				erase( my_f, my_l);

			//  구현 변경..(2014.08.07.수빈) ms스타일 버리고, gcc스타일로..
			//	clear();	insert_impl_n(begin(), count, _Tmp);		==> 구버전 코드
		}

		void splice_impl(const_iterator where, my_type& Right, const_iterator First, const_iterator Last, size_type count)
		{	// splice _Right [_First, _Last) before _Where

			if (this->node_alloc() == Right.node_alloc())
			{	// same allocator, just relink
				if (this != &Right)
				{	// splicing from another list, adjust counts
					m_size += count;
					Right.m_size -= count;
				}
				
				First.get_node()->pPrev->pNext = Last.get_node();
				Last.get_node()->pPrev->pNext  = where.get_node();
				where.get_node()->pPrev->pNext = First.get_node();
				
				node* pnode = where.get_node()->pPrev;
				where.get_node()->pPrev = Last.get_node()->pPrev;
				Last.get_node()->pPrev  = First.get_node()->pPrev;
				First.get_node()->pPrev = pnode;
			}
			else
			{	// different allocator, copy nodes then erase source
				insert(where, First, Last);
				Right.erase(First, Last);
			}
		}
		
		compressed_pair<node_allocator_type, node*>				m_alloc_and_head;

		__forceinline const node_allocator_type&		node_alloc() const { return	m_alloc_and_head.first(); }
		__forceinline node_allocator_type&				node_alloc()  { return	m_alloc_and_head.first(); }
		__forceinline node* const &						head() const { return	m_alloc_and_head.second(); }
		__forceinline node*&							head()  { return	m_alloc_and_head.second(); }

		size_type												m_size;
	};

	template<class T, class A> inline
	void swap(list<T, A>& Left, list<T, A>& Right) {	Left.swap(Right);  }

	template<class T, class A> inline
	bool operator==(const list<T, A>& Left,	const list<T, A>& Right)
	{	
		return Left.size() == Right.size()	&& equal(Left.begin(), Left.end(), Right.begin());
	}

	template<class T, class A> inline
	bool operator!=(const list<T, A>& Left,	const list<T, A>& Right) {	return !(Left == Right);	}

	template<class T, class A> inline
	bool operator<(const list<T, A>& Left,	const list<T, A>& Right) {	return lexicographical_compare(Left.begin(), Left.end(), Right.begin(), Right.end()); }

	template<class T, class A> inline
	bool operator>(const list<T, A>& Left,	const list<T, A>& Right) {	return Right < Left; }

	template<class T, class A> inline
	bool operator<=(const list<T, A>& Left,	const list<T, A>& Right) {	return !(Right < Left); }

	template<class T, class A> inline
	bool operator>=(const list<T, A>& Left,	const list<T, A>& Right) {	return !(Left < Right);	}


}

#pragma pop_macro("new")

#pragma pack(pop)