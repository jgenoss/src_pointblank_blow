#pragma once

#include "v_pool.h"

namespace i3
{
	//
	// 추상클래스의 경우 이쪽으로 처리되고..이쪽이 실행되지 않도록 유의할것.....
	//

	class v_invalid_pool : public v_pool 
	{
	public:
#if !defined(_DEBUG)
		virtual void*	allocate(size_t size) { return nullptr;    }
#else
		virtual void*	allocate(size_t size, const char* file, int line) { return nullptr; }
#endif
		virtual void	deallocate(void* data) {}
	};
		
		
}
