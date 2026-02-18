#pragma once

#include "ft_components.h"

namespace i3
{
	namespace ft
	{
		template<class T, class Tag = null_tag>
		struct is_function_reference : represents<components<T>, tag<Tag, reference_tag> > {};
	}
}
