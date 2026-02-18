#pragma once

//#include "xutility.h"
#include "type_traits/has_trivial_destructor.h"

namespace i3
{
	
	template<class T> inline
	T*  allocate (size_t Count, T*)
	{
		T* p = (T*)::operator new( Count * sizeof(T) );
		NetworkDump_Malloc( p, Count * sizeof(T), __FILE__, __LINE__);
		return p;
	}
	template<class T> inline
	T*  allocate (size_t Count)
	{
		T* p = (T*)::operator new( Count * sizeof(T) );
		NetworkDump_Malloc( p, Count * sizeof(T), __FILE__, __LINE__);
		return p;
	}

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


	// TEMPLATE FUNCTION Destroy_range
	namespace detail
	{
		template<class Alloc> inline
		void destroy_range(typename Alloc::pointer First, typename Alloc::pointer Last, Alloc& Al, mpl::false_)
		{	
			for (; First != Last; ++First) Al.destroy(First);
		}
		template<class Alloc> inline
		void destroy_range(typename Alloc::pointer, typename Alloc::pointer,Alloc& , mpl::true_){}

		template<class Alloc> inline
		void destroy_range_1(typename Alloc::pointer First, Alloc& Al, mpl::false_) { Al.destroy(First); }

		template<class Alloc> inline
		void destroy_range_1(typename Alloc::pointer, Alloc& , mpl::true_){}
	}


	template<class Alloc> inline
	void destroy_range(typename Alloc::pointer First, typename Alloc::pointer Last, Alloc& Al)
	{	detail::destroy_range(First, Last, Al, 
		typename i3::mpl::bool_<i3::has_trivial_destructor<typename Alloc::value_type>::value>::type() ) ;	}	


	template<class Alloc> inline
	void destroy_range_1(typename Alloc::pointer First, Alloc& Al)
	{	detail::destroy_range_1(First, Al, 
		typename i3::mpl::bool_<i3::has_trivial_destructor<typename Alloc::value_type>::value>::type() );	}	


}