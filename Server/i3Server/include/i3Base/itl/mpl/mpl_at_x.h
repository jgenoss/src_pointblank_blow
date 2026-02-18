#pragma once

#include "mpl_at.h"
#include "../type_traits/has_value.h"

namespace i3
{
	namespace mpl
	{

	// 임수빈 // 별도추가.. integral value가 존재하는 경우만, value를 노출시킨다..
		namespace aux
		{
			template < class ImplBase, bool HasValue = has_integral_value<typename ImplBase::type>::value >
			struct at_x_selector;

			template<class ImplBase, bool HasValue>
			struct at_x_selector : ImplBase
			{
				typedef typename ImplBase::type type;
				typedef typename if_< has_value_type<type>, typename type::value_type, long>::type		value_type;		// value_type이 없다면 long으로 대체한다..
				static const value_type	value =	type::value;
			};

			template<class ImplBase>
			struct at_x_selector<ImplBase, false> : ImplBase {};
		}

// 수정본..

		template<class Seq = na, class N = na>
		struct at_x : aux::at_x_selector<typename at_impl<typename sequence_tag<Seq>::type>::template apply<Seq, N> >{};
		
		template<class Seq, long N>
		struct at_x_c : aux::at_x_selector<typename at_impl<typename sequence_tag<Seq>::type>::template apply<Seq, long_<N> > >{};
		


	}
}

#define na_spec (2, at_x)
#include "aux_/mpl_na_spec.h"
