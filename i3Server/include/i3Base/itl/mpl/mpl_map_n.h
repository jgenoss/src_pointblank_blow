#if !defined(pp_is_iterating)

#ifndef mpl_map_n_h
#define mpl_map_n_h

#include "mpl_map_core.h"


#include	"../preprocessor/pp_iterate.h"

#define		pp_iteration_limits ( 1, mpl_map_limit )		// 1 ~20까지는 특화 / 자체재귀..(양개구간)
#define		pp_filename_1	"mpl_map_n.h"
#include	pp_iterate()



#endif //mpl_map_n_h

#else // pp_is_iterating

#define		i_			pp_iteration()
#define		dec_i_		pp_dec(i_)

namespace i3
{
	namespace mpl
	{
		template<class Map>
		struct m_at<Map, dec_i_>
		{
			typedef typename Map::pp_cat(item, dec_i_) type;
		};

		template< class Key, class T, class Base >
		struct m_item<i_,Key,T,Base> : m_item_<Key,T,Base>
		{
			typedef pair<Key,T> pp_cat(item,dec_i_);
		};
		
		template< pp_enum_params(i_, class P) >
		struct pp_cat(map,i_) : m_item<	i_, typename pp_cat(P,dec_i_)::first, typename pp_cat(P,dec_i_)::second, 
										pp_cat(map,dec_i_)<pp_enum_params(dec_i_, P)> >
		{
			typedef pp_cat(map,i_) type;
		};

	}
}

#undef dec_i_
#undef i_

#endif // pp_is_iterating
