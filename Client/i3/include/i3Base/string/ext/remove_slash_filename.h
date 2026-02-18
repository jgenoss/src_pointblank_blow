#pragma once

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/algorithm/find_first_of.h"
#include "../../itl/range/generic_erase_iter_end.h"

//
//  remove_filename과 끝슬래시까지 같이 제거..  (함수별 호출보다 따로 함수가 있는게 낫다고 판단되서 따로 작성)
//

namespace i3
{
	namespace detail
	{
		template<class LIT_OR_SEQ>
		std::size_t remove_slash_filename_impl( LIT_OR_SEQ path)
		{
			typedef typename remove_reference<LIT_OR_SEQ>::type  src_type;

			typedef typename get_reverse_iterator_range_type<src_type>::type rev_range_type;

			rev_range_type	lit_rev_rng = i3::as_reverse_literal(path);

			typedef typename rev_range_type::iterator		rev_iterator;
			typedef typename rev_iterator::iterator_type	iterator;

			rev_iterator rev_iter = i3::range::find_first_of(lit_rev_rng, "\\/");
			
			if ( rev_iter != lit_rev_rng.end() )
				++rev_iter;

			iterator new_it_end = i3::generic_erase_iter_end( path, rev_iter.base());
			
			return i3::distance( lit_rev_rng.end().base(), new_it_end);
		}
	}

	template< class Lit> inline typename i3::enable_if_iterator<Lit, std::size_t>::type
	remove_slash_filename(Lit inout)
	{
		return detail::remove_slash_filename_impl<Lit>(inout);
	}

	template< class Seq> inline typename i3::disable_if_iterator<Seq, std::size_t>::type
	remove_slash_filename(Seq& inout)
	{
		return detail::remove_slash_filename_impl<Seq&>(inout);
	}
	
	
}