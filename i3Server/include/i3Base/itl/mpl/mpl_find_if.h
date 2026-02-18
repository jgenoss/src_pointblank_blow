#pragma once

#include "aux_/mpl_find_if_pred.h"
#include "mpl_arg.h"
#include "mpl_iter_fold_if.h"
#include "mpl_na.h"


namespace i3
{
	namespace mpl
	{
		template< class Seq = na, class Pred = na >
		struct find_if
		{
			typedef typename iter_fold_if<Seq, void, mpl::arg<1>, protect< aux::find_if_pred<Pred> > >::type result_;
			typedef typename second<result_>::type type;
		};

	}
}


#define na_spec (2, find_if)
#include "aux_/mpl_na_spec.h"
