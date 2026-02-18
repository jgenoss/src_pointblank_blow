#pragma once

#include <cstddef>

namespace i3
{
	size_t  calculate_adjusted_align( size_t align);

	template<size_t align>
	struct	static_calculate_adjusted_align {	static const std::size_t value = ( align > sizeof(void*) ) ? align : sizeof(void*);	};

	__forceinline
	size_t  calculate_adjusted_align( size_t align) {	return ( align > sizeof(void*) ) ? align : sizeof(void*); }
	
}
