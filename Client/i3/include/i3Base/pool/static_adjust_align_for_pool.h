#pragma once

#include "../itl/type_traits/alignment_of.h"

namespace i3
{
	template<class T>
	struct static_adjust_align_for_pool : static_calculate_adjusted_align< alignment_of<T>::value > {};
}
