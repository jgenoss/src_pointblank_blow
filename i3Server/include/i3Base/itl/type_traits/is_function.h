#pragma once


#ifndef _MSC_EXTENSIONS
#include "is_function_pointer_helper.h"
#else
#include "is_function_pointer_tester.h"
#endif

namespace i3
{
	
	template <class T> 
	struct is_function : detail::is_function_impl<T>::type {};
	
}
