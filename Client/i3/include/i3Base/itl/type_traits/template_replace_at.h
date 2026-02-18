#if !defined(pp_is_iterating)

#ifndef template_replace_at_h
#define template_replace_at_h

#include "../preprocessor/comparison/pp_not_equal.h"
#include "../preprocessor/ext/pp_enum_params_from_to.h"

#define template_replace_at_limit		20

namespace i3
{
	// 템플릿 클래스가 아닌 경우나 인덱스가 맞지 않으면 처리없이 리턴..
	template<class T, long N, class Elem>
	struct template_replace_at_c {	typedef T	type;	};

	template<class T, class N, class Elem>
	struct template_replace_at : template_replace_at_c<T, long(N::value), Elem> {};

}

#define pp_iteration_params_1 (3, (1, template_replace_at_limit, "template_replace_at.h") )		// 1개부터..
#include pp_iterate()

#endif // template_replace_at_h

#else // pp_is_iterating

#if				pp_iteration_depth() == 1

#define i_		pp_frame_iteration(1)


#define pp_iteration_params_2 (3, (0, pp_dec(i_), "template_replace_at.h") )		// 0번 부터..
#include pp_iterate()

#undef  i_


#elif		pp_iteration_depth() == 2

#define j_		pp_frame_iteration(2)

#define j_inc   pp_inc(j_)
// i_ 는 1부터 , j_는 0부터..2중루프


namespace i3
{
	template< template< pp_enum_datas(i_, class)> class Tpl,
		pp_enum_params(i_, class P), class Elem>
	struct template_replace_at_c< Tpl<pp_enum_params(i_, P)>, j_, Elem>
	{
		typedef Tpl<pp_enum_params(j_, P) pp_comma_if(j_) Elem pp_comma_if( pp_not_equal(j_inc, i_) )
			pp_enum_params_from_to(j_inc, i_, P) >			type;
	};
}

#undef  j_inc
#undef  j_

#endif		// pp_iteration_depth()


#endif // pp_is_iterating
