#pragma once

#include "xmemory.h"
#include "pair.h"

namespace i3
{
	
	template <class T> inline
	i3::pair<T*, ptrdiff_t> get_temporary_buffer(ptrdiff_t n)
	{
		if (n <= 0)
			n = 0;
		T* pBuf;
		for (pBuf = 0 ; 0 < n ; n /= 2)
			if ((pBuf = (T*)operator new( static_cast<size_t>(n) * sizeof(T), nothrow)) != 0)
				break;
		return (i3::pair<T*, ptrdiff_t>(_Pbuf, n));			
	}
	
	template<class T> inline
	void return_temporary_buffer(T* pBuff) {	operator delete(pBuff);  } // delete raw temporary buffer
	
	
	namespace detail
	{
		template<class InputIterator, class ForwardIterator> inline
		ForwardIterator uninit_copy( InputIterator First, InputIterator Last, ForwardIterator Dest, nonscalar_ptr_iterator_tag )
		{
			typedef typename i3::iterator_traits<ForwardIterator>::value_type		value_type;
			for ( ; First != Last ; ++Dest, ++First )
				::new (&*Dest) value_type(*First);
			return Dest;
		}
		
		template<class T1, class T2> inline
		T2 uninit_copy( T1 First, T1 Last, T2 Dest, scalar_ptr_iterator_tag )
		{
			size_t count = (size_t)(Last - First);
			T2		Result = Dest + count;
			if (count > 0)
				::memmove_s(&*Dest, count * sizeof (*First), &*First, count * sizeof (*First) );
			return Result;
		}
		
		// TEMPLATE FUNCTION _Uninitialized_copy WITH ALLOCATOR
		template<class InputIterator, class ForwardIterator, class Alloc> inline
		ForwardIterator uninit_copy(InputIterator First, InputIterator Last, ForwardIterator Dest, Alloc& Al, nonscalar_ptr_iterator_tag )
		{	
			for (; First != Last; ++Dest, ++First)
				Al.construct(Dest, *First);
			return Dest;
		} // copy [_First, _Last) to raw _Dest, using _Al, arbitrary type

		template<class InputIterator, class ForwardIterator, class Alloc> inline
		ForwardIterator uninit_copy(InputIterator First, InputIterator Last, ForwardIterator Dest, Alloc&, scalar_ptr_iterator_tag )
		{	
			size_t count = (size_t)(Last - First);
			ForwardIterator Result = Dest + count;
			if (count > 0)
				::memmove_s(&*Dest, count * sizeof (*First), &*First, count * sizeof (*First));	// NB: non-overlapping move
			return Result;
		}// copy [_First, _Last) to raw _Dest, scalar type


		template<class ForwardIterator, class Val> inline
		void uninit_fill(ForwardIterator First, ForwardIterator Last, const Val& val, nonscalar_ptr_iterator_tag)
		{	
			typedef typename i3::iterator_traits<ForwardIterator>::value_type		value_type;
			for (; First != Last; ++First)
				::new (&*First) value_type(val);
		}	// copy _Val throughout raw [_First, _Last), arbitrary type

		template<class T, class Val> inline
		void uninit_fill(T* First, T* Last, const Val& val, scalar_ptr_iterator_tag)
		{	
			i3::fill(First, Last, val);
		}	// copy _Val throughout raw [_First, _Last), scalar type
		// std::fill 구현을 i3에 다시 옮길것...		

		
		template<class ForwardIterator, class Diff,	class T> inline
		void uninit_fill_n(ForwardIterator First, Diff Count, const T& Val,	nonscalar_ptr_iterator_tag )
		{	
			for ( ; 0 < Count ; --Count , ++First)
				construct( &*First, Val);
		} // copy _Count *_Val to raw _First, arbitrary type

		template<class ForwardIterator,	class Diff,class T> inline
		void uninit_fill_n(ForwardIterator First, Diff Count, const T& Val,	scalar_ptr_iterator_tag )
		{	
			i3::fill_n(&*First, Count, Val);
		} // copy _Count *_Val to raw _First, scalar type


		template<class ForwardIterator, class Diff,	class T, class Alloc> inline
		void uninit_fill_n(ForwardIterator First, Diff Count, const T& Val, Alloc& Al, nonscalar_ptr_iterator_tag )
		{	
			for (; 0 < Count; --Count, ++First)
				Al.construct(First, Val);
		}

		template<class ForwardIterator,	class Diff, class T, class Alloc> inline
		void uninit_fill_n(ForwardIterator First, Diff Count, const T& Val, Alloc&, scalar_ptr_iterator_tag )
		{	
			i3::fill_n(&*First, Count, Val);
		}

		//
		template<class ForwardIterator, class T, class Alloc> inline
		void uninit_fill_1(ForwardIterator First, const T& Val, Alloc& Al, nonscalar_ptr_iterator_tag )
		{	
			Al.construct(First, Val);
		}

		template<class ForwardIterator,	class T, class Alloc> inline
		void uninit_fill_1(ForwardIterator First, const T& Val, Alloc&, scalar_ptr_iterator_tag )
		{	
			i3::fill_1(&*First, Val);
		}

	}

	template<class InputIterator, class ForwardIterator> inline
	ForwardIterator uninitialized_copy(InputIterator First, InputIterator Last, ForwardIterator Dest)
	{	
		return detail::uninit_copy(First, Last, Dest, pointer_category(First, Dest) );
	}	// copy [_First, _Last) to raw _Dest

	template<class InputIterator, class ForwardIterator, class Alloc> inline
	ForwardIterator uninitialized_copy(InputIterator First, InputIterator Last, ForwardIterator Dest, Alloc& Al)
	{	
		return detail::uninit_copy(First, Last, Dest, Al,	pointer_category(First, Dest) ) ;
	}	// copy [_First, _Last) to raw _Dest, using _Al


	template<class ForwardIterator,	class T> inline
	void uninitialized_fill(ForwardIterator First, ForwardIterator Last, const T& val)
	{	// copy _Val throughout raw [_First, _Last)
		detail::uninit_fill(First, Last, val, pointer_category(First) );
	}
	
	template<class ForwardIterator, class Diff,	class T> inline
	void uninitialized_fill_n(ForwardIterator First, Diff Count, const T& Val)
	{	
		detail::uninit_fill_n(First, Count, Val, pointer_category(First) );
	}	// copy _Count *_Val to raw _First
	
	template<class ForwardIterator, class Diff, class T, class Alloc> inline
	void uninitialized_fill_n(ForwardIterator First, Diff Count, const T& Val, Alloc& Al)
	{	
		detail::uninit_fill_n(First, Count, Val, Al, pointer_category(First) ) ; 
	}	// copy _Count *_Val to raw _First, using _Al

	

	template<class ForwardIterator, class T, class Alloc> inline
	void uninitialized_fill_1(ForwardIterator First, const T& Val, Alloc& Al)
	{	
		detail::uninit_fill_1(First, Val, Al, pointer_category(First) ) ; 
	}	// copy _Count *_Val to raw _First, using _Al


	


}