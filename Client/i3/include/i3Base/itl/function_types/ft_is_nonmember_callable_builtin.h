#pragma once

#include "ft_components.h"

namespace i3
{
	namespace ft
	{
		template<class T, class Tag = null_tag>
		struct is_nonmember_callable_builtin : represents< components<T>, tag<Tag, nonmember_callable_builtin_tag> > {};

	}
}