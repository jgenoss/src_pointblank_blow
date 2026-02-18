#pragma once

#include "is_array.h"
#include "is_function.h"
#include "remove_extent.h"
#include "add_pointer.h"
#include "remove_reference.h"

#include "mpl/mpl_if.h"
#include "mpl/mpl_identity.h"

namespace i3
{
	template<class T>
	struct decay
	{
		typedef typename remove_reference<T>::type  unref;
		typedef typename mpl::eval_if<  is_array<unref>, 
										mpl::identity<typename remove_extent<unref>::type*>,
										mpl::eval_if<is_function<unref>, add_pointer<unref>, mpl::identity<unref> > >::type type;
	};

}