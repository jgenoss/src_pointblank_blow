#pragma once

#include "msb.h"
#include "static_msb.h"

namespace i3
{
	
	size_t		convert_size_to_size_idx( size_t size );
	size_t		convert_size_to_actual_size( size_t size);
	size_t		convert_size_idx_to_actual_size(size_t size_index);

	template<size_t size>	struct static_convert_size_to_size_idx;
	template<size_t size>	struct static_convert_size_to_actual_size;
	template<size_t size_index>		struct static_convert_size_idx_to_actual_size;
		
			
}


#include "pool_size_helper.inl"