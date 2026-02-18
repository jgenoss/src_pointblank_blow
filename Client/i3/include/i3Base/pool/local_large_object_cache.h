#pragma once

#include "large_object_cache_dispatch.h"


namespace i3
{

	//
	// 로컬 캐시는 싱글턴으로 두기 어렵다..(특화된 object 풀의 디스패치 한쪽에 심어놔야 할 것이다...)
	//
	
	template<class T>
	class local_large_object_cache : public detail::select_large_object_cache_dispatch<T>::type {};
	//	
	
}


#include "local_large_object_cache.inl"