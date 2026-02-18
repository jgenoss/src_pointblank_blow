#pragma once

#include "mpl_transform.h"
#include "mpl_begin_end.h"
#include "mpl_iterator_tags.h"
#include "mpl_next_prior.h"
#include "mpl_lambda.h"
#include "mpl_deref.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		template< class IteratorSeq >
		struct zip_iterator
		{
			typedef forward_iterator_tag category;
			typedef typename transform1< IteratorSeq, deref<_1> >::type type;

			typedef zip_iterator< typename transform1< IteratorSeq, mpl::next<_1> >::type > next;
		};

		template<class Seqs = na>
		struct zip_view
		{
		 private:
			typedef typename transform1< Seqs, mpl::begin<_1> >::type first_ones_;
			typedef typename transform1< Seqs, mpl::end<_1> >::type last_ones_;
		 public:
			typedef nested_begin_end_tag tag;
			typedef zip_view type;
			typedef zip_iterator<first_ones_> begin;
			typedef zip_iterator<last_ones_> end;
		};
				
	}
}

#define na_spec (1, zip_view)
#include "aux_/mpl_na_spec.h"
