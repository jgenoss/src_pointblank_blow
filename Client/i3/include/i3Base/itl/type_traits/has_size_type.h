#pragma once

namespace i3
{
	template<class T> struct has_size_type;
}

#define has_xxx		(has_size_type, size_type)
#include "has_xxx.h"
