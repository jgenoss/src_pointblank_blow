#pragma once

namespace i3
{
	//
	// 32(or 64)바이트 정렬까지만 보장시키고, 64바이트 정렬부터는 dbg를 사용하지 않는 쪽으로 처리하는쪽으로 변경..
	// 
	
	struct dbg_block
	{
		dbg_block*		next;					// 4	8
		dbg_block*		prev;					// 4	8
		const char*		szFilename;				// 4	8
		DWORD			line_flag;				// 4	4

		DWORD			data_size;				// 4	4
		DWORD			request_number;			// 4	4
		DWORD			reserved;				// 4    4
#ifdef _WIN64
		DWORD			no_op[5];				// 0    20
#endif
		DWORD			gap;					// 4	4
	};
	
	// 32,44 
	struct aligned_dummy
	{
		void*		head;
		uintptr_t	gap;
	};
		
}
