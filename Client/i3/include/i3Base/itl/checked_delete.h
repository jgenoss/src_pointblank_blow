#pragma once

namespace i3
{
		
//  boost에서 그대로 갖고옴...

	template<class T> __forceinline 
	void checked_delete(T* x)
	{
		typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
		(void) sizeof(type_must_be_complete);
#if defined( I3_DEBUG)
		NetworkDump_Free( x, __FILE__, __LINE__);
#endif
		delete x;
	}

	template<class T> __forceinline 
	void checked_array_delete(T* x)
	{
		typedef char type_must_be_complete[ sizeof(T)? 1: -1 ];
		(void) sizeof(type_must_be_complete);
#if defined( I3_DEBUG)
		NetworkDump_Free( x, __FILE__, __LINE__);
#endif
		delete [] x;
	}

	template<class T> struct checked_deleter
	{
		typedef void result_type;	typedef T * argument_type;
		__forceinline void operator()(T* x) const {  checked_delete(x);	}
	};

	template<class T> struct checked_array_deleter
	{
		typedef void result_type;    typedef T * argument_type;
		__forceinline void operator()(T* x) const {	checked_array_delete(x);  }
	};


}