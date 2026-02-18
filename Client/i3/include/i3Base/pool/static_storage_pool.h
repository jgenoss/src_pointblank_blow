#pragma once

#include "storage_pool.h"		//  "dbg_util_aligned_apply.h"¸¦ Æ÷ÇÔ..
#include "../itl/mpl/mpl_identity.h"

namespace i3
{
		
	template<class T>
	void*				static_storage_pool_malloc( identity<T> );

	template<class T>
	void				static_storage_pool_free(identity<T>, void* data);
	
	template<class T>
	void*				static_storage_pool_malloc_no_dbg( identity<T> );

	template<class T>
	void				static_storage_pool_free_no_dbg(identity<T>, void* data);

	
	
	template<class T> 
	class static_storage_pool : public	shared_ginst<static_storage_pool<T> >
	{
	public:
		enum { object_alignment = static_adjust_align_for_pool<T>::value };
		
		template<class U>
		friend void*			static_storage_pool_malloc( identity<U>);
		template<class U>
		friend void				static_storage_pool_free(identity<U>, void* data);

	private:
		void*			allocate() {	return m_Pool.allocate();	}
		void			deallocate( void* data ) {	return m_Pool.deallocate(data);	}

		storage_pool<T>	m_Pool;
	};
		
	
}

#include "static_storage_pool.inl"

namespace i3
{
#if defined(_DEBUG) && defined(I3_POOL_DBG)
	
	#define static_storage_pool_malloc(type_obj)		static_storage_pool_malloc_dbg(type_obj, __FILE__, __LINE__)
	#define static_storage_pool_free(type_obj, data)	static_storage_pool_free_dbg(type_obj, data)
	
#else
		
	#define static_storage_pool_malloc_dbg( type_obj, f, l)		static_storage_pool_malloc(type_obj)
	#define static_storage_pool_free_dbg( type_obj, data)		static_storage_pool_free(type_obj, data)
	
#endif
}
