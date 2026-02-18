#pragma once

#include "cv_traits_impl.h"
#include "integral_constant.h"


namespace i3
{
	template <class T> struct	is_volatile : integral_constant<bool, detail::cv_traits_imp_ptr<T*>::is_volatile> {};	
	template<class T>  struct	is_volatile<T&> : false_type {}; // 참조는 volatile 없음..
}

