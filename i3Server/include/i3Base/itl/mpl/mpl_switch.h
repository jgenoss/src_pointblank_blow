#pragma once

#include "mpl_find_if.h"
#include "mpl_deref.h"
#include "mpl_lambda.h"
#include "mpl_apply.h"
#include "mpl_pair.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		
		template< class Body = na, class T = na >
		struct switch_
		{
			typedef typename find_if<	Body, 
										apply1< lambda< first<_1> >, T > >::type iter_;
		        
			typedef typename deref<iter_>::type pair_;
			typedef typename lambda< typename second<pair_>::type >::type f_;
			typedef typename apply1<f_,T>::type type;
		};

	}
}

#define na_spec (2, switch_)
#include "aux_/mpl_na_spec.h"
