#pragma once

#include "remove_reference.h"

namespace i3
{
	template <class T> struct add_pointer {	typedef typename remove_reference<T>::type* type; };
}

