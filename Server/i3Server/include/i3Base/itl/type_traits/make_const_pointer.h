#pragma once

#include "add_pointer.h"
#include "add_const.h"
#include "remove_pointer.h"
#include "make_nonconst_referred.h"

namespace i3
{
	
	template <class T> struct make_const_pointer
	{
		typedef typename add_pointer< typename add_const< typename remove_pointer< typename make_nonconst_referred<T>::type>::type>::type>::type	type;
	};
	
	

}