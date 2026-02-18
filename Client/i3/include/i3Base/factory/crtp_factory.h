#pragma once

#include "factory.h"
#include "detail/crtp_factory_impl.h"

#define crtp_factory_arity_limit 5

#define crtp_factory_global_init(type)	static i3::crtp_factory_initializer<type>	pp_cat(pp_cat(_crtp_factory_initializer_, type), _; )	

namespace i3
{
// 
//  crtp_root 내부에만 스태틱한 멤버를 갖는다....
//
	template<class FunctionType, class B = i3::null_type>
	class crtp_factory_root : public B 
	{
	public:
		typedef i3::factory<i3::fixed_string, FunctionType>			factory_type;
		typedef typename factory_type::result_class_type			T;			// 실제 클래스타입..

		crtp_factory_root() {}
#define repeat_crtp_factory_cons(z, n, d) \
		template<pp_enum_params_1(n, class A)> \
		crtp_factory_root( pp_enum_binary_params_1(n, A, a) ) : B(pp_enum_params_1(n, a)) {}
		pp_repeat_from_to(1, pp_inc(crtp_factory_arity_limit), repeat_crtp_factory_cons, ~)
#undef	repeat_crtp_factory_cons
	
		static void				create_explicit() {	 _create_factory();	_register_factory();	}		
		static factory_type*	get_factory() {  return ms_factory; }

	private:	
		factory_type*				_create_factory() { return i3::create_global_with_atexit<factory_type, ms_factory>(); }
		register_crtp_factory<T>*	_register_factory() { return i3::create_global_with_atexit<register_crtp_factory<T, factory_type>, ms_reg_factory>( get_factory() ); }

		static factory_type*		ms_factory;
		static register_crtp_factory<T>*	ms_reg_factory;
	};
	
	template<class F, class B>
	typename crtp_factory_root<F,B>::factory_type*	crtp_factory_root<F,B>::ms_factory = crtp_factory_root<F,B>::_create_factory();
	template<class F, class B>
	register_crtp_factory<typename crtp_factory_root<F,B>::T>*	crtp_factory_root<F,B>::ms_reg_factory = crtp_factory_root<F,B>::_register_factory();
	
	template<class T, class B = i3::null_type>
	class crtp_factory : public B 
	{
	public:
		typedef typename B::factory_type			factory_type;

		crtp_factory() {}
#define repeat_crtp_factory_cons(z, n, d) \
		template<pp_enum_params_1(n, class A)> \
		crtp_factory( pp_enum_binary_params_1(n, A, a) ) : B(pp_enum_params_1(n, a)) {}
		pp_repeat_from_to(1, pp_inc(crtp_factory_arity_limit), repeat_crtp_factory_cons, ~)
#undef	repeat_crtp_factory_cons
		
		static void		create_explicit() {	 _register_factory();	}
	private:
		register_crtp_factory<T>*	_register_factory() { return i3::create_global_with_atexit<register_crtp_factory<T, factory_type>, ms_reg_factory>( get_factory() ); }
		static register_crtp_factory<T>*	ms_reg_factory;
	};
	
	template<class T, class B>
	register_crtp_factory<T>*	crtp_factory<T,B>::ms_reg_factory = crtp_factory<T,B>::_register_factory();
	
}
