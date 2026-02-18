#pragma once

#include "cache_pool_impl.h"

namespace i3
{
		
	class storage_pool_st;

	I3_EXPORT_BASE void*	storage_pool_st_malloc(storage_pool_st* pool_obj, std::size_t obj_size);
	I3_EXPORT_BASE void	storage_pool_st_free(storage_pool_st* pool_obj, void* data);

	I3_EXPORT_BASE void*	storage_pool_st_malloc_no_dbg(storage_pool_st* pool_obj, std::size_t obj_size);
	I3_EXPORT_BASE void	storage_pool_st_free_no_dbg(storage_pool_st* pool_obj, void* data);


	class I3_EXPORT_BASE storage_pool_st : public detail::cache_pool_st_impl
	{
	public:
		typedef detail::cache_pool_st_impl	base_type;

		storage_pool_st(std::size_t obj_size, std::size_t num_reserve) : base_type( obj_size, num_reserve) {}
		storage_pool_st() : base_type() {}
	
		friend	I3_EXPORT_BASE void*	storage_pool_st_malloc(storage_pool_st* pool_obj, std::size_t size);
		friend	I3_EXPORT_BASE void	storage_pool_st_free(storage_pool_st* pool_obj, void* data);

	private:
		void*		allocate(std::size_t obj_size) 
		{
			void* result = base_type::allocate_impl();
			if ( result != 0 ) return result;
			return base_type::allocate_from_static_pool(obj_size);
		}

		void		deallocate(void* data) { base_type::deallocate_impl(data); }
	};

	__forceinline
	void*	storage_pool_st_malloc(storage_pool_st* pool_obj, std::size_t obj_size)
	{
		return pool_obj->allocate(obj_size);
	}

	__forceinline
	void	storage_pool_st_free(storage_pool_st* pool_obj, void* data)
	{
		if (data) pool_obj->deallocate(data);
	}

	__forceinline 
	void*	storage_pool_st_malloc_no_dbg(storage_pool_st* pool_obj, std::size_t obj_size)
	{
		return storage_pool_st_malloc(pool_obj, obj_size);
	}

	__forceinline 
	void	storage_pool_st_free_no_dbg(storage_pool_st* pool_obj, void* data)
	{
		storage_pool_st_free(pool_obj, data);
	}

	__forceinline I3_EXPORT_BASE
	void*	storage_pool_st_malloc_dbg( storage_pool_st* pool_obj, std::size_t obj_size, const char* file, int line )
	{
		size_t ext_num = i3::calculate_dbg_data_size( obj_size );		// 사이즈교체..
		void* raw_data = storage_pool_st_malloc(pool_obj, ext_num);
		return i3::apply_dbg_data(file, line, raw_data, obj_size);
	}

	__forceinline I3_EXPORT_BASE
	void	storage_pool_st_free_dbg(storage_pool_st* pool_obj, void* data)
	{
		storage_pool_st_free( pool_obj, i3::cleanup_raw_data_from_dbg_data( data ) );
	}

}


namespace i3
{

#if defined(_DEBUG) && defined(I3_POOL_DBG)

#define storage_pool_st_malloc(pool_obj, size)					storage_pool_st_malloc_dbg(pool_obj, size, __FILE__, __LINE__)
#define storage_pool_st_free(pool_obj, data)					storage_pool_st_free_dbg(pool_obj, data)

#else

#define storage_pool_st_malloc_dbg(pool_obj, size, f, l )		storage_pool_st_malloc(pool_obj, size)
#define storage_pool_st_free_dbg(pool_obj, data)				storage_pool_st_free( pool_obj, data)

#endif

}
