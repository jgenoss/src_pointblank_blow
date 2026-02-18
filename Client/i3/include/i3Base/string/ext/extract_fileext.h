#pragma once

//
//   단일함수들도 좋지만, 개인적으로는 CRT함수인 _splitpath의 일반형도 추가로 
//   존재하는게 좋다고 생각한다..
//
#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/algorithm/find.h"
#include "../../itl/range/copy_range.h"
#include "../../itl/range/generic_clear.h"

namespace i3
{
	namespace detail
	{

		template< class Lit_OR_SEQ, class Range>
		void extract_fileext_impl(const Range& path, Lit_OR_SEQ out)
		{
			typedef typename get_reverse_iterator_range_type<const Range>::type rev_range_type;

			rev_range_type	lit_rev_rng = i3::as_reverse_literal(path);

			typedef typename rev_range_type::iterator		rev_iterator;
			typedef typename rev_iterator::iterator_type	iterator;

			rev_iterator rev_iter = i3::range::find(lit_rev_rng, '.');
			
			if (rev_iter == lit_rev_rng.end() )
			{
				i3::generic_clear(out);
				return;
			}

			iterator it_beg = rev_iter.base();
			iterator it_end = lit_rev_rng.begin().base();	// 

			i3::generic_string_copy(out, it_beg, it_end);
		}
	}

	
	template<class Range, class Lit> inline typename i3::enable_if_iterator<Lit>::type
	extract_fileext(const Range& path, Lit out)
	{
		detail::extract_fileext_impl<Lit>(i3::as_literal(path), out);
	}

	template<class Range, class Seq> inline typename i3::disable_if_iterator<Seq>::type
	extract_fileext(const Range& path, Seq& out)
	{
		detail::extract_fileext_impl<Seq&>(i3::as_literal(path), out);
	}
}