#pragma once

#include "v_pool.h"
#include "../pool/static_pool.h"

namespace i3
{

	class I3_EXPORT_BASE v_common_pool : public v_pool 
	{
	public:
#if !defined(_DEBUG)
		virtual void*	allocate(size_t size) { return i3::static_pool_malloc_no_dbg(size); }
		virtual void	deallocate(void* data) { i3::static_pool_free_no_dbg(data);  }
#else
		virtual void*	allocate(size_t size, const char* file, int line) { return i3::static_pool_malloc_dbg(size, file, line); }
		virtual void	deallocate(void* data) { i3::static_pool_free_dbg(data); }
#endif
	};

	class I3_EXPORT_BASE v_common_pool_no_dbg : public v_pool 
	{
	public:
#if !defined(_DEBUG)
		virtual void*	allocate(size_t size) { return i3::static_pool_malloc_no_dbg(size); }
#else
		virtual void*	allocate(size_t size, const char* , int ) { return i3::static_pool_malloc_no_dbg(size); }
#endif
		virtual void	deallocate(void* data) { i3::static_pool_free_no_dbg(data);  }
	};

}
