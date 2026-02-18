#pragma once

#include "v_pool.h"
#include "../pool/object_pool.h"

namespace i3
{

	template<class T>
	class v_object_pool : public v_pool
	{
	public:
#if !defined(_DEBUG)
		virtual void*	allocate(size_t size) {    return i3::object_pool_malloc_no_dbg(&m_pool);         }
		virtual void	deallocate(void* data) {  i3::object_pool_free_no_dbg(&m_pool, data);  }
#else
		virtual void*	allocate(size_t size, const char* file, int line) { return i3::object_pool_malloc_dbg(&m_pool, file, line); }
		virtual void	deallocate(void* data) { i3::object_pool_free_dbg(&m_pool, data); }
#endif
	private:
		i3::object_pool<T>	m_pool;
	};

	template<class T>
	class v_object_pool_no_dbg : public v_pool 
	{
	public:
#if !defined(_DEBUG)
		virtual void*	allocate(size_t size) {    return i3::object_pool_malloc_no_dbg(&m_pool);         }
#else
		virtual void*	allocate(size_t size, const char* , int ) {  return i3::object_pool_malloc_no_dbg(&m_pool);  }
#endif
		virtual void	deallocate(void* data) {  i3::object_pool_free_no_dbg(&m_pool, data);  }
	private:
		i3::object_pool<T>	m_pool;	
	};

}
