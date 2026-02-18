#pragma once

#include "ft_components.h"

namespace i3
{
	namespace ft 
	{
		template<class T, class Tag = null_tag>
		struct is_member_pointer : represents< components<T>, tag<Tag, member_pointer_tag> > {};
	}
}
