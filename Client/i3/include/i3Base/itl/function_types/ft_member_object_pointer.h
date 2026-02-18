#pragma once

#include "ft_synthesize.h"
#include "ft_to_sequence.h"

namespace i3
{
	namespace ft
	{
		template<class Types>
		struct member_object_pointer : synthesize_member_object_pointer< typename to_sequence<Types>::type>	{};
	}
}
