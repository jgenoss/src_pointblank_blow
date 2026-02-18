#pragma once

#include "make_nonconst_referred.h"

namespace i3
{
	// make_const_reference
	template <class T> struct make_const_reference 
	{
		typedef const typename make_nonconst_referred<T>::type&				type;
	};
}
