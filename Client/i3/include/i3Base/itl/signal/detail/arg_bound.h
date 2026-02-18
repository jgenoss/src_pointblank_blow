#if !defined(pp_is_iterating)

#ifndef arg_bound_h
#define arg_bound_h

//
// call_bound와 비슷하지만 리턴타입이 없고, 함수형으로 표현되지 않음..
// 리턴타입은 함수호출시 result_type을 구해서 쓴다..
//

namespace i3
{
	template< pp_enum_params_with_a_default(ft_arity_limit, class T, null_type) >		
	class arg_bound;
}

#define pp_iteration_params_1 (3, (0, ft_arity_limit, "arg_bound.h") )		// 0부터 반복임..
#include pp_iterate()


#endif // arg_bound_h

#else  // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{
	template<pp_enum_params(i_, class T)>
	class arg_bound< pp_enum_params_spec(ft_arity_limit, i_, T, null_type) > 
	{
	public:
		pp_repeat_binary_params_with_a_cat(i_, typedef typename make_parameter<T, >::type P, ;)
		typedef i3::tuple< pp_enum_params(i_, P) >							tuple_type;
		
		arg_bound( pp_enum_binary_params(i_, P, p) ) : m_args( pp_enum_params(i_, p) ) {}

		template<class Fun> __forceinline
		typename i3::result_of<Fun>::type	operator()( const Fun& fun) const
		{
			// m_args.get<0>() , m_args.get<1>(), ...
			typedef typename i3::result_of<Fun>::type		result_type;
			return fun( pp_enum_params_with_a_cat(i_, m_args.get<, >() ) );
		}
		
		template<class Fun, class ResultType> __forceinline
		ResultType							operator()( const Fun& fun, i3::identity<ResultType> ) const
		{
			return 	fun( pp_enum_params_with_a_cat(i_, m_args.get<, >() ) );		
		}
	private:
		tuple_type															m_args;		
	};

}

#undef  i_

#endif // pp_is_iterating
