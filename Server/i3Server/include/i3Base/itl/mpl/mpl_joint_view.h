#pragma once

#include "aux_/mpl_joint_iter.h"
#include "mpl_plus.h"
#include "mpl_size_fwd.h"
#include "mpl_begin_end.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		namespace aux { struct joint_view_tag; }

		template<>
		struct size_impl< aux::joint_view_tag >
		{
			template < class JointView > 
			struct apply : plus<size<typename JointView::sequence1_>, size<typename JointView::sequence2_> > {};
		};

		template<class Seq1 = na, class Seq2 = na>
		struct joint_view
		{
			typedef typename mpl::begin<Seq1>::type   first1_;
			typedef typename mpl::end<Seq1>::type     last1_;
			typedef typename mpl::begin<Seq2>::type   first2_;
			typedef typename mpl::end<Seq2>::type     last2_;

			// agurt, 25/may/03: for the 'size_traits' implementation above
			typedef Seq1 sequence1_;
			typedef Seq2 sequence2_;

			typedef joint_view type;
			typedef aux::joint_view_tag tag;
			typedef joint_iter<first1_,last1_,first2_>  begin;
			typedef joint_iter<last1_,last1_,last2_>    end;
		};
	}
}

#define na_spec (2, joint_view)
#include "aux_/mpl_na_spec.h"
