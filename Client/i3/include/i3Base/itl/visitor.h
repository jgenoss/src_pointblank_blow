#if !defined(pp_is_iterating)

#ifndef i3_visitor_h
#define i3_visitor_h

#include "preprocessor/repetition/pp_enum_params_with_a_default.h"
#include "preprocessor/repetition/pp_enum_params.h"
#include "preprocessor/ext/pp_enum_datas.h"

#include "mpl/mpl_na.h"
#include "mpl/mpl_vector.h"

#define i3_acceptor_limit		30			//

namespace i3
{
	// i3::visitor<R> 이 기본클래스..하지만 가상함수는 템플릿을 동반하면 안되기 때문에 비템플릿 클래스가 필요함..

	template<class R, pp_enum_params_with_a_default(i3_acceptor_limit, class Acceptor, mpl::na)>
	struct visitor;

	namespace detail
	{
		template<class R, class Seq>
		struct visitor_sequence : public visitor_sequence<R, typename mpl::pop_front<Seq>::type >
		{
			typedef typename mpl::at_c<Seq, 0>::type Acceptor;

			virtual R operator()(Acceptor* a) { return R();  }
		};

		template<class R>
		struct visitor_sequence<R, mpl::vector0<> >
		{
			virtual ~visitor_sequence() {}
		};

	}


	template<class R, pp_enum_params(i3_acceptor_limit, class Acceptor)>
	struct visitor : public visitor<class R, pp_enum_shifted_params(i3_acceptor_limit, class Acceptor)>
	{
		virtual R operator()(Acceptor0* acceptor) { return R(); }
	};

	template<class R>
	struct visitor<R, pp_enum_datas(i3_acceptor_limit, mpl::na)>
	{
		virtual ~visitor() {}
	};

}


#endif	// i3_visitor_h

#else	// pp_is_iterating

#define		i_		pp_iteration()



#undef i_

#endif // pp_is_iterating
