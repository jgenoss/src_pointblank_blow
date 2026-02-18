#pragma once

#include "is_integral.h"
#include "is_floating_point.h"
#include "../mpl/mpl_or.h"

namespace i3
{
	
	template<class T>
	struct is_arithmetic : integral_constant<bool, mpl::or_<is_integral<T>, is_floating_point<T> >::value> {};
	
}
