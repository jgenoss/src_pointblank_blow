#pragma once

namespace i3
{
	template<class T> struct has_result_type;
}

#define has_xxx		(has_result_type, result_type)
#include "has_xxx.h"
