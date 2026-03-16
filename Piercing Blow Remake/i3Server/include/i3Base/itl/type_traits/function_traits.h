
#if !defined(pp_is_iterating)

#ifndef function_traits_h
#define function_traits_h

//
// type_traits의 표준으로 이 구현이 들어있음...pp를 활용해서 재정리함..
// 결론적으로 그다지 유용하지 않다...(특질정보도 부족하고, 멤버함수포인터가 되지 않는다..)
//

#include "is_function.h"
#include "add_pointer.h"

namespace i3
{
	namespace detail
	{
		template<class Function> struct function_traits_helper;
		
		template<class R>
		struct function_traits_helper< R (*)(void) >
		{
			static const unsigned	arity = 0;
			typedef R				result_type;
		};
		
		template<class R, class T1>
		struct function_traits_helper< R (*)(T1) >
		{
			static const unsigned	arity = 1;
			typedef R				result_type;
			typedef T1 arg1_type;
			typedef T1 argument_type;
		};
		
		template<class R, class T1, class T2>
		struct function_traits_helper< R (*)(T1, T2) >
		{
			static const unsigned	arity = 2;
			typedef R				result_type;
			typedef T1 arg1_type;
			typedef T2 arg2_type;
			typedef T1 first_argument_type;
			typedef T2 second_argument_type;
		};
	}
}

#include "../preprocessor/pp_iterate.h"
#define pp_iteration_params_1 (3, (3, 25, "function_traits.h") )
#include pp_iterate()

namespace i3
{
	template<class Function>
	struct function_traits : detail::function_traits_helper< add_pointer<Function>::type > {};
}


#endif //function_traits_h

#else // pp_is_iterating


#define i_		pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		template<class R, pp_enum_params_1(i_, class T)>
		struct function_traits_helper< R (*)( pp_enum_params_1(i_, T) ) >
		{
			static const unsigned	arity = i_;
			typedef	R				result_type;
			
			#define detail_typedef(unused, n_, unused2)	typedef pp_cat(T, pp_inc(n_)) pp_cat(pp_cat(arg, pp_inc(n_)),_type);
			pp_repeat(i_, detail_typedef, unused)
			#undef detail_typedef
		};
	}
}

#undef i_


#endif // pp_is_iterating
