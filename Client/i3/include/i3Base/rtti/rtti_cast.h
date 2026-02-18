#pragma once

#include "get_rtti_tag.h"
#include "../itl/compile_assert.h"
#include "detail/rtti_cast_impl.h"

namespace i3
{

	template<class Ptr, class T> __forceinline
	bool	same_of( const T* p ) 
	{  
		compile_assert_pred( (i3::is_pointer<Ptr>) );
		typedef typename i3::remove_pointer<Ptr>::type	U;
		typedef detail::get_dynamic_rtti_info<T, typename get_rtti_tag<T>::type>	src_f;
		typedef detail::get_static_rtti_info<U, typename get_rtti_tag<U>::type>		compare_f;
	
		return (p) ? src_f::call(p)->same_of( compare_f::call() ) : false;
	}

	template<class Ptr, class T> __forceinline
	bool	kind_of( const T* p ) 
	{  
		compile_assert_pred( (i3::is_pointer<Ptr>) );
		typedef typename i3::remove_pointer<Ptr>::type	U;
		typedef detail::get_dynamic_rtti_info<T, typename get_rtti_tag<T>::type>	src_f;
		typedef detail::get_static_rtti_info<U, typename get_rtti_tag<U>::type>		compare_f;
	
		return (p) ? src_f::call(p)->kind_of( compare_f::call() ) : false;
	}

	template<class Ptr, class T> __forceinline
	Ptr		same_cast( T* p ) {   return (same_of<Ptr>(p) ) ? static_cast<Ptr>(p) : nullptr; }

	template<class Ptr, class T> __forceinline
	Ptr		kind_cast( T* p) {    return (kind_of<Ptr>(p) ) ? static_cast<Ptr>(p) : nullptr; }

}

// same_of¿Í kind_of È®Àå..
#include "rtti_cast_ext.h"
//

