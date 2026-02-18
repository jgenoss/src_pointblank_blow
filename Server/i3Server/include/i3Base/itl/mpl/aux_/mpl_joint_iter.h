#pragma once

#include "../mpl_next_prior.h"
#include "../mpl_deref.h"
#include "../mpl_iterator_tags.h"

namespace i3
{
	namespace mpl
	{
		
		template<class Iterator1, class LastIterator1, class Iterator2>
		struct joint_iter
		{
			typedef Iterator1 base;
			typedef forward_iterator_tag category;
		};

		template<class LastIterator1, class Iterator2>
		struct joint_iter<LastIterator1,LastIterator1,Iterator2>
		{
			typedef Iterator2 base;
			typedef forward_iterator_tag category;
		};

		template< class I1, class L1, class I2 >
		struct deref< joint_iter<I1,L1,I2> >
		{
			typedef typename joint_iter<I1,L1,I2>::base base_;
			typedef typename deref<base_>::type type;
		};

		template< class I1, class L1, class I2 >
		struct next< joint_iter<I1,L1,I2> >
		{
			typedef joint_iter< typename mpl::next<I1>::type,L1,I2 > type;
		};

		template< class L1, class I2 >
		struct next< joint_iter<L1,L1,I2> >
		{
			typedef joint_iter< L1,L1,typename mpl::next<I2>::type > type;
		};
		
	}
}

#define lambda_spec (3, joint_iter)
#include "mpl_lambda_spec.h"

