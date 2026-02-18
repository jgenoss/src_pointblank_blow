#pragma once

// 구코드에 SplitFileName의 리턴값을 이용한 구현이 있는데...파일명 찾는 루틴은 활용하지 않고,
// 파일명 앞자리에 널값만 넣는 구현으로 되어 있었음.. 이렇게 하는 대신, 
// 디렉토리에서 파일명을 제거하는 함수 추가 쪽으로 변경..
//
//  제거하면서 리턴값은 제거한 곳의 위치를 size_t로 알려주도록 처리..
//  
// 나중에 보니, RemoveFileName이란 함수도 별도로 존재했음...
//

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/algorithm/find_first_of.h"
#include "../../itl/range/generic_erase_iter_end.h"


namespace i3
{
	namespace detail
	{
		template<class LIT_OR_SEQ>
		std::size_t remove_filename_impl( LIT_OR_SEQ path)
		{
			typedef typename remove_reference<LIT_OR_SEQ>::type  src_type;

			typedef typename get_reverse_iterator_range_type<src_type>::type rev_range_type;

			rev_range_type	lit_rev_rng = i3::as_reverse_literal(path);

			typedef typename rev_range_type::iterator		rev_iterator;
			typedef typename rev_iterator::iterator_type	iterator;

			rev_iterator rev_iter = i3::range::find_first_of(lit_rev_rng, "\\/");
					
			iterator new_it_end = i3::generic_erase_iter_end( path, rev_iter.base());
			
			return i3::distance( lit_rev_rng.end().base(), new_it_end);
		}
	}

	template< class Lit> inline typename i3::enable_if_iterator<Lit, std::size_t>::type
	remove_filename(Lit inout)
	{
		return detail::remove_filename_impl<Lit>(inout);
	}

	template< class Seq> inline typename i3::disable_if_iterator<Seq, std::size_t>::type
	remove_filename(Seq& inout)
	{
		return detail::remove_filename_impl<Seq&>(inout);
	}
	
}

	
	