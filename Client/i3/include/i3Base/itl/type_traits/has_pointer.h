#pragma once

namespace i3
{
	template<class T> struct has_pointer;
}

#define has_xxx		(has_pointer, pointer)
#include "has_xxx.h"
