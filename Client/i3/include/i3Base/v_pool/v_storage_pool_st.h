#pragma once

#include "v_pool.h"
#include "../pool/storage_pool_st.h"

namespace i3
{

	class I3_EXPORT_BASE v_storage_pool_st : public v_pool 
	{
	public:
		v_storage_pool_st() {}
		v_storage_pool_st(std::size_t size, std::size_t num_reserve) : m_pool(size, num_reserve) {}

#if !defined(_DEBUG)
		virtual void*	allocate(size_t size)	{ return i3::storage_pool_st_malloc_no_dbg(&m_pool, size); }
		virtual void	deallocate(void* data) { i3::storage_pool_st_free_no_dbg(&m_pool, data); }
#else
		virtual void*	allocate(size_t size, const char* file, int line) { return i3::storage_pool_st_malloc_dbg(&m_pool, size, file, line); }
		virtual void	deallocate(void* data) { i3::storage_pool_st_free_dbg(&m_pool, data); }
#endif		
	private:	
		i3::storage_pool_st	m_pool;
	};

	class I3_EXPORT_BASE v_storage_pool_st_no_dbg : public v_pool 
	{
	public:
		v_storage_pool_st_no_dbg() {}
		v_storage_pool_st_no_dbg(std::size_t size, std::size_t num_reserve) : m_pool(size, num_reserve) {}

#if !defined(_DEBUG)
		virtual void*	allocate(size_t size )	{ return i3::storage_pool_st_malloc_no_dbg(&m_pool, size); }
#else
		virtual void*	allocate(size_t size, const char* , int ) { return i3::storage_pool_st_malloc_no_dbg(&m_pool, size); }
#endif		
		virtual void	deallocate(void* data) { i3::storage_pool_st_free_no_dbg(&m_pool, data); }
	private:	
		i3::storage_pool_st	m_pool;
	};

}
