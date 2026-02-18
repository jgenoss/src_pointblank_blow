#if !defined(pp_is_iterating)

#ifndef mpl_list_h
#define mpl_list_h

#include "mpl_limit.h"
#include "mpl_na.h"
#include "mpl_list_n.h"
#include "../preprocessor/ext/pp_typename_if.h"
#include "../preprocessor/ext/pp_enum_params_spec.h"
#include "../preprocessor/pp_enum_params_with_a_default.h"

namespace i3
{

	//
	// 한가지 주의할 점은 list< >, listN< >은 상속관계일지라도, 템플릿특화에서는 완전이 다른 별개 타입이라는 점이 주의사항이며,
	// 따라서, 상등비교시에 is_same이 제대로 적용되지 못한다... 
	//

	namespace mpl
	{
		template< pp_enum_params_with_a_default(mpl_list_limit, class T, na) >
		struct list;

		template<pp_enum_params(mpl_list_limit, class T) >
		struct list : pp_cat(list, mpl_list_limit)<pp_enum_params(mpl_list_limit, T) >
		{ 
			typedef typename pp_cat(list, mpl_list_limit)<pp_enum_params(mpl_list_limit, T)>::type type;
		};
	}
}

#include	"../preprocessor/pp_iterate.h"

#define		pp_iteration_limits ( 0, pp_dec(mpl_list_limit) )		// 0 ~39까지는 특화 / 자체재귀..(양개구간임에 유의할것)
#define		pp_filename_1	"mpl_list.h"
#include	pp_iterate()

#endif // mpl_list_h

#else

#define		i_		pp_iteration()
	
namespace i3
{
	namespace mpl
	{
		template< pp_enum_params(i_, class T) >
		struct list<pp_enum_params_spec(mpl_list_limit, i_, T, na) > : pp_cat(list, i_)<pp_enum_params(i_, T)>
		{ 
			typedef pp_typename_if(i_) pp_cat(list, i_)<pp_enum_params(i_, T)>::type type;
		};
	}
}


#undef		i_

#endif // pp_is_iterating
