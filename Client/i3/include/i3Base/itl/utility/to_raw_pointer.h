#pragma once

#include "../type_traits/pointer_traits.h"

namespace i3
{
	template<class T> __forceinline
	T* to_raw_pointer(T* p) { return p; }
	
	template<class Ptr> __forceinline 
	typename i3::pointer_traits<Ptr>::element_type* to_raw_pointer(const Ptr& p) { return to_raw_pointer( p.operator->()); }
	
}
