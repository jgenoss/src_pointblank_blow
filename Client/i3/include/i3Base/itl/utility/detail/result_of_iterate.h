
#define i_		pp_iteration()

namespace i3
{

#define pp_result_of_args		pp_enum_params(i_, T)
	
	template<class F pp_enum_trailing_params(i_, class T)>
	struct tr1_result_of< F (pp_result_of_args) > 
		: mpl::if_< 
					mpl::or_< is_pointer<F>, is_member_function_pointer<F> >,	// (멤버)함수포인터
					detail::tr1_result_of_impl<
							typename remove_cv<F>::type,						
							typename remove_cv<F>::type (pp_result_of_args),
							has_result_type<F>::value>,
					detail::tr1_result_of_impl<
							F, F (pp_result_of_args), has_result_type<F>::value> >::type {};

	template<class F pp_enum_trailing_params(i_, class T)>
	struct result_of< F (pp_result_of_args) > : tr1_result_of< F (pp_result_of_args) > {};

#undef pp_result_of_args


#if  i_ >= 1

	namespace detail
	{
		template<class R, class FArgs pp_enum_trailing_params(i_, class T)>
		struct tr1_result_of_impl< R (*)( pp_enum_params(i_, T) ), FArgs, false> { typedef R type; };

		template<class R, class FArgs pp_enum_trailing_params(i_, class T)>
		struct tr1_result_of_impl< R (&)( pp_enum_params(i_, T) ), FArgs, false> { typedef R type; };

		template<class R, class FArgs pp_enum_trailing_params(i_, class T)>
		struct tr1_result_of_impl< R (T0::*)( pp_enum_shifted_params(i_, T) ), FArgs, false> { typedef R type; };

		template<class R, class FArgs pp_enum_trailing_params(i_, class T)>
		struct tr1_result_of_impl< R (T0::*)( pp_enum_shifted_params(i_, T) ) const, FArgs, false> { typedef R type; };

		template<class R, class FArgs pp_enum_trailing_params(i_, class T)>
		struct tr1_result_of_impl< R (T0::*)( pp_enum_shifted_params(i_, T) ) volatile, FArgs, false> { typedef R type; };

		template<class R, class FArgs pp_enum_trailing_params(i_, class T)>
		struct tr1_result_of_impl< R (T0::*)( pp_enum_shifted_params(i_, T) ) const volatile, FArgs, false> { typedef R type; };

	}

#endif

}

#undef i_