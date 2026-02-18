#pragma once

#include "pool_dbg_user_config.h"

#if defined(_DEBUG) && defined(I3_POOL_DBG)

#include "../memory/calculate_adjusted_align.h"

namespace i3
{
	size_t		calculate_adjusted_align( size_t align);

	I3_EXPORT_BASE
	size_t		calculate_aligned_dbg_data_size( size_t size, size_t adjusted_align);
	I3_EXPORT_BASE
	void*		apply_aligned_dbg_data( const char* szFilename, int line, void* raw_data, 
		size_t adjusted_align, size_t orig_size);
	I3_EXPORT_BASE
	void*		cleanup_raw_data_from_aligned_dbg_data( void* dbg_data);
	
	I3_EXPORT_BASE
	size_t		calculate_dbg_data_size( size_t size);
	I3_EXPORT_BASE
	void*		apply_dbg_data( const char* szFilename, int line, void* raw_data, size_t orig_size);
	I3_EXPORT_BASE
	void*		cleanup_raw_data_from_dbg_data( void* dbg_data);

}

#endif	
