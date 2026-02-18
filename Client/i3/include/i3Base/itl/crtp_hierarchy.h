#pragma once

//
//  crtp를 중복해서 사용할 경우에 chain 문법이 일관성 없어 보이기 때문에 다음과 같은 헬퍼를 사용..
//	
//  crtp1<T, crtp2<T, crtp3<T, B> > >  ---> .다음과 같이 변경..
//
//  crtp_hierarchy< T, B,	crtp1<mpl::_,mpl::_>, 
//							crtp2<mpl::_,mpl::_>, 
//							crtp3<mpl::_,mpl::_> > --> . 길어지지만, 일관성을 가진다..
// crtp template은 crtp<T, B> 형태를 지녀야 한다..
//
//
//	mpl::apply함수가 적용되지 않고 컴파일 오류를 내는데, 검색해보니 람다식+자리표현식의 경우, 가변인수양식템플릿이 적절치 않아서
//  그렇다고 한다.  mpl::apply는 디폴트매개변수를 쥔 가변 템플릿메타함수라서 자리표현식이 잘 들어맞지 않는다..
//
//
//

#include "mpl/mpl_reverse_fold.h"
#include "mpl/mpl_vector.h"

#define crtp_hierarchy_limit	10

namespace i3
{



	template<class T, class B, pp_enum_params_1_with_a_default(crtp_hierarchy_limit, class crtp, mpl::na)>
	struct crtp_hierarchy;
	

	template<class T, class B, pp_enum_params_1(crtp_hierarchy_limit, class crtp) >
	struct crtp_hierarchy :  mpl::reverse_fold< typename mpl::vector<pp_enum_params_1(crtp_hierarchy_limit, crtp)>::type, B, 
		mpl::apply2<mpl::_2, T, mpl::_1> >::type
	{
	};
	
}
