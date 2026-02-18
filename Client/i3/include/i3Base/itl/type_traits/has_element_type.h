#pragma once

namespace i3
{
	template<class T> struct has_element_type;
}

#define has_xxx		(has_element_type, element_type)
#include "has_xxx.h"
