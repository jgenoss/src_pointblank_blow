#pragma once

#include "ft_synthesize.h"
#include "ft_to_sequence.h"

namespace i3
{
	namespace ft
	{
		template<class Types, class Tag = null_tag>
		struct member_function_pointer : synthesize_member_function_pointer< typename to_sequence<Types>::type, Tag> {};
	}
}
