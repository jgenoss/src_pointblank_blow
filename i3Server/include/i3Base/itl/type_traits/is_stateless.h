#pragma once

#include "has_trivial_constructor.h"
#include "has_trivial_copy.h"
#include "has_trivial_destructor.h"
#include "is_class.h"
#include "is_empty.h"

#include "../mpl/mpl_and.h"

namespace i3
{
	template <class T>
	struct is_stateless : integral_constant<bool, mpl::and_< has_trivial_constructor<T>, has_trivial_copy<T>, 
		has_trivial_destructor<T>, is_class<T>, is_empty<T> >::value > {};
}

