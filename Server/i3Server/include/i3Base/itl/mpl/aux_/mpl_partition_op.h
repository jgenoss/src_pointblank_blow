#pragma once

#include "../mpl_apply.h"
#include "../mpl_if.h"
#include "../mpl_pair.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< class Pred, class In1Op, class In2Op >
			struct partition_op
			{
				template< class State, class T >
				struct apply
				{
					typedef typename State::first first_;
					typedef typename State::second second_;
					typedef typename apply1< Pred,T >::type pred_;

					typedef typename eval_if< pred_, apply2<In1Op,first_,T>, apply2<In2Op,second_,T> >::type result_;
					typedef typename if_< pred_, pair< result_,second_ >, pair< first_,result_ > >::type type;
				};
			};
		}
	}
}

#define lambda_spec (3, aux::partition_op)
#include "mpl_lambda_spec.h"
