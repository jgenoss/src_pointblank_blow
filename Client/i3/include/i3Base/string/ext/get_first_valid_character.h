#pragma once

// i3String::GetFirstValidCharater ¿œπ›»≠..

#include "make_generic_literal.h"
#include "generic_find_first_not_of.h"

namespace i3
{
	template<class Rng> inline typename i3::range_iterator<Rng>::type
	get_first_valid_character(const Rng& str)
	{
		return i3::generic_find_first_not_of( str, i3::make_generic_literal(Ch, " \t\r\n") );
	}
	
}
