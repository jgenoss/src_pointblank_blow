#pragma once

#include "remove_cv.h"
#include "remove_reference.h"

namespace i3
{

// make_nonconst_referred
	template <class T> struct make_nonconst_referred 
	{
		typedef typename remove_cv< typename remove_reference<T>::type >::type type;
	};
}