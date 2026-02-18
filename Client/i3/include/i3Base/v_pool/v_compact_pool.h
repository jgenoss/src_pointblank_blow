#pragma once

#include "v_pool.h"
#include "../pool/compact_pool.h"


namespace i3
{
	class I3_EXPORT_BASE v_compact_pool : public v_pool 
	{
	public:
		v_compact_pool(std::size_t obj_size) : m_pool(obj_size, true) {} 
		~v_compact_pool() {}

#if !defined(_DEBUG)
		virtual void*	allocate(size_t size)	{ return i3::compact_pool_malloc(&m_pool); }
		virtual void	deallocate(void* data) { i3::compact_pool_free(&m_pool, data); }
#else
		virtual void*	allocate(size_t size, const char* file, int line) { return i3::compact_pool_malloc_dbg(&m_pool, file, line); }
		virtual void	deallocate(void* data) { i3::compact_pool_free_dbg(&m_pool, data); }
#endif		
	private:	
		i3::compact_pool	m_pool;
	};


	class I3_EXPORT_BASE v_compact_pool_no_dbg : public v_pool 
	{
	public:
		v_compact_pool_no_dbg(std::size_t obj_size) : m_pool(obj_size, false) {} 
#if !defined(_DEBUG)
		virtual void*	allocate(size_t size )	{ return i3::compact_pool_malloc(&m_pool); }
#else
		virtual void*	allocate(size_t size, const char* f, int l) { return i3::compact_pool_malloc_dbg(&m_pool, f, l); }
#endif		
		virtual void	deallocate(void* data) { i3::compact_pool_free(&m_pool, data); }
	private:	
		i3::compact_pool	m_pool;
	};

}

	
	
	
