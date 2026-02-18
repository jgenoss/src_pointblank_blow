#if !defined(pp_is_iterating)

#ifndef template_at_h
#define template_at_h

#define template_at_limit		20

namespace i3
{
	// 템플릿 클래스가 아닌 경우나 인덱스가 맞지 않으면 void타입 리턴..
	template<class T, long N>
	struct template_at_c {	typedef void	type;	};
	
	template<class T, class N>
	struct template_at : template_at_c<T, long(N::value)> {};

}

#define pp_iteration_params_1 (3, (1, template_at_limit, "template_at.h") )		// 1개부터..
#include pp_iterate()

#endif // template_at_h

#else // pp_is_iterating

#if				pp_iteration_depth() == 1

#define i_		pp_frame_iteration(1)


#define pp_iteration_params_2 (3, (0, pp_dec(i_), "template_at.h") )		// 0번 부터..
#include pp_iterate()

#undef  i_


#elif		pp_iteration_depth() == 2

#define j_		pp_frame_iteration(2)

// i_ 는 1부터 , j_는 0부터..2중루프

namespace i3
{
	template< template< pp_enum_datas(i_, class)> class Tpl,
			pp_enum_params(i_, class P)>
	struct template_at_c< Tpl<pp_enum_params(i_, P)>, j_>
	{
		typedef pp_cat(P, j_)	type;
	};
}

#undef  j_

#endif		// pp_iteration_depth()


#endif // pp_is_iterating
