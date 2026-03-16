#pragma once

#include "cv_traits_impl.h"
#include "integral_constant.h"

namespace i3
{
	
	//  최상위것만 따짐...(레퍼런스나 포인터의 경우엔 제거하고 접근 할것)
	template<class T> struct is_const : integral_constant<bool, detail::cv_traits_imp_ptr<T*>::is_const> {};
	template<class T> struct is_const<T&> : false_type {}; 		// 참조는 const가 없음..
	//

}