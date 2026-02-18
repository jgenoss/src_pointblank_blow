#include "i3CommonType.h"
#include "i3Base/pool/compact_pool.h"
#include "i3Debug.h"

namespace i3
{
#if defined(_DEBUG) && defined(I3_POOL_DBG)	
	void*	compact_pool_malloc_dbg_enable(compact_pool* pool_obj, const char* f, int l)
	{
		void* raw_data = pool_obj->m_pool.get()->allocate(pool_obj);
			
		return i3::apply_dbg_data(f, l, raw_data, pool_obj->m_orig_size);
	}

	void*	compact_pool_malloc_dbg_disable(compact_pool* pool_obj, const char*, int )
	{
		return pool_obj->m_pool.get()->allocate(pool_obj);
	}

	void	compact_pool_free_dbg_enable(compact_pool* pool_obj, void* data)
	{
		pool_obj->m_pool.get()->deallocate( i3::cleanup_raw_data_from_dbg_data( data ) );
	}

	void	compact_pool_free_dbg_disable(compact_pool* pool_obj, void* data )
	{
		pool_obj->m_pool.get()->deallocate(data);
	}
#endif

	compact_pool::compact_pool(std::size_t obj_size, bool enable_dbg)  
	{
#if defined(_DEBUG) && defined(I3_POOL_DBG)	
		m_orig_size = obj_size;
		if (enable_dbg)
		{
			obj_size = i3::calculate_dbg_data_size( obj_size );
			m_fn_malloc = &i3::compact_pool_malloc_dbg_enable;
			m_fn_free   = &i3::compact_pool_free_dbg_enable;
		}
		else
		{
			m_fn_malloc = &i3::compact_pool_malloc_dbg_disable;
			m_fn_free   = &i3::compact_pool_free_dbg_disable;
		}
#else
		enable_dbg;
#endif		
		I3ASSERT( compact_pool::is_valid_size(obj_size) );
		m_actual_size = obj_size; //i3::convert_size_to_actual_size( obj_size);
	}

}
