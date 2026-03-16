#pragma once

namespace i3
{
	template<class T> struct has_value_type;
}

#define has_xxx		(has_value_type, value_type)
#include "has_xxx.h"
