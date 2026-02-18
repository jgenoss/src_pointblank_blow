#pragma once

#include "../mpl_apply.h"
#include "../mpl_iterator_tags.h"
#include "../mpl_next_prior.h"
#include "../mpl_deref.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class Iterator, class LastIterator, class F>
			struct transform_iter
			{
				typedef Iterator base;
				typedef forward_iterator_tag category;
				typedef transform_iter< typename mpl::next<base>::type,LastIterator,F > next;
			    typedef typename apply1< F, typename deref<base>::type>::type type;
			};

			template<class LastIterator, class F>
			struct transform_iter< LastIterator,LastIterator,F >
			{
				typedef LastIterator base;
				typedef forward_iterator_tag category;
			};
		}
	}
}

#define lambda_spec (3, aux::transform_iter)
#include "mpl_lambda_spec.h"

