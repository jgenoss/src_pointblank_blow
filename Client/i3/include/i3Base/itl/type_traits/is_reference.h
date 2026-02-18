#pragma once

//#include "integral_constant.h"
#include "is_lvalue_reference.h"
#include "is_rvalue_reference.h"

namespace i3
{
//	template<class T>	struct is_reference : false_type {};
//	template<class T>	struct is_reference<T&> : true_type {};		

	template<class T> struct is_reference : mpl::or_< is_lvalue_reference<T>, is_rvalue_reference<T> >::type {};

}
