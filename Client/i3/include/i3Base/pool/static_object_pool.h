#pragma once

#include "object_pool.h"			// "dbg_util_aligned_apply.h"를 포함..
#include "../itl/mpl/mpl_identity.h"
#include "../inst/ginst.h"

namespace i3
{
		

	template<class T>
	void*				static_object_pool_malloc( identity<T> );

	template<class T>
	void				static_object_pool_free(identity<T>, void* data);

	template<class T>
	void*				static_object_pool_malloc_no_dbg( identity<T> );

	template<class T>
	void				static_object_pool_free_no_dbg(identity<T>, void* data);
	
	template<class T>
	class static_object_pool;

	template<class T> struct use_delayed_atexit_ginst< static_object_pool<T> > : true_type {};
	
	template<class T> 
	class static_object_pool : public	shared_ginst<static_object_pool<T> >
	{
	public:
		enum { object_alignment = i3::object_pool<T>::object_alignment };

		template<class U>
		friend void*			static_object_pool_malloc( identity<U> );
		template<class U>
		friend void				static_object_pool_free(identity<U>, void* data);

	private:
		void*			allocate();
		void			deallocate( void* data );		
		object_pool<T>	m_Pool;
	};
	
	
		
}

#include "static_object_pool.inl"


// 매크로쪽은 인라인에서 우선 제외했다...

namespace i3
{
#if defined(_DEBUG) && defined(I3_POOL_DBG)	
	
	#define static_object_pool_malloc(type_obj)					static_object_pool_malloc_dbg(type_obj, __FILE__, __LINE__)
	#define static_object_pool_free(type_obj, data)				static_object_pool_free_dbg(type_obj, data)
	
#else
	
	#define static_object_pool_malloc_dbg( type_obj, f, l)		static_object_pool_malloc(type_obj)
	#define static_object_pool_free_dbg( type_obj, data)		static_object_pool_free(type_obj, data)
	
#endif
}
