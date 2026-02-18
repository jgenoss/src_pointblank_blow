#pragma once

#include "v_pool.h"
#include "../pool/storage_pool_mt.h"

namespace i3
{
	class I3_EXPORT_BASE v_storage_pool_mt : public v_pool 
	{
	public:
		v_storage_pool_mt() {} 
		v_storage_pool_mt(std::size_t num_reserve) : m_pool(num_reserve) {}
		~v_storage_pool_mt() {}

#if !defined(_DEBUG)
		virtual void*	allocate(size_t size)	{ return i3::storage_pool_mt_malloc_no_dbg(&m_pool, size); }
		virtual void	deallocate(void* data) { i3::storage_pool_mt_free_no_dbg(&m_pool, data); }
#else
		virtual void*	allocate(size_t size, const char* file, int line) { return i3::storage_pool_mt_malloc_dbg(&m_pool, size, file, line); }
		virtual void	deallocate(void* data) { i3::storage_pool_mt_free_dbg(&m_pool, data); }
#endif		
	private:	
		i3::storage_pool_mt	m_pool;
	};


	class I3_EXPORT_BASE v_storage_pool_mt_no_dbg : public v_pool 
	{
	public:
		v_storage_pool_mt_no_dbg() {} 
		v_storage_pool_mt_no_dbg(std::size_t num_reserve) : m_pool(num_reserve) {}
		~v_storage_pool_mt_no_dbg() {}
#if !defined(_DEBUG)
		virtual void*	allocate(size_t size )	{ return i3::storage_pool_mt_malloc_no_dbg(&m_pool, size); }
#else
		virtual void*	allocate(size_t size, const char* , int ) { return i3::storage_pool_mt_malloc_no_dbg(&m_pool, size); }
#endif		
		virtual void	deallocate(void* data) { i3::storage_pool_mt_free_no_dbg(&m_pool, data); }
	private:	
		i3::storage_pool_mt	m_pool;
	};

}
