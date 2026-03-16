#pragma once

#include "pool_dbg_apply.h"
#include "pool_by_thread.h"

#include "../itl/mpl/mpl_identity.h"
#include "../itl/type_traits/alignment_of.h"

#include "../inst/ginst.h"

namespace i3
{

	void*			static_pool_malloc( size_t num, size_t align );
	void			static_pool_free( void* pData );	
	
	void*			static_pool_malloc_no_dbg( size_t num, size_t align);
	void			static_pool_free_no_dbg( void* pData );	
	
	template<class T>
	void*			static_pool_type_malloc(identity<T>, std::size_t size);

	template<class T>
	void			static_pool_type_free(identity<T>, void* pData);


	class static_pool : public shared_ginst<static_pool>
	{
	public:
		friend void*	static_pool_malloc( size_t num, size_t align );
		friend void		static_pool_free( void* pData );	
		
		template<class T> friend
		void*			static_pool_type_malloc(identity<T>, std::size_t size);

		template<class T> friend
		void			static_pool_type_free(identity<T>, void* pData);

	private:
		__forceinline	void*	allocate( size_t num, size_t align ); 
		__forceinline	void	deallocate( void* data );			
		i3::tls_ptr<pool_by_thread>	m_Pool;
	};
		
	template<> struct use_delayed_atexit_ginst< static_pool   > : true_type {};

}

#include "static_pool.inl"

// 매크로쪽은 인라인에서 우선 제외했다...

namespace i3
{
#if defined(_DEBUG) && defined(I3_POOL_DBG)	
	
	#define static_pool_malloc(num, align)				static_pool_malloc_dbg( num, align, __FILE__, __LINE__, 0)
	#define static_pool_type_malloc(type, size)			static_pool_type_malloc_dbg(type, size, __FILE__, __LINE__)
	#define static_pool_malloc_name(num, align, name)	static_pool_malloc_dbg( num, align, __FILE__, __LINE__, name)

	#define static_pool_free(data)						static_pool_free_dbg(data)
	#define static_pool_type_free(type, data)			static_pool_type_free_dbg(type, data)
#else
	
	#define static_pool_malloc_name( num, align, name)		static_pool_malloc( num, align)
	#define static_pool_malloc_dbg( num, align, f, l, name) static_pool_malloc(num, align)
	#define static_pool_type_malloc_dbg(type, size, f, l)	static_pool_type_malloc(type, size)

	#define static_pool_free_dbg(data)						static_pool_free(data)	
	#define static_pool_type_free_dbg(type, data)			static_pool_type_free(type, data)
#endif
	
}
