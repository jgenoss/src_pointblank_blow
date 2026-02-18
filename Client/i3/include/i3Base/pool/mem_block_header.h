#pragma once

#include "free_object.h"

namespace i3
{
	
	const size_t	pool_block_size = 16 * 1024;				// 16k로 확정

	const size_t	pool_block_header_alignment = 64;				// 헤더는 64바이트 정렬(캐시라인)을 유지(중요)

	struct mem_block_header;
		
	struct mem_block_header_local
	{
		mem_block_header*		next;					// 링크 리스트...(프리블록리스트캐싱과 액티브리스트 2개용도에 사용)
		mem_block_header*		prev;					// 링크 리스트...(액티브리스트 용도로만 사용..)
		void*					curr_alloc_ptr;			// 충분이 할당된 시작점부터 할당마다 위치를 옮기는 포인터..
		free_object*			freeList;				// 프리리스트..(프라이빗쓰레드에 한해...로컬로 유지하는게 더 좋을것 같다..)

		DWORD					threadID;				//	
		UINT					alloc_count;			//
		UINT					obj_actual_size;		// ..실제 사이즈...
		WORD					size_index;				// 사이즈 인덱스..(넣는게 더 나을것 같다..free가속에 도움됨..)
		WORD					is_full;				// 꽉 찬 경우...
		void*					cp_commit_h;			// 컴팩트풀에서만 쓴다..
	}; // 36, 56 //

	struct mem_block_header	: public mem_block_header_local								
	{
		size_t					padding_front[ ( pool_block_header_alignment - sizeof(mem_block_header_local)) / sizeof(size_t)];	// 캐시라인분리
		free_object*			freeList_tss;			// 타쓰레드 프리리스트..
		mem_block_header*		tss_free_block_next;	// 다음번 tss프리블록....
		size_t					padding_back[ (pool_block_header_alignment - 2*sizeof(void*)) / sizeof(size_t)];						// 캐시라인분리
	};		// bit PC에 관계없이.....128바이트...
	
	
}