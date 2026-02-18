#pragma once

#include "../vector_fwd.h"
#include "../allocator.h"
#include "../memory.h"
#include "../ebo.h"

#pragma pack(push, _CRT_PACKING)


namespace i3
{

//	template <class T, class Alloc > class vector;
//	template <class T>
//	struct vector_std_alloc {	typedef vector<T, i3::allocator<T> >	type; };

	template <class T, class Alloc>
	class vector : private enable_ebo< typename rebind_wrap<Alloc, T>::type >
	{
	public:
		typedef vector<T, Alloc>								my_type;
		
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
		
		vector() : m_first(NULL), m_last(NULL), m_end(NULL)	{ } 
		explicit vector(const allocator_type& al) : allocator_base(al), m_first(NULL), m_last(NULL), m_end(NULL) {} 
		explicit vector(size_type n) : m_first(NULL), m_last(NULL), m_end(NULL) {	construct_n( n, T() );	}
		explicit vector(size_type n, const T& val) : m_first(NULL), m_last(NULL), m_end(NULL) {	construct_n(n, val);  }
		vector(size_type n, const T& val, const allocator_type& al) : allocator_base(al), m_first(NULL), m_last(NULL), m_end(NULL) { construct_n( n, val ); }
		vector(const my_type& rhs ) : allocator_base( rhs ), m_first(NULL), m_last(NULL), m_end(NULL)
		{
			size_t numRhs = rhs.size();
			if ( numRhs > 0 )
			{
				m_first = allocator_base::get().allocate(numRhs);		//m_alloc.allocate(numRhs);
				m_last = i3::uninitialized_copy(rhs.begin(), rhs.end(), m_first, allocator_base::get() );	// i3::uninitialized_copy(rhs.begin(), rhs.end(), m_first, m_alloc );
				m_end   = m_first + numRhs;
			}
		}

		template< class Iter>
		vector( Iter first, Iter last ) : m_first(NULL), m_last(NULL), m_end(NULL) 
		{  construct( first, last, typename iterator_traits<Iter>::iterator_category()  );      }
		
		template< class Iter>
		vector( Iter first, Iter last, const allocator_type& al ) : m_first(NULL), m_last(NULL), m_end(NULL), allocator_base(al) 
		{  construct( first, last, typename iterator_traits<Iter>::iterator_category()  );      }

		~vector()
		{
			if (m_first != 0)
			{
				destroy_range(m_first, m_last, allocator_base::get() );	//destroy_range(m_first, m_last, m_alloc);
				allocator_base::get().deallocate(m_first, m_end - m_first); // m_alloc.deallocate(m_first, m_end - m_first);
			}
		}
		
		my_type&		operator= (const my_type& rhs)
		{
			if (this == &rhs) return *this;
			
			const size_type rhsSize = rhs.size();

			if (rhsSize == 0)
				clear();	// new sequence empty, erase existing sequence
			else 
			if (rhsSize <= size() )
			{	// enough elements, copy new and destroy old
				pointer Ptr = i3::copy( rhs.m_first, rhs.m_last, m_first);
				destroy_range(Ptr, m_last, allocator_base::get() );	//destroy_range(Ptr, m_last, m_alloc);	// destroy old
				m_last = m_first + rhsSize;
			}
			else if ( rhsSize <= capacity() )
			{	// enough room, copy and construct new
				pointer Ptr = rhs.m_first + size();
				i3::copy(rhs.m_first, Ptr, m_first);
				m_last = i3::uninitialized_copy(Ptr, rhs.m_last, m_last);
			}
			else
			{	// not enough room, allocate new array and construct new
				if (m_first != 0)
				{	// discard old array
					destroy_range(m_first, m_last, allocator_base::get() ); //destroy_range(m_first, m_last,m_alloc);
					allocator_base::get().deallocate(m_first, m_end - m_first); // m_alloc.deallocate(m_first, m_end - m_first );
				}
				
				m_first = allocator_base::get().allocate(rhsSize);	// m_alloc.allocate( rhsSize );
				m_last  = i3::uninitialized_copy(rhs.m_first, rhs.m_last, m_first);
				m_end   = m_first + rhsSize;
			}

			return *this;
		}
		
		void			reserve(size_type count)
		{
			if ( capacity() >= count ||	count > max_size() )
				return;	// result too long

			pointer Ptr = allocator_base::get().allocate(count); // m_alloc.allocate(count);
			i3::uninitialized_copy(begin(), end(), Ptr);
			
			size_type Size = size();
			if ( m_first != 0)
			{	// destroy and deallocate old array
				destroy_range(m_first, m_last,allocator_base::get() );		//	destroy_range(m_first, m_last, m_alloc);
				allocator_base::get().deallocate(m_first, m_end - m_first);	//	m_alloc.deallocate(m_first, m_end - m_first);
			}

			m_end = Ptr + count;
			m_last = Ptr + Size;
			m_first = Ptr;
		}
		
		size_type		capacity() const { return m_end - m_first; }
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
		bool			empty() const	{ return size() == 0 ; }
		allocator_type	get_allocator() const { return allocator_base::get(); }  // { return m_alloc; }
		
		const_reference at(size_type Pos) const	{	return (*(begin() + Pos));	}
		reference		at(size_type _Pos) {	return (*(begin() + _Pos));	}
		
		const_reference operator[](size_type Pos) const	{	return (*(m_first + Pos));	}
		reference		operator[](size_type Pos)		{	return (*(m_first + Pos));  }

		reference front() {	return (*begin());	}
		const_reference front() const {	return (*begin());	}
		reference back() {	return (*(end() - 1));	}
		const_reference back() const {	return (*(end() - 1)); }
		
		void push_back(const T& Val)
		{	// insert element at end
			if ( size() < capacity() )
			{
				i3::uninitialized_fill_1(m_last, Val, allocator_base::get() );		//	i3::uninitialized_fill_1( m_last, Val, m_alloc);
				++m_last;
			}
			else
				insert_1(end(), Val);
		}
		void pop_back()
		{	// erase element at end
			if (!empty())
			{	
				destroy_range_1(m_last - 1, allocator_base::get() );		//	destroy_range_1( m_last - 1, m_alloc);
				--m_last;
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
			i3::copy( Where + 1, m_last, Where );
			destroy_range_1(m_last - 1, allocator_base::get());	// destroy_range_1( m_last - 1, m_alloc );
			--m_last;
			return Where;
		}
		
		iterator erase(const_iterator f, const_iterator l)
		{	// erase [first, last)
			iterator first = iterator(f);
			iterator last =  iterator(l);

			if (first != last)
			{	// worth doing, copy down over hole
				pointer p = i3::copy(last, m_last, first);
				destroy_range(p, m_last, allocator_base::get() );	// destroy_range(Ptr, m_last, m_alloc);
				m_last = p;
			}
			return first;
		}
	
		void clear() {	erase_begin_end(); }    // { erase(begin(), end());	}

		void swap(my_type& Right)
		{	// exchange contents with Right
			if (this == &Right)
				return ;

			if ( this->allocator_base::get() == Right.allocator_base::get() )	// if (this->m_alloc == Right.m_alloc)
			{	// same allocator, swap control information
				i3::swap(m_first, Right.m_first);
				i3::swap(m_last, Right.m_last);
				i3::swap(m_end, Right.m_end);
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
			m_first = allocator_base::get().allocate(n);				// m_alloc.allocate(n);
			i3::uninitialized_fill_n(m_first, n, val, allocator_base::get() );	//i3::uninitialized_fill_n(m_first, n, val, m_alloc);
			m_last  = m_first + n;
			m_end   = m_last;
		}
	
		template <class Iter>
		void	insert_impl(const_iterator where, Iter first, Iter last, integral_iterator_tag )
		{
			insert_n( where, (size_type)first, (T)last);
		}
		template <class Iter>
		void	insert_impl(const_iterator where, Iter first, Iter last, input_iterator_tag )
		{
			if (first != last)
			{	// worth doing, gather at end and rotate into place
				size_type Oldsize = size();
				size_type Whereoff = where - m_first;

				for (; first != last; ++first)
					insert_n( end(), (size_type)1, (value_type) *first );

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
			size_type Capacity = capacity();

			if (Count == 0 || max_size() - size() < Count)
				;
			else if (Capacity < size() + Count)
			{	// not enough room, reallocate
				Capacity = ( max_size() - Capacity / 2 < Capacity )	? 0 : Capacity + Capacity / 2;	// try to grow by 50%
				if (Capacity < size() + Count)
					Capacity = size() + Count;
				pointer Newvec = allocator_base::get().allocate(Capacity);	// m_alloc.allocate(Capacity);
				pointer Ptr = Newvec;

				
				Ptr = i3::uninitialized_copy(m_first, Where, Newvec, allocator_base::get());	//i3::uninitialized_copy(m_first, Where, Newvec, m_alloc);	// copy prefix
				Ptr = i3::uninitialized_copy(First, Last, Ptr, allocator_base::get());		//i3::uninitialized_copy(First, Last, Ptr, m_alloc );
				i3::uninitialized_copy(Where, m_last, Ptr, allocator_base::get());			//i3::uninitialized_copy( Where, m_last, Ptr, m_alloc);

				Count += size();

				if (m_first != 0)
				{	// destroy and deallocate old array
					destroy_range(m_first, m_last, allocator_base::get() );		//destroy_range(m_first, m_last, m_alloc);
					allocator_base::get().deallocate( m_first, m_end - m_first);	//m_alloc.deallocate(m_first, m_end - m_first);
				}

				m_end = Newvec + Capacity;
				m_last = Newvec + Count;
				m_first = Newvec;
			}
			else
			{	// new stuff fits, append and rotate into place
				i3::uninitialized_copy(First, Last, m_last, allocator_base::get());			// i3::uninitialized_copy(First, Last, m_last, m_alloc);
				reverse(Where, m_last);
				reverse(m_last, m_last + Count);
				reverse(Where, m_last + Count);
				m_last += Count;
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
			size_type Capacity = capacity();

			if ( count == 0 || max_size() - size() < count )
				;
			else if ( Capacity < size() + count)
			{	// not enough room, reallocate
				Capacity = ( max_size() - Capacity / 2 < Capacity) ? 0 : Capacity + Capacity / 2;	// try to grow by 50%
				if ( Capacity < size() + count)	Capacity = size() + count;

				pointer Newvec = allocator_base::get().allocate(Capacity);			//	m_alloc.allocate(Capacity);
				size_type Whereoff = Where - m_first;
			
				i3::uninitialized_fill_n(Newvec + Whereoff, count, val, allocator_base::get()); //	i3::uninitialized_fill_n(Newvec + Whereoff, count, val, m_alloc );	// add new stuff
				i3::uninitialized_copy(m_first, Where, Newvec, allocator_base::get());			//	i3::uninitialized_copy( m_first, Where, Newvec, m_alloc );			// move prefix
				i3::uninitialized_copy( Where, m_last, Newvec + Whereoff + count, allocator_base::get()); // i3::uninitialized_copy( Where, m_last, Newvec + Whereoff + count, m_alloc);	// move suffix

				count += size();

				if (m_first != 0)		// destroy and deallocate old array
				{	
					destroy_range(m_first, m_last, allocator_base::get());			// destroy_range(m_first, m_last, m_alloc);
					allocator_base::get().deallocate( m_first, m_end - m_first);		// m_alloc.deallocate(m_first, m_end - m_first);
				}

				m_end = Newvec + Capacity;
				m_last = Newvec + count;
				m_first = Newvec;
			}
			else if ( (size_type)( m_last - Where ) < count)
			{	// new stuff spills off end
				T Tmp = val;	// in case _Val is in sequence
				i3::uninitialized_copy( Where, m_last, Where + count, allocator_base::get()); // i3::uninitialized_copy( Where, m_last, Where + count , m_alloc );	//  copy suffix
				i3::uninitialized_fill_n( m_last, count - ( m_last - Where), Tmp, allocator_base::get() ); //i3::uninitialized_fill_n( m_last, count - ( m_last - Where), Tmp, m_alloc);	// insert new stuff off end
				m_last += count;
				i3::fill(Where, m_last - count,	Tmp);	// insert up to old end
			}
			else
			{	// new stuff can all be assigned
				T Tmp = val;	// in case _Val is in sequence

				pointer Oldend = m_last;
				m_last = i3::uninitialized_copy( Oldend - count, Oldend, m_last, allocator_base::get()); //i3::uninitialized_copy( Oldend - count, Oldend, m_last, m_alloc);// copy suffix
				i3::copy_backward(Where, Oldend - count, Oldend );	// copy hole
				i3::fill(Where, Where + count,	Tmp);	// insert into hole
			}
		}


////		
		void	insert_1(const_iterator where, const T& val)
		{	// insert _Count * _Val at _Where
			iterator Where = iterator(where);
			size_type Capacity = capacity();

			if ( max_size() - size() < 1 )
				;
			else if ( Capacity < size() + 1 )
			{	// not enough room, reallocate
				Capacity = ( max_size() - Capacity / 2 < Capacity) ? 0 : Capacity + Capacity / 2;	// try to grow by 50%
				if ( Capacity < size() + 1)	Capacity = size() + 1;

				pointer Newvec = allocator_base::get().allocate(Capacity); //	m_alloc.allocate(Capacity);
				size_type Whereoff = Where - m_first;
			
				i3::uninitialized_fill_1(Newvec + Whereoff, val, allocator_base::get() );	// i3::uninitialized_fill_1(Newvec + Whereoff, val, m_alloc );	// add new stuff
				i3::uninitialized_copy(m_first, Where, Newvec, allocator_base::get());	// i3::uninitialized_copy( m_first, Where, Newvec, m_alloc );			// move prefix
				i3::uninitialized_copy(Where, m_last, Newvec + Whereoff + 1, allocator_base::get());	//i3::uninitialized_copy( Where, m_last, Newvec + Whereoff + 1, m_alloc);	// move suffix

				size_type count = 1 + size();

				if (m_first != 0)		// destroy and deallocate old array
				{	
					destroy_range(m_first, m_last, allocator_base::get());		// destroy_range(m_first, m_last, m_alloc);
					allocator_base::get().deallocate(m_first, m_end - m_first);	// m_alloc.deallocate(m_first, m_end - m_first);
				}

				m_end = Newvec + Capacity;
				m_last = Newvec + count;
				m_first = Newvec;
			}
			else if ( (size_type)( m_last - Where ) < 1 )
			{	// new stuff spills off end
				T Tmp = val;	// in case _Val is in sequence
				i3::uninitialized_copy(Where, m_last, Where + 1, allocator_base::get()); // i3::uninitialized_copy( Where, m_last, Where + 1 , m_alloc );	//  copy suffix
				i3::uninitialized_fill_1(m_last, Tmp, allocator_base::get());			// i3::uninitialized_fill_1( m_last, Tmp, m_alloc);	// insert new stuff off end
				++m_last;
				i3::fill(Where, m_last - 1,	Tmp);	// insert up to old end
			}
			else
			{	// new stuff can all be assigned
				T Tmp = val;	// in case _Val is in sequence

				pointer Oldend = m_last;
				m_last = i3::uninitialized_copy(Oldend - 1, Oldend, m_last, allocator_base::get()); // i3::uninitialized_copy( Oldend - 1, Oldend, m_last, m_alloc);// copy suffix
				i3::copy_backward(Where, Oldend - 1, Oldend );	// copy hole
				i3::fill(Where, Where + 1,	Tmp);	// insert into hole
			}
		}

		void erase_begin_end()
		{
			if (m_first != m_last)
			{	// worth doing, copy down over hole
				destroy_range(m_first, m_last, allocator_base::get());	// destroy_range(m_first, m_last, m_alloc);
				m_last = m_first;
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
		pointer												m_last;
		pointer												m_end;
	};
	
	template<class T, class Alloc> inline
	bool operator==(const vector<T, Alloc>& Left, const vector<T, Alloc>& Right)
	{	// test for vector equality
		return (Left.size() == Right.size() && i3::equal(Left.begin(), Left.end(), Right.begin()));
	}

	template<class T, class Alloc> inline
	bool operator!=(const vector<T, Alloc>& Left, const vector<T, Alloc>& Right)
	{	// test for vector inequality
		return (!(Left == Right));
	}

	template<class T, class Alloc> inline
	bool operator<(const vector<T, Alloc>& Left, const vector<T, Alloc>& Right)
	{	// test if Left < Right for vectors
		return (i3::lexicographical_compare(Left.begin(), Left.end(), Right.begin(), Right.end()));
	}

	template<class T, class Alloc> inline
	bool operator>(const vector<T, Alloc>& Left, const vector<T, Alloc>& Right)
	{	// test if Left > Right for vectors
		return (Right < Left);
	}

	template<class T,	class Alloc> inline
	bool operator<=(const vector<T, Alloc>& Left, const vector<T, Alloc>& Right)
	{	// test if Left <= Right for vectors
		return (!(Right < Left));
	}

	template<class T, class Alloc> inline
	bool operator>=(const vector<T, Alloc>& Left, const vector<T, Alloc>& Right)
	{	// test if Left >= Right for vectors
		return (!(Left < Right));
	}

	template<class T, class Alloc> inline
	void swap(vector<T, Alloc>& Left, vector<T, Alloc>& Right)
	{	// swap Left and Right vectors
		Left.swap(Right);
	}

}



#pragma pack(pop)