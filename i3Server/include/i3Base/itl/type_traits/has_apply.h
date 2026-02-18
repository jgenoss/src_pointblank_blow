#pragma once

namespace i3
{
	template<class T> struct has_apply;
}

#define has_xxx		(has_apply, apply)
#include "has_xxx.h"
