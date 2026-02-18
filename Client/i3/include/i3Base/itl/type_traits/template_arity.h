#if !defined(pp_is_iterating)

#ifndef template_arity_h
#define template_arity_h

// 언젠가 이런 기능을 찾을수 있을것이라고 생각했음..mpl에서 참고
// 문제가 되는 것은 반복의 횟수임...(가장 높은수부터 반복되며, 우선 40으로 제한..)
// 원본소스의 거듭된 if사용 대신 모두 오버로딩함수목록으로 빼놓음...(이렇게 수정한다고 결과가 잘못될 이유는 없다..)
//

#define	template_arity_limit 40


#include "../mpl/mpl_int.h"
#include "../preprocessor/pp_iterate.h"
#include "../preprocessor/pp_enum_params.h"
#include "../preprocessor/ext/pp_enum_datas.h"
#include "../mpl/mpl_identity.h"

namespace i3
{
	namespace detail
	{
		template<int N> struct template_arity_tag { typedef char(&type)[N+1]; };
		template_arity_tag<0>::type template_arity_helper(...);
	}
}

#define pp_iteration_params_1 (3, (1, template_arity_limit, "template_arity.h") )		// 1개부터..
#include pp_iterate()

namespace i3
{
	namespace detail
	{
		// identity매크로가 기본클래스로의 형변환을 막고 있다는 점을 주목할 것...
		template<class F>
		struct template_arity_impl
		{
			static const int value = sizeof(template_arity_helper( (const volatile identity<F>*) 0 )) -1;
			typedef mpl::int_<value>	type;
		};
	}
	
	template<class F>
	struct template_arity : detail::template_arity_impl<F> {};
	
}

#endif //template_arity_h

#else  // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		template<	template< pp_enum_datas(i_, class) > class F, pp_enum_params(i_, class T) >
		typename template_arity_tag<i_>::type template_arity_helper(const volatile identity< F< pp_enum_params(i_,T) > >* ); //, template_arity_tag<i_> );
	}
}

#undef  i_

#endif // pp_is_iterating