#pragma once

#include "../../itl/range/algorithm/copy_if.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/type_traits/make_unsigned.h"

namespace i3
{
	// 이건 i3Engine원본에서 , 기존 head/tail쪽 제거하는 것과 비교구문이 다르게 되있음
	// 배열의 경우 길이를 잴 필요가 없기 때문에, 원본을 조금 수정해서 쓴다..

	namespace detail
	{
		struct find_non_blank_pos_all_functor		// 구버전 코드 참조..
		{
			template<class C> inline
			bool operator()(C c) 
			{	
				typedef typename i3::make_unsigned<C>::type unsigned_C;

				return unsigned_C(c) > 32;									// TODO : 더 좋은 것이 있으면 다시 고칠 것..(2014.07.21.수빈)
			//	return !isspace(unsigned_C(c)) || ( unsigned_C(c) > 127);	// 유니코드 호환성이 없어서 일단 수정한다..
			}
		};
	}

	template<class Seq> inline 
	typename i3::disable_if_iterator<Seq>::type remove_all_blank( Seq& s)
	{
		// 이건 copy_if알고리즘을 활용..
		i3::range::copy_if(s, detail::find_non_blank_pos_all_functor() );
	}

	// 문자열 리터럴의 널값문제...
	// as_literal로 iterator_range를 먼저 세우면 길이값부터 먼저 계산되는게 단점..
	// 그렇게 안하는 방식이 필요함..
	//

	template<class It> inline
	typename i3::enable_if_iterator<It>::type remove_all_blank(It it)
	{
		typedef typename i3::iterator_traits<It>::value_type	value_type;
		i3::iterator_range<It>	rng = i3::as_literal(it);
		It dest = i3::copy_if(i3::begin(rng), i3::end(rng), detail::find_non_blank_pos_all_functor());
		*dest = value_type();
	}

}