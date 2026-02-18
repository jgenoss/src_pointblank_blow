#pragma once

#include "ft_components.h"

namespace i3
{
	namespace ft
	{
		template<class T>
		struct is_member_object_pointer : represents<components<T>, member_object_pointer_tag> {};

	}
}
