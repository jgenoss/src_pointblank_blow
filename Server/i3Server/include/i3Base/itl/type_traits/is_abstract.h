#pragma once

#include "integral_constant.h"

namespace i3
{

	template<class T>
	struct is_abstract : integral_constant<bool, __is_abstract(T) > {};
	
}
