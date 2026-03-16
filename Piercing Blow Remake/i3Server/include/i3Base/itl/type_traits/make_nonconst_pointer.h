#pragma once

#include "add_pointer.h"
#include "remove_pointer.h"
#include "make_nonconst_referred.h"

namespace i3
{

	template <class T> struct make_nonconst_pointer
	{
		typedef typename add_pointer< typename remove_pointer< typename make_nonconst_referred<T>::type>::type>::type	type;
	};

}
