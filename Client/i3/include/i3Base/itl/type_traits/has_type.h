#pragma once

namespace i3
{
	template<class T> struct has_type;
}

#define has_xxx		(has_type, type)
#include "has_xxx.h"
