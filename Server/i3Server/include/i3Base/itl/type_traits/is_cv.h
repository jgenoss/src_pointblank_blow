#pragma once

#include "is_const.h"
#include "is_volatile.h"
#include "../mpl/mpl_and.h"


namespace i3
{
	template<class T> struct is_cv : integral_constant<bool, mpl::and_<is_const<T>, is_volatile<T> >::value> {};
}
