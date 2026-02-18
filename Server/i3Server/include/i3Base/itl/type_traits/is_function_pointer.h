#pragma once

#include "is_function.h"
#include "is_pointer.h"
#include "remove_pointer.h"
#include "integral_constant.h"
#include "../mpl/mpl_and.h"

namespace i3
{
	
	template <class T>
	struct is_function_pointer 
		: integral_constant<bool, mpl::and_<is_pointer<T>, is_function< typename remove_pointer<T>::type > >::value> {};
		
		
}
