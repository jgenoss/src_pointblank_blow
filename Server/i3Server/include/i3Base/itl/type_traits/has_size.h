#pragma once

namespace i3
{
	template<class T> struct has_size;
}

#define has_xxx		(has_size, size)
#include "has_xxx.h"
