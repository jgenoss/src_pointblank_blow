#pragma once

#include "pool_dbg_user_config.h"


namespace i3
{
	// 16바이트정렬에 강제맞춤.... ( 32 / 48바이트)
#if defined(_DEBUG) && defined(I3_POOL_DBG)

	void		init_dbg_util(); 
	void		dump_leak();
	DWORD		set_break_alloc(DWORD alloc_number);


#else
	__forceinline 
	void		init_dbg_util() {}
	__forceinline		
	void		dump_leak() {}
	__forceinline
	void		set_break_alloc(DWORD){}

#endif
}

