#pragma once

#include "mpl_begin_end.h"
#include "mpl_lambda.h"
#include "aux_/mpl_transform_iter.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		
		template<class Seq = na, class F = na>
		struct transform_view
		{
		 private:
			typedef typename lambda<F>::type f_;
			typedef typename begin<Seq>::type first_;
			typedef typename end<Seq>::type last_;
		 public:
			struct tag;
			typedef transform_view type;
			typedef aux::transform_iter< first_,last_,f_ > begin;
			typedef aux::transform_iter< last_,last_,f_ > end;
		};
	}
}

#define na_spec (2, transform_view)
#include "aux_/mpl_na_spec.h"
