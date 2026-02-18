#pragma once

#include "v_pool.h"

#include "../i3Memory.h"
#pragma push_macro("new")
#undef new

namespace i3
{
	class v_none_pool : public v_pool 
	{
	public:
#if !defined(_DEBUG)
		virtual void*	allocate(size_t size) { return ::operator new( size );    }
		virtual void	deallocate(void* data) {  return ::operator delete(data); }
#else
		virtual void*	allocate(size_t size, const char* file, int line) 
		{ 
			void* data = ::operator new(size, _NORMAL_BLOCK, file, line); 
			NetworkDump_Malloc(data, size, file, line);				// 단순 new이지만, 덤프에 반영되지 않아 추가한다.
			return data;
		}	//_CLIENT_BLOCK ->_NORMAL_BLOCK
		virtual void	deallocate(void* data) 
		{  
			NetworkDump_Free(data, __FILE__, __LINE__);				// 단순 delete이지만, 덤프에 반영되지 않아 추가한다.
			return ::operator delete(data); 
		}
#endif

	};
}

#pragma pop_macro("new")
