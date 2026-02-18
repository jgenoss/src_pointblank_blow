#if !defined(pp_is_iterating)

#ifndef call_bound_h
#define call_bound_h


// 인수보관을 해야하고, 여기서는 레퍼런스 타입 튜플을 쓴다...
// 인수를 사전 보관한 뒤에는 함수호출시 더 이상의 인수를 요구하지 않는다..
// 응용 여지가 있을것 같아서 detail네임스페이스는 일단 쓰지 않는다.

namespace i3
{
	template<class FunSig>		// 함수형 --> 순수 구현은 없고, 모두 특화시켜야함..
	class call_bound;
}

#define pp_iteration_params_1 (3, (0, ft_arity_limit, "call_bound.h") )		// 0부터 반복임..
#include pp_iterate()


#endif // call_bound_h

#else  // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{
	template<class R pp_enum_trailing_params(i_, class T)>
	class call_bound< R (pp_enum_params(i_, T)) > 
	{
	public:
		typedef R															result_type;
		pp_repeat_binary_params_with_a_cat(i_, typedef typename make_parameter<T, >::type P, ;)
		typedef i3::tuple< pp_enum_params(i_, P) >							tuple_type;
		
		call_bound( pp_enum_binary_params(i_, P, p) ) : m_args( pp_enum_params(i_, p) ) {}

		template<class Fun> __forceinline
		result_type		operator()( const Fun& fun) const
		{
			// m_args.get<0>() , m_args.get<1>(), ...
			return fun( pp_enum_params_with_a_cat(i_, m_args.get<, >() ) );
		}
	private:
		tuple_type										m_args;		
	};

}

#undef  i_

#endif // pp_is_iterating