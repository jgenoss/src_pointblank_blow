#pragma once


#include "detail/inst_memory_mgr.h"

namespace i3
{

	template<class T> 
	T*								create_inst_memory();			// 소거는 이 어플 수준에서 끝낼까...(T를 알아야함!)
	
#define		repeat_create_inst_memory(z, n, d) \
	template<class T, pp_enum_params_1(n, class P)> \
	T*								create_inst_memory( pp_enum_params_1_cat_params(n, const P, &, p) );
	pp_repeat_from_to(1, pp_inc(inst_arity_limit), repeat_create_inst_memory, ~)
#undef		repeat_create_inst_memory

	template<class T, class P1> 
	T*								create_inst_memory(const P1& p1);
	
	template<class T>
	T*								find_inst_memory();

	template<class T> 
	void							delete_inst_memory();			// T를 아는 상태에서 소거에 들어가야함...


}

#include "inst_memory.inl"
