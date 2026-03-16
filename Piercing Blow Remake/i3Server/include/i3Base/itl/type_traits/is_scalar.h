#pragma once

#include "is_arithmetic.h"
#include "is_enum.h"
#include "is_pointer.h"
#include "is_member_pointer.h"
#include "../mpl/mpl_or.h"

namespace i3
{
	// 스칼라는 void타입을 제외한 자명한 (빌트인타입/포인터류/enum)타입들을 의미함..
	template <typename T>
	struct is_scalar : integral_constant<bool, mpl::or_<is_arithmetic<T>, is_enum<T>, is_pointer<T>, is_member_pointer<T> >::value> {};
	template <> struct is_scalar<void> : false_type {}; 
	template <> struct is_scalar<void const> : false_type {}; 
	template <> struct is_scalar<void volatile> : false_type {}; 
	template <> struct is_scalar<void const volatile> : false_type {}; 

}
