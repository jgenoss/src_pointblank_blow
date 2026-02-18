#pragma once

#include "aux_/mpl_find_if_pred.h"
#include "mpl_begin_end.h"
#include "mpl_if.h"
#include "mpl_int.h"
#include "mpl_iter_fold_if.h"
#include "mpl_next_prior.h"
#include "mpl_void.h"
#include "mpl_na.h"


namespace i3
{
	
	namespace mpl
	{
		// 순차열에 traits류의 메타함수를 집어넣고, 인덱스 획득..

		template<class Seq = na, class Pred = na>
		struct index_if
		{
			typedef typename iter_fold_if<Seq, int_<0>, next<>, aux::find_if_pred<Pred> >::type result_;
			
			typedef typename end<Seq>::type				not_found_;
			typedef typename first<result_>::type		result_index_;
			typedef typename second<result_>::type		result_iterator_;
			typedef typename if_< is_same< result_iterator_, not_found_>, void_, result_index_>::type	type;
		};

	}

}

#define na_spec (2, index_if)
#include "aux_/mpl_na_spec.h"
