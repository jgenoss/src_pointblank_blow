#pragma once

namespace i3
{
	struct dbg_block
	{
		dbg_block*		next;					// 4	8
		dbg_block*		prev;					// 4	8
		const char*		szFilename;				// 4	8
		DWORD			line_flag;				// 4	4

		DWORD			data_size;				// 4	4
		const char*		szDataType;				// 4	8	// 가능한 경우에만...(개별 클래스 new버전에는 가능하겠네...)
		DWORD			request_number;			// 4	4
		DWORD			gap;					// 4	4
	};

	struct aligned_dummy
	{
		void*		head;
		uintptr_t	gap;
	};
		
}
