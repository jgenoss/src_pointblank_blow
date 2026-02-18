#if !defined(pp_is_iterating)

#ifndef create_global_with_atexit_h
#define create_global_with_atexit_h

#include "../../inst/inst_memory.h"

#define create_global_with_atexit_arity_limit		5

namespace i3
{
	// 굳이 인라인일 필요는 없음..

	template<class T, T*& var_name>
	T*			create_global_with_atexit();

	template<class T, T*& var_name, bool delay_exit>
	T*			create_global_with_atexit();	
	
	template<class T, class D, T*& var_name>
	T*			create_global_with_atexit();	

	template<class T, class D, T*& var_name, bool delay_exit>
	T*			create_global_with_atexit();	

	namespace detail
	{
		template<class T, T*& var_name>
		void	delete_global_for_atexit()
		{
			delete var_name;	var_name = nullptr;
		}
		template<bool delay_exit>
		struct  atexit_switch 
		{
			template<class T, T*& var_name> static void call() {  i3::delayed_atexit(delete_global_for_atexit<T, var_name>); }
		};
		template<>
		struct atexit_switch<false>
		{
			template<class T, T*& var_name> static void call() { ::atexit( delete_global_for_atexit<T, var_name> ); }
		};
	}
	
	template<class T, T*& var_name>
	T*			create_global_with_atexit()
	{
		return create_global_with_atexit<T, T, var_name, false>();
	}

	template<class T, T*& var_name, bool delay_exit>
	T*			create_global_with_atexit()
	{
		return create_global_with_atexit<T, T, var_name, delay_exit>();
	}

	template<class T, class D, T*& var_name>
	T*			create_global_with_atexit()
	{
		return create_global_with_atexit<T, D, var_name, false>();
	}

	template<class T, class D, T*& var_name, bool delay_exit>
	T*			create_global_with_atexit()
	{
		if (var_name) return var_name;
		T* new_var = new D;
		detail::atexit_switch<delay_exit>::call<T, var_name>();
		return var_name = new_var;
	}

}

#define		pp_iteration_params_1	(3, (1, create_global_with_atexit_arity_limit, "create_global_with_atexit.h"))				// 1단계 파일반복..
#include	pp_iterate()

#endif
#else		// pp_is_iterating

#define i_				pp_frame_iteration(1)

namespace i3
{
	template<class T, T*& var_name, pp_enum_params_1(i_, class P)>
	T*			create_global_with_atexit(pp_enum_params_1_cat_params(i_, const P,&, p))
	{
		return create_global_with_atexit<T, T, var_name, false>(pp_enum_params_1(i_, p));
	}

	template<class T, T*& var_name, bool delay_exit, pp_enum_params_1(i_, class P)>
	T*			create_global_with_atexit(pp_enum_params_1_cat_params(i_, const P,&, p))
	{
		return create_global_with_atexit<T, T, var_name, delay_exit>(pp_enum_params_1(i_, p));
	}

	template<class T, class D, T*& var_name, pp_enum_params_1(i_, class P)>
	T*			create_global_with_atexit(pp_enum_params_1_cat_params(i_, const P,&, p))
	{
		return create_global_with_atexit<T, D, var_name, false>(pp_enum_params_1(i_, p));
	}

	template<class T, class D, T*& var_name, bool delay_exit, pp_enum_params_1(i_, class P)>
	T*			create_global_with_atexit(pp_enum_params_1_cat_params(i_, const P,&, p))
	{
		if (var_name) return var_name;
		T* new_var = new D(pp_enum_params_1(i_, p) );;
		detail::atexit_switch<delay_exit>::call<T, var_name>();
		return var_name = new_var;
	}


}

#undef i_

#endif
