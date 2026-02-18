#pragma once

#include "mpl_at_fwd.h"
#include "mpl_long.h"
#include "mpl_sequence_tag.h"
#include "mpl_na.h"
#include "mpl_begin_end.h"
#include "mpl_advance.h"
#include "mpl_deref.h"

// 임수빈 추가..


namespace i3
{
	namespace mpl
	{
		// 아래는 일반구현으로 리스트는 이것을 그대로 따라간다..
		template<class Tag>
		struct at_impl
		{
			template<class Seq, class N> 
			struct apply 
			{
				typedef typename advance<typename begin<Seq>::type, N>::type iter_;
				typedef typename deref<iter_>::type		type;
			};
		};
		
		template<> struct at_impl<non_sequence_tag> {};
		
	
// 원본
		template<class Seq = na, class N = na>
		struct at : at_impl<typename sequence_tag<Seq>::type>::template apply<Seq, N>{};
		
		template<class Seq, long N>
		struct at_c : at_impl<typename sequence_tag<Seq>::type>::template apply<Seq, long_<N> > {};
		
	}

}

#define na_spec (2, at)
#include "aux_/mpl_na_spec.h"
