#pragma once

#include "static_msb.h"
#include "../memory/memory_alloc.h"

namespace i3
{
	// 기존 풀 블록사이즈 = 16K // 큰오브젝트(8192단위/ 최대1메가) 128개 정도의 버켓배열
	
	const size_t num_large_object_bucket  = 128;		// 1메가까지..
	const size_t bucket_step_large_object = 8192;
	const size_t bucket_step_large_object_shift = i3::static_msb32<bucket_step_large_object>::value;
	const size_t align_large_object = 1024*16;			//128;	// 이제 128대신 16K로 크게 잡고, 풀에서 큰사이즈검사를 간략화시킨다..

	// align인자를 제거할것.....128바이트 정렬..최소사이즈가 8192(8k), 스탭단위도 8192 private heap사용
	struct large_object_header
	{
		void*		raw_data;				// os에 반납할 원점을 가르킴..(128바이트정렬이 되지 않을수있음)
		size_t		size_index;				// 버킷에 들어가는 사이즈..
	};	// 8바이트 혹은 16바이트..
	
	class  mem_stack_lock_free;

	class large_object_cache_base
	{
	protected:
		large_object_cache_base();
		~large_object_cache_base();
		void*					private_malloc(size_t size);
		void					private_free(void* data);
		void*					allocate_from_malloc(size_t alloc_size, size_t size_index);
		void					cleanup_stack(mem_stack_lock_free& mem_stack);
	private:
		heap_handle				m_private_heap;
	};
		
	
	
}

#include "large_object_cache_base.inl"
