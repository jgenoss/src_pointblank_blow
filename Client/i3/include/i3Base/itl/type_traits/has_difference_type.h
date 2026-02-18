#pragma once

namespace i3
{
	template<class T> struct has_difference_type;
}

#define has_xxx		(has_difference_type, difference_type)
#include "has_xxx.h"
