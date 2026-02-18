#pragma once

// reserve() 관련된 이슈인 멤버변수 하나를 버리고 벡터처럼 구성... reserve() 제거..
// push_back보다는 생성자 사용 위주의 사용이라면 할당을 위한 멤버변수가 필요가 없다..
// 항상 재할당된다는 가정이 있어야하기 때문에 사이즈 변경시마다 모두 재할당되도록 코드를 수정해야한다..
//
#include "svector_fwd.h"
#include "allocator.h"
#include "memory.h"
#include "xmemory.h"
#include "ebo.h"
#include "algorithm/copy.h"
#include "algorithm/copy_backward.h"
#include "algorithm/fill.h"
#include "algorithm/fill_n.h"
#include "algorithm/equal.h"
#include "algorithm/lexicographical_compare.h"

#pragma pack(push, _CRT_PACKING)

namespace i3
{
	
	template <class T, class Alloc>
	class svector : private enable_ebo< typename rebind_wrap<Alloc, T>::type >
	{
	public:
		typedef svector<T, Alloc>								my_type;

		typedef enable_ebo< typename rebind_wrap<Alloc, T>::type >	allocator_base;

		typedef typename Alloc::template rebind<T>::other		allocator_type;
		typedef	typename allocator_type::reference				reference;
		typedef typename allocator_type::const_reference		const_reference;

		typedef T*												iterator;		// 기존것과 다르게..
		typedef const T*										const_iterator;
		typedef typename allocator_type::size_type				size_type;
		typedef typename allocator_type::difference_type		difference_type;

		typedef T												value_type;
		typedef typename allocator_type::pointer				pointer;
		typedef typename allocator_type::const_pointer			const_pointer;

		typedef i3::reverse_iterator<iterator>					reverse_iterator;
		typedef i3::reverse_iterator<const_iterator>			const_reverse_iterator;

		svector() : m_first(nullptr), m_last(nullptr)	{ }
		explicit svector(const allocator_type& al) : allocator_base(al), m_first(nullptr), m_last(nullptr) {}
		explicit svector(size_type n) : m_first(nullptr), m_last(nullptr) {	construct_n( n, T() );	}
		explicit svector(size_type n, const T& val) : m_first(nullptr), m_last(nullptr) {	construct_n(n, val);  }
		svector(size_type n, const T& val, const allocator_type& al) : allocator_base(al), m_first(nullptr), m_last(nullptr) { construct_n( n, val ); }
		svector(const my_type& rhs ) : allocator_base( rhs ), m_first(nullptr), m_last(nullptr)
		{
			size_t numRhs = rhs.size();
			if ( numRhs > 0 )
			{
				m_first = allocator_base::get().allocate(numRhs);		//m_alloc.allocate(numRhs);
				m_last = i3::uninitialized_copy(rhs.begin(), rhs.end(), m_first, allocator_base::get() );	// i3::uninitialized_copy(rhs.begin(), rhs.end(), m_first, m_alloc );
			}
		}

		template< class Iter>
		svector( Iter first, Iter last ) : m_first(nullptr), m_last(nullptr)
		{  construct( first, last, typename iterator_traits<Iter>::iterator_category()  );      }

		template< class Iter>
		svector( Iter first, Iter last, const allocator_type& al ) : m_first(nullptr), m_last(nullptr), allocator_base(al)
		{  construct( first, last, typename iterator_traits<Iter>::iterator_category()  );      }

		~svector()
		{
			if (m_first != 0)
			{
				destroy_range(m_first, m_last, allocator_base::get() );	
				allocator_base::get().deallocate(m_first, m_last - m_first); 
			}
		}

		my_type&		operator= (const my_type& rhs)
		{
			if (this == &rhs) return *this;

			const size_type rhsSize = rhs.size();

			if (rhsSize == 0)
				clear();	// new sequence empty, erase existing sequence
			else 
			{	// not enough room, allocate new array and construct new
				if (m_first != 0)
				{	// discard old array
					destroy_range(m_first, m_last, allocator_base::get() ); 
					allocator_base::get().deallocate(m_first, m_last - m_first); 
				}
				m_first = allocator_base::get().allocate(rhsSize);	
				m_last  = i3::uninitialized_copy(rhs.m_first, rhs.m_last, m_first);
			}

			return *this;
		}

		size_type		capacity() const { return m_last - m_first; }		// capacity() == size()
		iterator		begin() { return m_first; }
		const_iterator	begin() const { return m_first; }
		iterator		end()	{ return m_last; }
		const_iterator	end() const { return m_last; }

		reverse_iterator		rbegin() {	return reverse_iterator(end()); }
		const_reverse_iterator	rbegin() const {	return const_reverse_iterator(end()); }
		reverse_iterator		rend()	{	return reverse_iterator(begin());	}
		const_reverse_iterator	rend() const {	return const_reverse_iterator(begin());	}

		void					resize(size_type newSize) {  resize(newSize, T()); }
		void					resize(size_type newSize, T val)
		{
			if (size() < newSize)
				insert_n(end(), newSize - size(), val);
			else if ( newSize < size() )
				erase(begin() + newSize, end());
		}

		size_type		max_size() const { return allocator_base::get().max_size(); }  // { return m_alloc.max_size(); }

		size_type		size() const	{ return m_last - m_first; }
		bool			empty() const	{ return m_first == 0 ; }

		allocator_type	get_allocator() const { return allocator_base::get(); }  // { return m_alloc; }

		const_reference at(size_type Pos) const	{	return (*(begin() + Pos));	}
		reference		at(size_type _Pos) {	return (*(begin() + _Pos));	}

		const_reference operator[](size_type Pos) const	{	return (*(m_first + Pos));	}
		reference		operator[](size_type Pos)		{	return (*(m_first + Pos));  }

		reference front() {	return (*begin());	}
		const_reference front() const {	return (*begin());	}
		reference back() {	return (*(end() - 1));	}
		const_reference back() const {	return (*(end() - 1)); }

		// push_back은 이제 재할당을 경유하는 insert개념이 됨..
		void push_back(const T& Val) {	insert_1(end(), Val);	}

		// pop_back 역시 재할당을 경유하도록 해야한다..
		void pop_back()
		{	// erase element at end
			if (!empty())
			{	
				size_type new_count = size() - 1;
				pointer new_vec = allocator_base::get().allocate(new_count);
				i3::uninitialized_copy(m_first, m_last - 1, new_vec, allocator_base::get());
				
				destroy_range(m_first, m_last, allocator_base::get() ); 
				allocator_base::get().deallocate(m_first, m_last - m_first); 
				
				m_first = new_vec;
				m_last = new_vec + new_count;
			}
		}

		template<class Iter>
		void assign(Iter First, Iter Last)
		{	// assign [_First, _Last)
			assign_impl(First, Last,	typename iterator_traits<Iter>::iterator_category()); 
		}

		void assign(size_type count, const T& val){	assign_n(count, val); }

		iterator insert(const_iterator where, const T& Val)
		{	// insert _Val at _Where
			size_type Off = size() == 0 ? 0 : where - begin();
			insert_1(where, Val);
			return (begin() + Off);
		}

		void insert(const_iterator where, size_type Count, const T& Val) {	insert_n(where, Count, Val); }

		template<class Iter>
		void insert(const_iterator Where, Iter First, Iter Last)
		{	
			insert_impl(Where, First, Last, typename iterator_traits<Iter>::iterator_category() );
		}

		iterator erase(const_iterator where)
		{	// erase element at where

			iterator Where = iterator(where);
			
			size_type new_count = size() - 1;
		
			if ( !new_count )
			{
				destroy_range(m_first, m_last, allocator_base::get() ); 
				allocator_base::get().deallocate(m_first, m_last - m_first);
				m_first = 0; m_last = 0;
				return end();
			}

			size_type Whereoff  = Where - m_first;
			pointer new_vec = allocator_base::get().allocate(new_count);
			i3::uninitialized_copy(m_first, Where, new_vec, allocator_base::get());
			i3::uninitialized_copy( Where + 1, m_last, new_vec + Whereoff, allocator_base::get());
			destroy_range(m_first, m_last, allocator_base::get() ); 
			allocator_base::get().deallocate(m_first, m_last - m_first);

			m_first = new_vec;
			m_last = new_vec + new_count;

			return begin() + Whereoff;
		}

		iterator erase(const_iterator f, const_iterator l)
		{	// erase [first, last)
			iterator first = iterator(f);
			iterator last =  iterator(l);

			if (first != last)
			{	
				size_type new_count = size() - ( last - first);

				if ( !new_count )
				{
					destroy_range(m_first, m_last, allocator_base::get() ); 
					allocator_base::get().deallocate(m_first, m_last - m_first);
					m_first = 0; m_last = 0;
					return end();
				}

				size_type Whereoff  = first - m_first;

				pointer new_vec = allocator_base::get().allocate(new_count);
				i3::uninitialized_copy(m_first, first, new_vec, allocator_base::get());
				i3::uninitialized_copy( last, m_last, new_vec + Whereoff, allocator_base::get());

				destroy_range(m_first, m_last, allocator_base::get() ); 
				allocator_base::get().deallocate(m_first, m_last - m_first);

				m_first = new_vec;
				m_last = new_vec + new_count;

				first = begin() + Whereoff;
			}

			return first;
		}

		void clear() {	erase_begin_end(); }   

		void swap(my_type& Right)
		{	// exchange contents with Right
			if (this == &Right)
				return ;

			if ( this->allocator_base::get() == Right.allocator_base::get() )	
			{	// same allocator, swap control information
				i3::swap(m_first, Right.m_first);
				i3::swap(m_last, Right.m_last);
			}
			else
			{	// different allocator, do multiple assigns
				my_type Ts = *this;
				*this = Right;
				Right = Ts;
			}
		}

	private:
		template <class Iter>
		void		construct( Iter count, Iter val, integral_iterator_tag ){	construct_n( (size_type)count, (T)val); 	}
		template <class Iter>
		void		construct( Iter first, Iter last, input_iterator_tag ) {    insert(begin(), first, last);	}

		void		construct_n( size_type n, const T& val)
		{
			if ( !n || n > max_size() ) 
				return;
			m_first = allocator_base::get().allocate(n);				
			i3::uninitialized_fill_n(m_first, n, val, allocator_base::get() );	
			m_last  = m_first + n;
		}

		template <class Iter>
		void	insert_impl(const_iterator where, Iter first, Iter last, integral_iterator_tag )
		{
			insert_n( where, (size_type)first, (T)last);
		}

		// 아래 함수의 경우는 문제가 있을수 있기 때문에 좀 많이 수정한다.
		template <class Iter>
		void	insert_impl(const_iterator where, Iter first, Iter last, input_iterator_tag )
		{
			if (first != last)
			{	// worth doing, gather at end and rotate into place
				size_type Oldsize = size();
				size_type Whereoff = where - m_first;
				
				size_type NewSize = 0;  	
				i3::distance(first, last, NewSize);
				NewSize += Oldsize;

				pointer Newvec = allocator_base::get().allocate(NewSize);	

				pointer Ptr = Newvec;
				Ptr = i3::uninitialized_copy(m_first, m_last, Newvec, allocator_base::get());	

				for (; first != last ; ++first )
				{
					Ptr = i3::uninitialized_copy_1(first, Ptr, allocator_base::get());
				}
				
				if (m_first != 0)
				{	
					destroy_range(m_first, m_last, allocator_base::get() );		
					allocator_base::get().deallocate( m_first, m_last - m_first);	
				}
				
				m_last = Newvec + NewSize;
				m_first = Newvec;

				reverse(m_first + Whereoff, m_first + Oldsize);
				reverse(m_first + Oldsize, m_last);
				reverse(m_first + Whereoff, m_last); 
			}
		}

		template<class Iter>
		void	insert_impl(const_iterator where, Iter First, Iter Last, forward_iterator_tag)
		{	// insert [_First, _Last) at _Where, forward iterators
			iterator Where = iterator(where);
			size_type Count = 0;
			i3::distance(First, Last, Count);
		
			if (Count == 0 || max_size() - size() < Count)
				;
			else 
			{	
				Count += size();

				pointer Newvec = allocator_base::get().allocate(Count);	
				pointer Ptr = Newvec;

				Ptr = i3::uninitialized_copy(m_first, Where, Newvec, allocator_base::get());	
				Ptr = i3::uninitialized_copy(First, Last, Ptr, allocator_base::get());		
				i3::uninitialized_copy(Where, m_last, Ptr, allocator_base::get());			
			

				if (m_first != 0)
				{	
					destroy_range(m_first, m_last, allocator_base::get() );		
					allocator_base::get().deallocate( m_first, m_last - m_first);	
				}

				m_last = Newvec + Count;
				m_first = Newvec;
			}
			
		}

		void	reverse(pointer first, pointer last)
		{
			for ( ; first != last && first != --last ; ++first )
				i3::swap(*first, *last);
		}

		void	insert_n(const_iterator where, size_type count, const T& val)
		{	// insert _Count * _Val at _Where
			iterator Where = iterator(where);
		
			if ( count == 0 || max_size() - size() < count )
				;
			else 
			{	// not enough room, reallocate
			
				pointer Newvec = allocator_base::get().allocate( count + size() );			
				size_type Whereoff = Where - m_first;

				i3::uninitialized_fill_n(Newvec + Whereoff, count, val, allocator_base::get()); 
				i3::uninitialized_copy(m_first, Where, Newvec, allocator_base::get());			
				i3::uninitialized_copy( Where, m_last, Newvec + Whereoff + count, allocator_base::get()); 
				
				count += size();

				if (m_first != 0)		// destroy and deallocate old array
				{	
					destroy_range(m_first, m_last, allocator_base::get());			
					allocator_base::get().deallocate( m_first, m_last - m_first);		
				}

				m_last = Newvec + count;
				m_first = Newvec;
			}
			
		}


		////		
		void	insert_1(const_iterator where, const T& val)
		{	// insert _Count * _Val at _Where
			iterator Where = iterator(where);
			
			if ( max_size() - size() < 1 )
				;
			else 
			{	
				size_type NewCount = size() + 1;
				pointer Newvec = allocator_base::get().allocate(NewCount); //	m_alloc.allocate(Capacity);
				size_type Whereoff = Where - m_first;

				i3::uninitialized_fill_1(Newvec + Whereoff, val, allocator_base::get() );	
				i3::uninitialized_copy(m_first, Where, Newvec, allocator_base::get());	
				i3::uninitialized_copy(Where, m_last, Newvec + Whereoff + 1, allocator_base::get());	
			
				if (m_first != 0)		// destroy and deallocate old array
				{	
					destroy_range(m_first, m_last, allocator_base::get());		
					allocator_base::get().deallocate(m_first, m_last - m_first);	
				}

				m_last = Newvec + NewCount;
				m_first = Newvec;
			}
			
		}

		void erase_begin_end()
		{
			if (m_first != 0)
			{	
				destroy_range(m_first, m_last, allocator_base::get());	
				allocator_base::get().deallocate(m_first, m_last - m_first);
				m_first = 0;	m_last = 0;
			}
		}
		////

		template<class Iter>
		void assign_impl(Iter Count, Iter Val, integral_iterator_tag)
		{	// assign _Count * _Val
			assign_n((size_type)Count, (T)Val);
		}

		template<class Iter>
		void assign_impl(Iter First, Iter Last, input_iterator_tag)
		{	// assign [_First, _Last), input iterators
			erase_begin_end();
			insert(begin(), First, Last);
		}

		void assign_n( size_type Count, const T& Val)
		{
			T Tmp = Val;	// in case _Val is in sequence
			erase_begin_end();
			insert(begin(), Count, Tmp);
		}

		pointer												m_first;
		pointer												m_last;			// 예약할당인 m_end를 제거한다..
	};

	template<class T, class Alloc> inline
		bool operator==(const svector<T, Alloc>& Left, const svector<T, Alloc>& Right)
	{	// test for svector equality
		return (Left.size() == Right.size() && i3::equal(Left.begin(), Left.end(), Right.begin()));
	}

	template<class T, class Alloc> inline
		bool operator!=(const svector<T, Alloc>& Left, const svector<T, Alloc>& Right)
	{	// test for svector inequality
		return (!(Left == Right));
	}

	template<class T, class Alloc> inline
		bool operator<(const svector<T, Alloc>& Left, const svector<T, Alloc>& Right)
	{	// test if Left < Right for vectors
		return (i3::lexicographical_compare(Left.begin(), Left.end(), Right.begin(), Right.end()));
	}

	template<class T, class Alloc> inline
		bool operator>(const svector<T, Alloc>& Left, const svector<T, Alloc>& Right)
	{	// test if Left > Right for vectors
		return (Right < Left);
	}

	template<class T,	class Alloc> inline
		bool operator<=(const svector<T, Alloc>& Left, const svector<T, Alloc>& Right)
	{	// test if Left <= Right for vectors
		return (!(Right < Left));
	}

	template<class T, class Alloc> inline
		bool operator>=(const svector<T, Alloc>& Left, const svector<T, Alloc>& Right)
	{	// test if Left >= Right for vectors
		return (!(Left < Right));
	}

	template<class T, class Alloc> inline
		void swap(svector<T, Alloc>& Left, svector<T, Alloc>& Right)
	{	// swap Left and Right vectors
		Left.swap(Right);
	}
		
	
	
}

#pragma pack(pop)