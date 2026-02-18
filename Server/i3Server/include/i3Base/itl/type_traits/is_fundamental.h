#pragma once

#include "is_arithmetic.h"
#include "is_void.h"
#include "../mpl/mpl_or.h"

namespace i3
{
	template<class T>
	struct is_fundamental : integral_constant<bool, mpl::or_<is_arithmetic<T>, is_void<T> >::value > {};
}

