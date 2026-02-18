#pragma once

namespace i3
{
	template<class T> struct has_reference;
}

#define has_xxx		(has_reference, reference)
#include "has_xxx.h"
