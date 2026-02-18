#pragma once

#include "pool_dbg_apply.h"
#include "pool_by_thread.h"

#include "../itl/mpl/mpl_identity.h"
#include "../itl/type_traits/alignment_of.h"

#include "../inst/ginst.h"
#include "../inst/ginst_global_init.h"

namespace i3
{

	void*			static_pool_malloc( size_t num );
	void			static_pool_free( void* pData );	
	
	void*			static_pool_malloc_no_dbg( size_t num);
	void			static_pool_free_no_dbg( void* pData );	
	
	template<class T>
	void*			static_pool_type_malloc(identity<T>, std::size_t size);

	template<class T>
	void			static_pool_type_free(identity<T>, void* pData);


	class static_pool : public shared_ginst<static_pool>
	{
	public:
		friend void*	static_pool_malloc( size_t num );
		friend void		static_pool_free( void* pData );	
	private:
		__forceinline	void*	allocate( size_t num ); 
		__forceinline	void	deallocate( void* data );			
		i3::tls_ptr<pool_by_thread>	m_Pool;
	};
		
	template<> struct use_delayed_atexit_ginst< static_pool   > : true_type {};
	
	ginst_global_init(static_pool);
}

#include "static_pool.inl"

// 매크로쪽은 인라인에서 우선 제외했다...

namespace i3
{
#if defined(_DEBUG) && defined(I3_POOL_DBG)	
	
	#define static_pool_malloc(num)					static_pool_malloc_dbg( num, __FILE__, __LINE__)
	#define static_pool_type_malloc(type, size)		static_pool_type_malloc_dbg(type, size, __FILE__, __LINE__)

	#define static_pool_free(data)						static_pool_free_dbg(data)
	#define static_pool_type_free(type, data)			static_pool_type_free_dbg(type, data)
#else
	
	#define static_pool_malloc_dbg( num, f, l)				static_pool_malloc(num)
	#define static_pool_type_malloc_dbg(type, size, f, l)	static_pool_malloc(size)

	#define static_pool_free_dbg(data)						static_pool_free(data)	
	#define static_pool_type_free_dbg(type, data)			static_pool_free(data)
#endif
	
}
