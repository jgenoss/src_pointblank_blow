#if !defined(pp_is_iterating)

#ifndef bind_h
#define bind_h

#include "detail/bind_t.h"
#include "detail/bind_function_equal.h"
#include "detail/bind_add_value.h"
#include "detail/bind_list_av.h"
#include "detail/bind_operators.h"
#include "detail/bind_visit_each.h"
#include "is_bind_expression.h"
#include "placeholders.h"

#include "../preprocessor/ext/pp_enum_shifted_params_1.h"

#ifndef mem_fn_enable_cc			// cc를 원한다면 번역단위 최초에서 인클루드 전에 1로 지정합니다...
#define mem_fn_enable_cc  0
#endif 

#pragma warning(push)
#pragma warning(disable: 4512) // assignment operator could not be generated

namespace i3
{
	
	// functor
	template<class R, class F>
	bi::bind_t<R, F, bi::list0> bind(F f)
	{
		typedef bi::list0	list_type;
		return bi:bind_t<R, F, list_type>(f, list_type());
	}
	// adaptable functor
	template<class F>
	bi::bind_t< bi::unspecified, F, bi::list0> bind(F f)
	{
		typedef bi::list0	list_type;
		return bi::bind_t<bi::unspecified, F, list_type>(f, list_type());
	}
	// function ptr의 경우 모든 호출규약을 다 사용..
	template<class R>
	bi::bind_t<R, R (*)(), bi::list0> bind( R (*f)() )
	{
		typedef R (*F)();
		typedef bi::list0	list_type;
		return bi::bind_t<R,F,list_type>(f, list_type());
	}
	template<class R>
	bi::bind_t<R, R (__stdcall *)(), bi::list0> bind( R (__stdcall *f)() )
	{
		typedef R (__stdcall *F)();
		typedef bi::list0	list_type;
		return bi::bind_t<R,F,list_type>(f, list_type());
	}
	template<class R>
	bi::bind_t<R, R (__fastcall *)(), bi::list0> bind( R (__fastcall *f)() )
	{
		typedef R (__fastcall *F)();
		typedef bi::list0	list_type;
		return bi::bind_t<R,F,list_type>(f, list_type());
	}
		
	//
	// mem fun ptr의 경우 선행매크로의 조건값으로 호출규약 확대를 결정한다..(1부터 시작되며, 반복구현루틴에 넣음..)
	//
	
	namespace bi
	{
		template<class MP, bool> struct add_cref;

		template<class M, class T> struct add_cref< M T::*, false> { typedef M type; };
		template<class M, class T> struct add_cref< M T::*, true>
		{
#pragma warning(push)
#pragma warning(disable:4180)
			typedef const M&	type;
#pragma warning(pop)
		};
		
		template<class R, class T> struct add_cref< R (T::*)(), true> { typedef void type; };
		template<class R, class T> struct add_cref< R (T::*)() const, true> { typedef void type; };
		
		template<class R> struct isref : false_type {};
		template<class R> struct isref<R&> : true_type {};
		template<class R> struct isref<R*> : true_type {};
		
		template<class MP, class A1> struct mp_result { typedef typename add_cref<MP, true>::type type; };
		template<class MP, class R, class F, class L> 
		struct mp_result< MP, bind_t<R,F,L> > 
		{
			typedef typename bind_t<R,F,L>::result_type		result_type;
			typedef typename add_cref<MP, isref<result_type>::value>::type	type;
		};
	}

	template<class A1, class M, class T>
	bi::bind_t< typename bi::mp_result< M T::*, A1>::type, mfi::mp<M, T>, typename bi::list_av_1<A1>::type>
	bind( M T::*f, A1 a1)
	{
		typedef typename bi::mp_result< M T::*, A1>::type	result_type;
		typedef mfi::mp<M, T> F;
		typedef typename bi::list_av_1<A1>::type	list_type;
		return bi::bind_t<result_type, F, list_type>( F(f), list_type(a1) );
	}


}

#define		pp_iteration_params_1	(3, (1, bind_arity_limit, "bind.h"))				// 1단계 파일반복..1부터..
#include	pp_iterate()


#pragma warning(default: 4512) 
#pragma warning(pop)


#endif // bind_h

#else // pp_is_iterating

#define i_		pp_frame_iteration(1)
#define dec_i_	pp_dec(i_)

namespace i3
{
	
	template<class R, class F, pp_enum_params_1(i_, class A)>
	bi::bind_t<R, F, typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type>
	bind(F f, pp_enum_binary_params_1(i_, A, a) )
	{
		typedef typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type	list_type;
		return bi::bind_t<R, F, list_type>(f, list_type(pp_enum_params_1(i_, a)) );
	}
	
	
	template<class F, pp_enum_params_1(i_, class A) >
	bi::bind_t< bi::unspecified, F, typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type>
	bind(F f, pp_enum_binary_params_1(i_, A, a) )
	{
		typedef typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type	list_type;
		return bi::bind_t<bi::unspecified, F, list_type>(f, list_type(pp_enum_params_1(i_,a)) ); 
	}
	
	
	template<class R, pp_enum_params_1(i_, class B), pp_enum_params_1(i_, class A) >
	bi::bind_t<R, R (*)(pp_enum_params_1(i_, B)), typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type>
	bind(R (*f)(pp_enum_params_1(i_, B)), pp_enum_binary_params_1(i_, A, a) )
	{
		typedef R (*F)(pp_enum_params_1(i_, B));
		typedef typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type	list_type;
		return bi::bind_t<R, F, list_type>(f, list_type(pp_enum_params_1(i_, a)) );
	}
	
	template<class R, pp_enum_params_1(i_, class B), pp_enum_params_1(i_, class A) >
	bi::bind_t<R, R (__stdcall *)(pp_enum_params_1(i_, B)), typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type>
	bind(R (__stdcall *f)(pp_enum_params_1(i_, B)), pp_enum_binary_params_1(i_, A, a) )
	{
		typedef R (__stdcall *F)(pp_enum_params_1(i_, B));
		typedef typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type	list_type;
		return bi::bind_t<R, F, list_type>(f, list_type(pp_enum_params_1(i_, a)) );
	}

	template<class R, pp_enum_params_1(i_, class B), pp_enum_params_1(i_, class A) >
	bi::bind_t<R, R (__fastcall *)(pp_enum_params_1(i_, B)), typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type>
	bind(R (__fastcall *f)(pp_enum_params_1(i_, B)), pp_enum_binary_params_1(i_, A, a) )
	{
		typedef R (__fastcall *F)(pp_enum_params_1(i_, B));
		typedef typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type	list_type;
		return bi::bind_t<R, F, list_type>(f, list_type(pp_enum_params_1(i_, a)) );
	}
	
	template<class R, pp_enum_params_1(i_, class B), pp_enum_params_1(i_, class A) >
	bi::bind_t<R, mfi::mf< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_,B)) >, typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type> 
	bind( R (B1::*f)(pp_enum_shifted_params_1(i_, B)), pp_enum_binary_params_1(i_, A, a) )
	{
		typedef mfi::mf< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_,B)) > F;
		typedef typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type	list_type;
		return bi::bind_t<R, F, list_type>(F(f), list_type(pp_enum_params_1(i_, a)) );
	}
	
	template<class R, pp_enum_params_1(i_, class B), pp_enum_params_1(i_, class A) >
	bi::bind_t<R, mfi::cmf< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_, B)) >, typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type>
	bind( R (B1::*f)(pp_enum_shifted_params_1(i_, B)) const, pp_enum_binary_params_1(i_, A, a) )
	{
		typedef mfi::cmf< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_,B)) > F;
		typedef typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type	list_type;
		return bi::bind_t<R, F, list_type>(F(f), list_type(pp_enum_params_1(i_, a)) );		
	}

#if		mem_fn_enable_cc > 0
		
	template<class R, pp_enum_params_1(i_, class B), pp_enum_params_1(i_, class A) >
	bi::bind_t<R, mfi::mf__cdecl< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_, B)) >, typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type>
	bind( R (__cdecl B1::*f)(pp_enum_shifted_params_1(i_, B)), pp_enum_binary_params_1(i_, A, a) )
	{
		typedef mfi::mf__cdecl< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_,B)) > F;
		typedef typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type	list_type;
		return bi::bind_t<R, F, list_type>(F(f), list_type(pp_enum_params_1(i_, a)) );
	}
	
	template<class R, pp_enum_params_1(i_, class B), pp_enum_params_1(i_, class A) >
	bi::bind_t<R, mfi::cmf__cdecl< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_, B)) >, typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type>
	bind( R (__cdecl B1::*f)(pp_enum_shifted_params_1(i_, B)) const, pp_enum_binary_params_1(i_, A, a) )
	{
		typedef mfi::cmf__cdecl< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_,B)) > F;
		typedef typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type	list_type;
		return bi::bind_t<R, F, list_type>(F(f), list_type(pp_enum_params_1(i_, a)) );		
	}
	
	template<class R, pp_enum_params_1(i_, class B), pp_enum_params_1(i_, class A) >
	bi::bind_t<R, mfi::mf__stdcall< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_, B)) >, typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type>
	bind( R (__stdcall B1::*f)(pp_enum_shifted_params_1(i_, B)), pp_enum_binary_params_1(i_, A, a) )
	{
		typedef mfi::mf__stdcall< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_,B)) > F;
		typedef typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type	list_type;
		return bi::bind_t<R, F, list_type>(F(f), list_type(pp_enum_params_1(i_, a)) );
	}
	
	template<class R, pp_enum_params_1(i_, class B), pp_enum_params_1(i_, class A) >
	bi::bind_t<R, mfi::cmf__stdcall< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_, B)) >, typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type>
	bind( R (__stdcall B1::*f)(pp_enum_shifted_params_1(i_, B)) const, pp_enum_binary_params_1(i_, A, a) )
	{
		typedef mfi::cmf__stdcall< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_,B)) > F;
		typedef typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type	list_type;
		return bi::bind_t<R, F, list_type>(F(f), list_type(pp_enum_params_1(i_, a)) );		
	}

	template<class R, pp_enum_params_1(i_, class B), pp_enum_params_1(i_, class A) >
	bi::bind_t<R, mfi::mf__fastcall< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_, B)) >, typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type>
	bind( R (__fastcall B1::*f)(pp_enum_shifted_params_1(i_, B)), pp_enum_binary_params_1(i_, A, a) )
	{
		typedef mfi::mf__fastcall< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_,B)) > F;
		typedef typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type	list_type;
		return bi::bind_t<R, F, list_type>(F(f), list_type(pp_enum_params_1(i_, a)) );
	}
	
	template<class R, pp_enum_params_1(i_, class B), pp_enum_params_1(i_, class A) >
	bi::bind_t<R, mfi::cmf__fastcall< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_, B)) >, typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type>
	bind( R (__fastcall B1::*f)(pp_enum_shifted_params_1(i_, B)) const, pp_enum_binary_params_1(i_, A, a) )
	{
		typedef mfi::cmf__fastcall< R (B1* pp_comma_if(dec_i_) pp_enum_shifted_params_1(i_,B)) > F;
		typedef typename pp_cat(bi::list_av_, i_)< pp_enum_params_1(i_,A)>::type	list_type;
		return bi::bind_t<R, F, list_type>(F(f), list_type(pp_enum_params_1(i_, a)) );		
	}

#endif // mem_fn_enable_cc > 0
	
}

#undef  i_

#endif	// pp_is_iterating	
