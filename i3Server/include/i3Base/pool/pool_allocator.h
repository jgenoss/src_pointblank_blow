#pragma once

#include "../itl/allocator.h"
#include "static_pool.h"

#pragma push_macro("new")
#pragma push_macro("static_pool_type_malloc")
#pragma push_macro("static_pool_type_free")

#undef new

#ifndef I3_POOL_DBG_ALLOCATOR			// 얼로케이터 디버그유틸이 지정되지 않으면 매크로를 취소시키도록 조정..
#if defined(_DEBUG) && defined(I3_POOL_DBG)

#undef static_pool_type_malloc
#undef static_pool_type_free
	
#endif
#endif

#pragma warning(push)
#pragma warning(disable : 4100 )

namespace i3
{

	template<class T>
	class pool_allocator : public allocator<T>	
	{	
	public:
	
		template<class Other>
		struct rebind {	typedef pool_allocator<Other> other;	};

		pool_allocator() {}
		pool_allocator(const pool_allocator<T>&) {}
		template<class Other>	pool_allocator(const pool_allocator<Other>&) {}
		template<class Other>	pool_allocator<T>& operator=(const pool_allocator<Other>&){	return (*this);	}

		void			deallocate(pointer _Ptr, size_type) 
		{	
			i3::static_pool_type_free( identity<value_type>(), _Ptr);
		}	// deallocate object at _Ptr, ignore size

		pointer			allocate(size_type _Count)	
		{	
			return (pointer) i3::static_pool_type_malloc( identity<value_type>(), _Count * sizeof(T) );
		}	// allocate array of _Count elements

		pointer			allocate(size_type _Count, const void* ) {	return (allocate(_Count));	}	// allocate array of _Count elements, ignore hint
	};

	template<class T, class Other> inline
		bool operator==(const pool_allocator<T>&, const pool_allocator<Other>&) {	return (true); }// test for allocator equality (always true)

	template<class T, class Other> inline
		bool operator!=(const pool_allocator<T>&, const pool_allocator<Other>&) { return (false); }// test for allocator inequality (always false)
	

}

#pragma warning(pop)

#pragma pop_macro("static_pool_type_free")

#pragma pop_macro("static_pool_type_malloc")

#pragma pop_macro("new")
