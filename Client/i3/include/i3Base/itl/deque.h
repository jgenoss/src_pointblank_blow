#pragma once

#include "deque_fwd.h"
#include "../pool/pool_allocator.h"
#include "memory.h"
#include "algorithm/equal.h"
#include "algorithm/copy.h"
#include "algorithm/copy_backward.h"
#include "algorithm/fill.h"
#include "algorithm/lexicographical_compare.h"

#pragma pack(push, _CRT_PACKING)

namespace i3
{

	template <class T, class Alloc>
	class deque : private enable_ebo< typename rebind_wrap<Alloc, T>::type > 
	{
	public:
		typedef deque<T, Alloc>									my_type;
		
		typedef enable_ebo< typename rebind_wrap<Alloc, T>::type >	allocator_base;	// value_allocator..

		typedef typename Alloc::template rebind<T>::other		allocator_type;
		typedef typename allocator_type::size_type				size_type;
		typedef typename allocator_type::difference_type		difference_type;
		typedef typename allocator_type::pointer				pointer;
		typedef typename allocator_type::const_pointer			const_pointer;
		typedef typename allocator_type::reference				reference;
		typedef typename allocator_type::const_reference		const_reference;
		typedef typename allocator_type::value_type				value_type;
		
		typedef typename Alloc::template rebind<pointer>::other pointer_allocator_type;
		typedef typename pointer_allocator_type::pointer		ppointer;

		enum 
		{
			deque_map_size = 8,
			deque_size = (	sizeof (T) <= 1 ? 16 : 
							sizeof (T) <= 2 ? 8  : 
							sizeof (T) <= 4 ? 4  :	
							sizeof (T) <= 8 ? 2  : 1),
			deque_size_m1 = deque_size - 1
		};
	
		class const_iterator : public i3::iterator< random_access_iterator_tag, value_type, difference_type, const_pointer, const_reference>
		{
		public:
			typedef const_pointer	pointer;
			typedef const_reference reference;
			typedef deque<T, Alloc>	container_type;

			const_iterator() : m_deque(nullptr), m_offset(0) {}
			const_iterator(size_type offset, const container_type* pdeque) : m_deque(pdeque), m_offset(offset) {}
			
			reference operator*() const 
			{
				size_type block = m_offset / deque_size;			// 몫.
				size_type offset = m_offset & deque_size_m1;		// 나머지.
				if ( block >= m_deque->m_map_size )					// VC구현의 deque는 순환된 메모리구조임..
					block -= m_deque->m_map_size;
				return m_deque->map_ptr()[block][offset];
			}
			pointer				operator->() const { return &**this; }
			const_iterator&		operator++() {   ++m_offset;  return *this;   }		
			const_iterator		operator++(int) { const_iterator tmp = *this;	++*this;	return tmp; }
			const_iterator&		operator--() {   --m_offset;  return *this;   }		
			const_iterator		operator--(int) {   const_iterator tmp = *this;	--*this;	return tmp; }
			
			const_iterator&		operator+=(difference_type off)	{	m_offset += off;	return *this;	}
			const_iterator		operator+(difference_type off) const {	const_iterator tmp = *this;		return tmp += off;	}
			const_iterator&		operator-=(difference_type off)	{	m_offset -= off;	return *this;	}
			const_iterator		operator-(difference_type off) const {	const_iterator tmp = *this;		return tmp -= off;	}

			difference_type		operator-(const const_iterator& rhs) const
			{	
				return static_cast<difference_type>(m_offset) - static_cast<difference_type>(rhs.m_offset);		// 기존것을 변경..
			}
			reference			operator[](difference_type off) const { return *(*this + off); }
			
			bool				operator==(const const_iterator& rhs) const { return m_offset == rhs.m_offset; }
			bool				operator!=(const const_iterator& rhs) const { return !(*this == rhs); }
			bool				operator<(const const_iterator& rhs) const 	{ return	m_offset < rhs.m_offset; }
			bool				operator>(const const_iterator& rhs) const	{ return rhs < *this; }
			bool				operator<=(const const_iterator& rhs) const { return !(rhs < *this); }
			bool				operator>=(const const_iterator& _Right) const { return !(*this < rhs);	}
			
			size_type			get_offset() const { return m_offset; }

		private:
			const container_type*	m_deque;
			size_type				m_offset;   
		};

		class iterator : public const_iterator
		{
		public:
			typedef typename allocator_type::pointer			pointer;
			typedef typename allocator_type::reference			reference;
			
			iterator(){}
			iterator(size_type off, const container_type* pdeque) : const_iterator(off, pdeque) {}

			reference	operator*() const {	return (reference)**(const_iterator*)this; }
			pointer		operator->() const 	{	return &**this; }
			iterator& operator++() {	++*(const_iterator *)this;		return *this;	}
			iterator operator++(int) {	iterator tmp = *this;	++*this;  return tmp;	}
			iterator& operator--() {	--*(const_iterator *)this;		return *this;	}
			iterator operator--(int) {	iterator tmp = *this;	--*this;	return tmp;	}
			iterator& operator+=(difference_type off) {	*(const_iterator *)this += off;		return *this;	}
			iterator operator+(difference_type off) const {	iterator tmp = *this;	return tmp += off;	}
			iterator& operator-=(difference_type off) {	return *this += -off;	}
			iterator operator-(difference_type off) const { iterator tmp = *this;	return tmp -= off;	}
			difference_type operator-(const const_iterator& rhs) const { return *(const_iterator *)this - rhs; }
			reference operator[](difference_type off) const	{	return *(*this + off);	}
		};



		typedef i3::reverse_iterator<iterator>			reverse_iterator;
		typedef i3::reverse_iterator<const_iterator>	const_reverse_iterator;
		
		deque() : m_map_alloc_and_map(0), m_map_size(0), m_offset(0), m_size(0) {}
		explicit deque(const Alloc& al) : allocator_base(al), m_map_alloc_and_map(al, 0), m_map_size(0), m_offset(0), m_size(0) {}
		explicit deque(size_type count) : m_map_alloc_and_map(0), m_map_size(0), m_offset(0), m_size(0) { construct_n(count, T()); }
		deque(size_type count, const T& val) : m_map_alloc_and_map(0), m_map_size(0), m_offset(0), m_size(0) { construct_n(count, val); }
		deque(size_type count, const T& val, const Alloc& al) : allocator_base(al), m_map_alloc_and_map(al, 0), m_map_size(0), m_offset(0), m_size(0)
			{	construct_n(count, val);	}
			 
		deque(const my_type& rhs) :	allocator_base(rhs), m_map_alloc_and_map(rhs.map_alloc(), 0), m_map_size(0), m_offset(0), m_size(0)
			{	insert(begin(), rhs.begin(), rhs.end());	}
		
		template<class It> 
		deque(It First, It Last) :  m_map_alloc_and_map(0), m_map_size(0), m_offset(0), m_size(0) 
			{	construct(First, Last, typename i3::iterator_traits<It>::iterator_category() );	}			

		~deque() 
		{	
			clear(); 
			for (size_type Count = m_map_size; 0 < Count; )
			{	// free storage for a block and destroy pointer
				if ( *(map_ptr() + --Count) != 0 )
					alloc().deallocate(*(map_ptr() + Count), deque_size);
			}

			if (map_ptr())
				map_alloc().deallocate(map_ptr(), m_map_size);	// free storage for map
//			m_map_size = 0;
//			m_map = 0;			
		}

		my_type& operator=(const my_type& rhs)
		{	// assign _Right
			if (this == &rhs) ;
			else if (rhs.m_size == 0)
				clear();
			else if (rhs.m_size <= m_size)
			{	// new sequence not longer, assign elements and erase unused
				iterator Mid = i3::copy(rhs.begin(), rhs.end(), begin());
				erase(Mid, end());
			}
			else
			{	// new sequence longer, assign elements and append rest
				const_iterator Mid = rhs.begin() + m_size;
				i3::copy(rhs.begin(), Mid, begin());
				insert(end(), Mid, rhs.end());
			}
			return *this;
		}

		iterator		begin() {	return iterator(m_offset, this); }
		const_iterator	begin() const {	return const_iterator(m_offset, this); }
		iterator		end()	{	return iterator(m_offset + m_size, this);	}
		const_iterator	end()	const {	return const_iterator(m_offset + m_size, this);  }

		
		reverse_iterator		rbegin() {	return reverse_iterator(end());}
		const_reverse_iterator	rbegin() const {	return const_reverse_iterator(end());	}
		reverse_iterator		rend()	{	return reverse_iterator(begin());	}
		const_reverse_iterator	rend() const	{	return const_reverse_iterator(begin());	}

		void			resize(size_type Newsize) {	resize(Newsize, T()); }
		void			resize(size_type Newsize, T val)
		{	
			if (m_size < Newsize)
				insert_n(end(), Newsize - m_size, val);
			else if (Newsize < m_size)
				erase(begin() + Newsize, end());
		}
		size_type		size() const {	return m_size;	}
		bool			empty() const {	return m_size == 0;  }

		allocator_type get_allocator() const {	return alloc();	}
		
		const_reference at(size_type Pos) const	{	return *(begin() + Pos);	}
		reference		at(size_type Pos){	return *(begin() + Pos);	}
		const_reference operator[](size_type Pos) const	{	return *(begin() + Pos);  }
		reference		operator[](size_type Pos) {	return *(begin() + Pos);	}
		
		reference front() {	return *begin(); }
		const_reference front() const {	return *begin(); }
		reference back() {	return *(--end()); }
		const_reference back() const {	return *(--end()); }
	

		void			push_front(const T& val)
		{	
			if ( m_offset % deque_size == 0 && m_map_size <= (m_size + deque_size)/deque_size )
				grow_map_1();

			size_type Newoff = (m_offset != 0) ? m_offset : m_map_size * deque_size;
			size_type Block = --Newoff / deque_size;
			if ( map_ptr()[Block] == 0 )		// 널값검사..
				map_ptr()[Block] = alloc().allocate(deque_size);

			alloc().construct(map_ptr()[Block] + Newoff % deque_size, val);
			m_offset = Newoff;
			++m_size;
		}
		
		void			push_back(const T& val)
		{	// insert element at end

			if ((m_offset + m_size) % deque_size == 0  && m_map_size <= (m_size + deque_size)/deque_size )
				grow_map_1();

			size_type Newoff = m_offset + m_size;
			size_type Block = Newoff / deque_size;
			if (m_map_size <= Block)
				Block -= m_map_size;
			if (map_ptr()[Block] == 0)
				map_ptr()[Block] = alloc().allocate(deque_size);

			alloc().construct(map_ptr()[Block] + Newoff % deque_size, val);
			++m_size;
		}

		void pop_front()
		{	
			if ( !empty() )// something to erase, do it
			{	
				size_type Block = m_offset / deque_size;
				alloc().destroy(map_ptr()[Block] + m_offset % deque_size);

				if ( m_map_size * deque_size <= ++m_offset)
					m_offset = 0;
				if (--m_size == 0)
					m_offset = 0;
			}
		}		

		void pop_back()
		{	
			if (!empty())// something to erase, do it
			{	
				size_type Newoff = m_size + m_offset - 1;
				size_type Block = Newoff / deque_size;
				if ( m_map_size <= Block)
					Block -= m_map_size;
				alloc().destroy(map_ptr()[Block] + Newoff % deque_size);
				if (--m_size == 0)
					m_offset = 0;
			}
		}

		template<class It>
		void assign(It First, It Last) {	assign_impl(First, Last, typename i3::iterator_traits<It>::iterator_category() );	}

		void assign(size_type count, const T& val) {	assign_n(count, val);	}

		iterator insert(const_iterator where, const T& val)
		{	
			if (where == begin())
			{	// insert at front
				push_front(val);
				return (begin());
			}
			else if (where == end())
			{	// insert at back
				push_back(val);
				return (end() - 1);
			}
			else
			{	// insert inside sequence
				iterator Mid;
				size_type Off = where - begin();
				T tmp = val;	// in case _Val is in sequence

				if (Off < m_size / 2)
				{	// closer to front, push to front then copy
					push_front(front());
					Mid = begin() + Off;
					i3::copy(begin() + 2, Mid + 1, begin() + 1);
				}
				else
				{	// closer to back, push to back then copy
					push_back(back());
					Mid = begin() + Off;
					i3::copy_backward(Mid, end() - 2, end() - 1);
				}

				*Mid = tmp;	// store inserted value
				return cast_to_iterator(Mid);
			}
		}

		void insert(const_iterator where, size_type count, const T& val) {	insert_n(where, count, val);  }

		template<class It>
		void insert(const_iterator where, It First, It Last)
			{	insert_impl(where, First, Last,	typename i3::iterator_traits<It>::iterator_category() ); 	}
		

		iterator erase(const_iterator where) {	return erase(where, where + 1);	}
		iterator erase(const_iterator First_arg, const_iterator Last_arg)
		{	
			iterator First = cast_to_iterator(First_arg);
			iterator Last = cast_to_iterator(Last_arg);

			size_type Off = First - begin();
			size_type Count = Last - First;

			if (Off < (size_type)(end() - Last))
			{	// closer to front
				i3::copy_backward(begin(), First, Last);	// copy over hole
				for (; 0 < Count; --Count)
					pop_front();	// pop copied elements
			}
			else
			{	// closer to back
				i3::copy(Last, end(), First);	// copy over hole
				for (; 0 < Count; --Count)
					pop_back();	// pop copied elements
			}

			return begin() + Off;
		}

		void clear()
		{
			while (!empty())
				pop_back();

			// 벡터처럼..영역을 유지하는게 낫다고 판단함 ( MSVC와는 달리 맵은 유지하도록 함)
		
		}
		
		void swap(my_type& rhs)
		{	// exchange contents with _Right
			if (this == &rhs)
				;	// same object, do nothing
			else if (alloc() == rhs.alloc())
			{	// same allocator, swap control information
				i3::swap(map_ptr(), rhs.map_ptr());
				i3::swap(m_map_size, rhs.m_map_size);
				i3::swap(m_offset, rhs.m_offset);
				i3::swap(m_size, rhs.m_size);
			}
			else
			{	// different allocator, do multiple assigns
				T tmp = *this;
				*this = rhs;
				rhs = tmp;
			}
		}
		

		size_type max_size() const {	return alloc().max_size();  }
		
	private:
		
		iterator		cast_to_iterator(const_iterator where) { return iterator( where.get_offset(), this );  }



		template<class It>
		void construct(It count, It val, i3::integral_iterator_tag)	{	construct_n((size_type)count, (T)val);	}
		template<class It>
		void construct(It First, It Last, i3::input_iterator_tag tag) {	insert_impl(begin(), First, Last, tag);  }

		void construct_n(size_type count, const T& val)	{	insert_n(begin(), count, val);  }

		template<class It>
		void assign_impl(It count, It val, i3::integral_iterator_tag) {	assign_n((size_type)count, (T)val);  }

		template<class It>
		void assign_impl(It First, It Last, i3::input_iterator_tag) { erase(begin(), end());	insert(begin(), First, Last);	}
		
		void assign_n(size_type count, const T& val)
		{	
			T tmp = val;	// in case _Val is in sequence
			erase(begin(), end());
			insert_n(begin(), count, tmp);
		}

		void insert_n(const_iterator where, size_type count, const T& val)
		{
			iterator Mid;
			size_type Num;
			size_type Off = where - begin();
			size_type Rem = m_size - Off;
			size_type Oldsize = m_size;
			
			if (Off < Rem)	// closer to front
			{	
				if (Off < count)	// insert longer than prefix
				{	
					for (Num = count - Off; 0 < Num; --Num)
						push_front(val);	// push excess values
					for (Num = Off; 0 < Num; --Num)
						push_front(begin()[count - 1]);	// push prefix

					Mid = begin() + count;
					i3::fill(Mid, Mid + Off, val);	// fill in rest of values
				}
				else
				{	// insert not longer than prefix
					for (Num = count; 0 < Num; --Num)
						push_front(begin()[count - 1]);	// push part of prefix

					Mid = begin() + count;
					T	tmp = val;	// in case _Val is in sequence
					i3::copy(Mid + count, Mid + Off, Mid);	// copy rest of prefix
					i3::fill(begin() + Off, Mid + Off, tmp);	// fill in values
				}
			}
			else
			{		// closer to back

				if (Rem < count)
				{	// insert longer than suffix
					for (Num = count - Rem; 0 < Num; --Num)
						push_back(val);	// push excess values
					for (Num = 0; Num < Rem; ++Num)
						push_back(begin()[Off + Num]);	// push suffix

					Mid = begin() + Off;
					i3::fill(Mid, Mid + Rem, val);	// fill in rest of values
				}
				else
				{	// insert not longer than prefix
					for (Num = 0; Num < count; ++Num)
						push_back(begin()[ Off + Rem - count + Num]);	// push part of prefix

					Mid = begin() + Off;
					T tmp = val;	// in case _Val is in sequence
					i3::copy_backward(Mid, Mid + Rem - count, Mid + Rem);	// copy rest of prefix
					i3::fill(Mid, Mid + count, tmp);	// fill in values
				}

			}
		}

		template<class It>
		void insert_impl(const_iterator where, It count, It val, i3::integral_iterator_tag ){ insert_n(where, (size_type)count, (T)val); }
		
		template<class It>
		void insert_impl(const_iterator where, It First, It Last, i3::input_iterator_tag )
		{	// insert [_First, _Last) at _Where, input iterators
			size_type Off = where - begin();
			size_type Rem = m_size - Off;
			size_type Oldsize = m_size;

			if (First == Last)
				;
			else if (Off < Rem)
			{	// closer to front
				for (; First != Last; ++First)
					push_front((value_type)*First);	// prepend flipped

				size_type Num = m_size - Oldsize;

				if (0 < Off)
				{	// insert not at beginning, flip new stuff into place
					reverse(Num, Num + Off);
					reverse(0, Num + Off);
				}
				else
					reverse(0, Num);	// flip new stuff in place
			}
			else
			{	// closer to back
				for (; First != Last; ++First)
					push_back((value_type)*First);	// append

				if (Off < Oldsize)
				{	// insert not at end, flip new stuff into place
					reverse(Off, Oldsize);
					reverse(Oldsize, m_size);
					reverse(Off, m_size);
				}
			}
		}


		void	grow_map_1()		// grow map by _Count pointers
		{	
			size_type count = m_map_size / 2;	// try to grow by 50%
			if (count < deque_map_size)
				count = deque_map_size;

			size_type Myboff = m_offset / deque_size;
			ppointer Newmap = map_alloc().allocate(m_map_size + count);
			ppointer Myptr = Newmap + Myboff;

			Myptr = i3::uninitialized_copy(map_ptr() + Myboff, map_ptr() + m_map_size, Myptr, map_alloc());	// copy initial to end

			if ( Myboff <= count )
			{	// increment greater than offset of initial block
				Myptr = i3::uninitialized_copy(map_ptr(), map_ptr() + Myboff, Myptr, map_alloc());	// copy rest of old
				//  포인터이므로, 모두 멤셋제로로 대치가능함..
				::memset( Myptr, 0, sizeof(pointer) * (count - Myboff) );
				::memset( Newmap, 0, sizeof(pointer) * Myboff );
			}
			else
			{	// increment not greater than offset of initial block
				i3::uninitialized_copy( map_ptr(), map_ptr() + count, Myptr, map_alloc());	// copy more old
				Myptr = i3::uninitialized_copy(map_ptr() + count, map_ptr() + Myboff, Newmap, map_alloc());	// copy rest of old
				::memset( Myptr, 0, sizeof(pointer) * count ); 
			}
			
//			destroyrange(map_ptr() + Myboff, map_ptr() + m_map_size, map_alloc());		// 포인터의 소멸자호출은 무의미..
			if (map_ptr())
				map_alloc().deallocate(map_ptr(), m_map_size);	// free storage for old

			map_ptr() = Newmap;	// point at new
			m_map_size += count;
		}

		void reverse(size_type First, size_type Last)
		{	// reverse a subrange
			iterator Start = begin();
			for (; First != Last && First != --Last ; ++First)
				i3::swap(Start[First], Start[Last]);
		}
		
		compressed_pair<pointer_allocator_type, ppointer>	m_map_alloc_and_map;
		
		__forceinline const allocator_type&				alloc() const { return allocator_base::get(); }
		__forceinline allocator_type&					alloc() { return allocator_base::get(); }
		__forceinline const pointer_allocator_type&		map_alloc() const { return m_map_alloc_and_map.first(); }
		__forceinline pointer_allocator_type&			map_alloc()  { return m_map_alloc_and_map.first(); }
		
		__forceinline const ppointer&					map_ptr() const { return m_map_alloc_and_map.second(); }
		__forceinline ppointer&							map_ptr()  { return m_map_alloc_and_map.second(); }
		
//      다음의 세 변수는 EBO최적화를 위해 삭제되고, 기본클래스와 compressed_pair를 씀...
//		pointer_allocator_type		m_map_allocator;
//		allocator_type				m_allocator;
//		ppointer					m_map;

		size_type					m_map_size;		// 배열크기..
		size_type					m_offset;		// 시작점
		size_type					m_size;			// 
	};

	template<class T, class A>	inline
	typename deque<T, A>::const_iterator operator+(typename deque<T, A>::difference_type Off, typename deque<T, A>::const_iterator Next)
	{	
		return Next += Off;
	}
	template<class T, class A>	inline
	typename deque<T, A>::iterator operator+(typename deque<T, A>::difference_type Off, typename deque<T, A>::iterator Next)
	{	
		return Next += Off;
	}

	// deque TEMPLATE OPERATORS
	template<class T, class A> inline
	void swap(deque<T, A>& Left, deque<T, A>& Right) {	Left.swap(Right); }

	template<class T, class A> inline
	bool operator==(const deque<T, A>& Left, const deque<T, A>& Right)
	{	
		return Left.size() == Right.size() && i3::equal(Left.begin(), Left.end(), Right.begin());
	}

	template<class T, class A> inline
	bool operator!=(const deque<T, A>& Left, const deque<T, A>& Right) { return !(Left == Right); }

	template<class T, class A> inline
	bool operator<(const deque<T, A>& Left, const deque<T, A>& Right) 
	{	
		return i3::lexicographical_compare(Left.begin(), Left.end(), Right.begin(), Right.end());
	}

	template<class T, class A> inline
	bool operator<=(const deque<T, A>& Left, const deque<T, A>& Right){	return !(Right < Left); }

	template<class T, class A> inline
	bool operator>(const deque<T, A>& Left,	const deque<T, A>& Right) {	return Right < Left;	}

	template<class T, class A> inline
	bool operator>=(const deque<T, A>& Left, const deque<T, A>& Right) { return !(Left < Right);	}


}



#pragma pack(pop)