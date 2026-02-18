#pragma once


#include "ft_synthesize.h"
#include "ft_to_sequence.h"


namespace i3
{
	namespace ft
	{
		template<class Types, class Tag = null_tag>
		struct function_reference : synthesize_function_reference< typename to_sequence<Types>::type, Tag> {};
	}
}
