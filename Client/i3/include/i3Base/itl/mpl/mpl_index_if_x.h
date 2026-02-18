#pragma once

#include "mpl_index_if.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class Res>
			struct index_if_x_impl : eval_if<is_same<Res, void_>, long_<-1>, Res> {};
		}

		template<class Seq = na, class Pred = na>
		struct index_if_x : aux::index_if_x_impl< typename index_if<Seq, Pred>::type > {};

	}
}

#define na_spec (2, index_if_x)
#include "aux_/mpl_na_spec.h"