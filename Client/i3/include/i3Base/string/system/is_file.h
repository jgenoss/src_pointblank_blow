#pragma once

#include "get_file_attributes.h"

namespace i3
{
	
	namespace detail
	{
		template<class Range> __forceinline
		bool is_file_imp(const Range& rng) 
		{
			DWORD attr = get_file_attributes(rng);
			if ( attr == INVALID_FILE_ATTRIBUTES) return false;
			if( attr & FILE_ATTRIBUTE_DIRECTORY) return false;
			return true;
		}
	}
	
	template<class Range> __forceinline
	bool is_file(const Range& rng) { return detail::is_file_imp(i3::as_literal(rng)); }

}