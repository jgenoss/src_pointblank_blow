#pragma once

#include "make_nonconst_referred.h"

namespace i3
{
	
	// make_nonconst_reference 
	template <class T> struct make_nonconst_reference 
	{
		typedef typename make_nonconst_referred<T>::type&					type;
	};

}
