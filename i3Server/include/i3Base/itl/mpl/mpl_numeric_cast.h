#pragma once

#include "mpl_apply_wrap.h"

namespace i3
{
	namespace mpl
	{

		template< class SourceTag, class TargetTag > 
		struct numeric_cast {	template<class N> struct apply;	};				// 현재 미정상태....(꼭 필요한 것은 아니다..)
		
		namespace aux
		{
			template< class F, class Tag1, class Tag2>
			struct cast1st_impl
			{
				template< class N1, class N2 > 
				struct apply : apply_wrap2< F, typename apply_wrap1< numeric_cast<Tag1,Tag2>,N1 >::type, N2>{};
			};

			template<class F, class Tag1, class Tag2>
			struct cast2nd_impl
			{
				template< class N1, class N2 > 
				struct apply : apply_wrap2< F, N1, typename apply_wrap1< numeric_cast<Tag2,Tag1>,N2 >::type>{};
			};
		}
	}
}