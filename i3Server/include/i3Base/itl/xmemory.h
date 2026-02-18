#pragma once

#include "xutility.h"
#include "type_traits/has_trivial_destructor.h"

namespace i3
{
	
	template<class T> inline
	T*  allocate (size_t Count, T*) {	return (T*)::operator new( Count * sizeof(T) );	}
	template<class T> inline
	T*  allocate (size_t Count) {	return (T*)::operator new( Count * sizeof(T) );	}

	template<class T1, class T2> inline
	void construct(T1* Ptr, const T2& Val) {	::new ((void*)Ptr) T1(Val);	}

	template<class T> inline
	void construct(T* ptr) {  ::new (static_cast<void*>(ptr)) T();  }

	namespace detail
	{	
		template<class T> __forceinline
		void destroy_impl( T* , mpl::true_ ){}		//자명소멸자일때
		template<class T> __forceinline
		void destroy_impl( T* Ptr, mpl::false_) {  Ptr->~T();  }	//소멸자호출이 요구되면..
		
		template<class T> __forceinline
		void destroy_impl( T* , T* , mpl::true_ ) {}
		
		template<class T> __forceinline
		void destroy_impl( T* first, T* last, mpl::false_ ) { for( ; first != last ; ++first ) first->~T(); }

	}

	template<class T> __forceinline 
	void destroy(T* Ptr) {   detail::destroy_impl( Ptr, typename i3::mpl::bool_<i3::has_trivial_destructor<T>::value>::type() );   }
	
	template<class T> __forceinline
	void destroy(T* first, T* last ) { detail::destroy_impl( first, last, typename i3::mpl::bool_<i3::has_trivial_destructor<T>::value>::type()); }

//	template<class T> inline void destroy(T* Ptr) {	Ptr->~T(); }
//	template<> inline void destroy(char*) {}
//	template<> inline void destroy(wchar_t*){}


	// TEMPLATE FUNCTION Destroy_range
	namespace detail
	{
		template<class Alloc> inline
		void destroy_range(typename Alloc::pointer First, typename Alloc::pointer Last, Alloc& Al, nonscalar_ptr_iterator_tag)
		{	
			for (; First != Last; ++First) Al.destroy(First);
		}
		template<class Alloc> inline
		void destroy_range(typename Alloc::pointer, typename Alloc::pointer,Alloc& , scalar_ptr_iterator_tag){}

		template<class Alloc> inline
		void destroy_range_1(typename Alloc::pointer First, Alloc& Al, nonscalar_ptr_iterator_tag) { Al.destroy(First); }

		template<class Alloc> inline
		void destroy_range_1(typename Alloc::pointer, Alloc& , scalar_ptr_iterator_tag){}

	}


	template<class Alloc> inline
	void destroy_range(typename Alloc::pointer First, typename Alloc::pointer Last, Alloc& Al)
	{	detail::destroy_range(First, Last, Al, pointer_category(First) );	}	


	template<class Alloc> inline
	void destroy_range_1(typename Alloc::pointer First, Alloc& Al)
	{	detail::destroy_range_1(First, Al, pointer_category(First) );	}	


}