#if !defined(pp_is_iterating)

#ifndef mpl_map_h
#define mpl_map_h

#include "mpl_limit.h"
#include "mpl_na.h"
#include "mpl_map_n.h"

namespace i3
{
	namespace mpl
	{
		template< pp_enum_params_with_a_default(mpl_map_limit, class T, na) >
		struct map;

		template<pp_enum_params(mpl_map_limit, class T) >
		struct map : pp_cat(map, mpl_map_limit)<pp_enum_params(mpl_map_limit, T) >
		{ 
			typedef typename pp_cat(map, mpl_map_limit)<pp_enum_params(mpl_map_limit, T)>::type type;
		};
	}
}


#include	"../preprocessor/pp_iterate.h"

#define		pp_iteration_limits ( 0, pp_dec(mpl_map_limit) )		// 0 ~39까지는 특화 / 자체재귀..(양개구간임에 유의할것)
#define		pp_filename_1	"mpl_map.h"
#include	pp_iterate()


#endif // mpl_map_h

#else


#define		i_		pp_iteration()
	
namespace i3
{
	namespace mpl
	{
		template< pp_enum_params(i_, class T) >
		struct map<pp_enum_params_spec(mpl_map_limit, i_, T, na) > : pp_cat(map, i_)<pp_enum_params(i_, T)>
		{ 
			typedef pp_typename_if(i_) pp_cat(map, i_)<pp_enum_params(i_, T)>::type type;
		};
	}
}


#undef		i_



#endif // pp_is_iterating


