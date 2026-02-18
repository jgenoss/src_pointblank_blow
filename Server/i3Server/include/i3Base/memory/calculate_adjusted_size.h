#pragma once

namespace i3
{
	size_t	calculate_adjusted_size( size_t size);

	template<size_t size>
	struct  static_calculate_adjusted_size {  static const size_t value = ( size > sizeof(size_t) ) ? size : sizeof(size_t);  };

	__forceinline
	size_t	calculate_adjusted_size( size_t size) { return ( size > sizeof(size_t) ) ? size : sizeof(size_t); }
}

