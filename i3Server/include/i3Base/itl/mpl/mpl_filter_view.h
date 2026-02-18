#pragma once

#include "mpl_begin_end.h"
#include "mpl_lambda.h"
#include "aux_/mpl_filter_iter.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		
		template<class Seq = na, typename Pred = na>
		struct filter_view
		{
		 private:    
			typedef typename lambda<Pred>::type pred_;
			typedef typename begin<Seq>::type first_;
			typedef typename end<Seq>::type last_;
		 public:
			struct tag;
			typedef filter_view type;
			typedef typename aux::next_filter_iter< first_,last_,pred_ >::type begin;
			typedef aux::filter_iter< last_,last_,pred_ > end;
		};
	}
}

#define na_spec (2, filter_view)
#include "aux_/mpl_na_spec.h"
