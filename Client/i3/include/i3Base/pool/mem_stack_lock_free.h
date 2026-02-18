#pragma once

#include "cas.h"

#include "free_object.h"

namespace i3
{
		
	__declspec( align(64) )			// false sharing을 막기 위한 사이즈확장..
	class  mem_stack_lock_free
	{
	public:
		mem_stack_lock_free() : m_head(nullptr), m_pop_count(0) {}
		void		push(void* data);
		void*		pop();
		bool		empty() const;
	private:
		free_object*	volatile	m_head;
		volatile		size_t		m_pop_count;			// 멤버변수 순서 고정.
		size_t						m_align_dummy[ (64 - ( sizeof(void*) + sizeof(size_t) )) / sizeof(size_t)];
	};
	
}

#include "mem_stack_lock_free.inl"