#pragma once

#include "get_file_attributes.h"

namespace i3
{

	namespace detail
	{
		template<class Range> __forceinline
		bool is_directory_imp(const Range& rng) 
		{
			DWORD attr = get_file_attributes(rng);
			if ( attr == INVALID_FILE_ATTRIBUTES) return false;
			if( attr & FILE_ATTRIBUTE_DIRECTORY) return true;
			return false;
		}
	}

	template<class Range> __forceinline
	bool is_directory(const Range& rng) { return detail::is_directory_imp(i3::as_literal(rng)); }

}
