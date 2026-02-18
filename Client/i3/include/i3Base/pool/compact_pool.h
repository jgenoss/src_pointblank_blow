#pragma once

#include "compact_pool_by_thread.h"
#include "maximum_object_size.h"

namespace i3
{
	
	class compact_pool;

	I3_EXPORT_BASE void*	compact_pool_malloc(compact_pool* pool_obj);
	I3_EXPORT_BASE void	compact_pool_free(compact_pool* pool_obj, void* data);

#if defined(_DEBUG) && defined(I3_POOL_DBG)	
	I3_EXPORT_BASE void*	compact_pool_malloc_dbg( compact_pool* pool_obj, const char* file, int line );
	I3_EXPORT_BASE void	compact_pool_free_dbg(compact_pool* pool_obj, void* data);
#endif

	// 사이즈가 블록에 적합하지 않을 정도로 크면, I3ASSERT같은 것으로 세울 것...
	class I3_EXPORT_BASE compact_pool 
	{
	public:
		compact_pool(std::size_t obj_size, bool enable_dbg = true);

		static bool		is_valid_size(size_t obj_size, bool enable_dbg = true); 

		friend I3_EXPORT_BASE void*	compact_pool_malloc(compact_pool* pool_obj);
		friend I3_EXPORT_BASE void	compact_pool_free(compact_pool* pool_obj, void* data);
				
#if defined(_DEBUG) && defined(I3_POOL_DBG)	
		friend I3_EXPORT_BASE void*	compact_pool_malloc_dbg( compact_pool* pool_obj, const char* file, int line );
		friend I3_EXPORT_BASE void	compact_pool_free_dbg(compact_pool* pool_obj, void* data);

		friend void*	compact_pool_malloc_dbg_enable(compact_pool* pool_obj, const char* f, int l);
		friend void*	compact_pool_malloc_dbg_disable(compact_pool* pool_obj, const char*, int );
		friend void		compact_pool_free_dbg_enable(compact_pool* pool_obj, void* data);
		friend void		compact_pool_free_dbg_disable(compact_pool* pool_obj, void* );
#endif

	private:
		void*			allocate();
		void			deallocate( void* data ); 

		std::size_t		actual_size()	const { return m_actual_size; }
	
		i3::tls_ptr< compact_pool_by_thread >	m_pool;
		std::size_t								m_actual_size;
	
#if defined(_DEBUG) && defined(I3_POOL_DBG)
		std::size_t								m_orig_size;
		void*									(*m_fn_malloc)(compact_pool*, const char*, int);
		void									(*m_fn_free)(compact_pool*, void*);
#endif		
		friend class	compact_pool_by_thread;
	};

#if defined(_DEBUG) && defined(I3_POOL_DBG)	

#endif


	
#if defined(_DEBUG) && defined(I3_POOL_DBG)	

	inline bool compact_pool::is_valid_size(size_t obj_size, bool enable_dbg ) 
	{ 
		if (!enable_dbg) return obj_size < maximum_object_size; 
		return i3::calculate_dbg_data_size( obj_size ) < maximum_object_size; 
	}

#else

	inline bool compact_pool::is_valid_size(size_t obj_size, bool )
	{ 
		return obj_size < maximum_object_size; 
	}

#endif
		
	__forceinline
	void*	compact_pool::allocate() 
	{
		return m_pool.get()->allocate(this);
	}

	__forceinline
	void	compact_pool::deallocate( void* data )
	{
		m_pool.get()->deallocate(data);
	}
	
	inline
	void*	compact_pool_malloc(compact_pool* pool_obj)
	{
		return pool_obj->allocate();
	}

	inline
	void	compact_pool_free(compact_pool* pool_obj, void* data)
	{
		if (data) pool_obj->deallocate(data);
	}
		
#if defined(_DEBUG) && defined(I3_POOL_DBG)	
	inline 
	void*	compact_pool_malloc_dbg( compact_pool* pool_obj, const char* file, int line )
	{
		return (*pool_obj->m_fn_malloc)(pool_obj, file, line);
	}

	inline 
	void	compact_pool_free_dbg(compact_pool* pool_obj, void* data)
	{
		(*pool_obj->m_fn_free)(pool_obj, data);
	}
#endif		


}


namespace i3
{

#if defined(_DEBUG) && defined(I3_POOL_DBG)

#define compact_pool_malloc(pool_obj)					compact_pool_malloc_dbg(pool_obj, __FILE__, __LINE__)
#define compact_pool_free(pool_obj, data)				compact_pool_free_dbg(pool_obj, data)

#else

#define compact_pool_malloc_dbg(pool_obj, f, l )		compact_pool_malloc(pool_obj)
#define compact_pool_free_dbg(pool_obj, data)			compact_pool_free( pool_obj, data)

#endif

}
