#pragma once


#ifndef _MSC_EXTENSIONS
#include "is_member_function_pointer_helper.h"
#else
#include "is_member_function_pointer_tester.h"	
#endif



namespace i3
{
	template <class T>	struct is_member_function_pointer : detail::is_member_function_pointer_impl<T> {};

}
