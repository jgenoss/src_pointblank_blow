#pragma once

#include "is_reference.h"
#include "is_void.h"
#include "is_function.h"

#include "../mpl/mpl_and.h"
#include "../mpl/mpl_not.h"

namespace i3
{
	template<class T>
	struct is_object : integral_constant<bool, mpl::and_<	mpl::not_<is_reference<T> >, 
															mpl::not_<is_void<T> >, 
															mpl::not_<is_function<T> > >::value> {};

}

