#pragma once

#include "pool_dbg_apply.h"
#include "pool_base.h"

#include "static_calculate_block_length_with_elem_length.h"

#include "local_large_object_cache.h"

#include "static_adjust_align_for_pool.h"
#include "static_adjust_size_for_pool.h"


#define I3_RESERVE_OBJECT_POOL(TYPE, RESERVE)	\
	namespace i3 { template<> struct reserve_object_pool<TYPE> { enum { value = (RESERVE > 0) ? RESERVE : 1 }; }; }



namespace i3
{
	template<class T> struct reserve_object_pool  {  enum { value = 64 };  };
	

	template<class>			class object_pool_small;
	template<class>			class object_pool_large;
	template<class>			class object_pool;
	

	template<class T>
	void*						object_pool_malloc(object_pool<T>* pool_obj);

	template<class T>
	void						object_pool_free(object_pool<T>* pool_obj, void* data);
	
	template<class T>
	void*						object_pool_malloc_no_dbg(object_pool<T>* pool_obj);
	template<class T>
	void						object_pool_free_no_dbg(object_pool<T>* pool_obj, void* data);

	template <class T>
	class object_pool 
	{
	public:
		enum { 	object_size      = static_adjust_size_for_pool<T>::value };
		enum {	object_alignment = static_adjust_align_for_pool<T>::value };
		enum {	is_large_object =  ( object_size > maximum_object_size ) };
		
	
		template<class U>
		friend void*					object_pool_malloc(object_pool<U>* pool_obj);
		template<class U>
		friend void						object_pool_free(object_pool<U>* pool_obj, void* data);

	private:
		void*			allocate();
		void			deallocate( void* data ); 

		typedef typename mpl::if_c< is_large_object, object_pool_large<T>, i3::tls_ptr< object_pool_small<T> > >::type	pool_type;

		pool_type		m_pool;
		
		template<class> friend class static_object_pool; 
	};
	
			
}

#include "object_pool.inl"

// 매크로쪽은 인라인에서 우선 제외했다...

namespace i3
{
#if defined(_DEBUG) && defined(I3_POOL_DBG)	
	
	#define object_pool_malloc(pool_obj)						object_pool_malloc_dbg(pool_obj, __FILE__, __LINE__)
	#define object_pool_free(pool_obj, data)					object_pool_free_dbg(pool_obj, data)

#else
	
	#define object_pool_malloc_dbg( pool_obj, f, l)				object_pool_malloc(pool_obj)
	#define object_pool_free_dbg( pool_obj, data )				object_pool_free( pool_obj, data)
	
#endif
}
