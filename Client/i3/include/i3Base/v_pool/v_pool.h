#pragma once

#include "../smart_ptr/make_shared.h"

namespace i3
{

	class I3_EXPORT_BASE v_pool
	{
	public:
		const char*		type_name() const { return typeid(*this).name(); }

#if !defined(_DEBUG)
		virtual void*	allocate(size_t size) = 0;
#else
		virtual void*	allocate(size_t size, const char* file, int line) = 0;
#endif
		virtual void	deallocate(void* data) = 0;
		virtual ~v_pool() {}
	};

	typedef	i3::shared_ptr<v_pool>		v_pool_ptr;
	
	template<class v_pool_derived>
	v_pool_ptr	create_v_pool_ptr() { return i3::make_shared<v_pool_derived>();	}		
	template<class v_pool_derived, class A1>
	v_pool_ptr	create_v_pool_ptr(A1 a1) { return i3::make_shared<v_pool_derived>(a1); }
}
