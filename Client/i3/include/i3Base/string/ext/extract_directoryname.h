#pragma once

// 구버전 SplitPath를 변경..
#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/algorithm/find_first_of.h"
#include "../../itl/range/copy_range.h"
#include "../../itl/range/generic_clear.h"
#include "make_generic_literal.h"
#include "../../itl/iterator/iterator_traits.h"

namespace i3
{
	
	namespace detail
	{
		// 첫번째 인수는 명기하도록 바꾸고, 두번째인수는 추론하게 만든다..

		template<class Lit_OR_SEQ, class Range>
		void extract_directoryname_impl( Lit_OR_SEQ out, const Range& path)
		{
			typedef typename get_reverse_iterator_range_type<const Range>::type rev_range_type;

			rev_range_type	lit_rev_rng = i3::as_reverse_literal(path);

			typedef typename rev_range_type::iterator		rev_iterator;
			typedef typename rev_iterator::iterator_type	iterator;
			typedef typename i3::iterator_value<iterator>::type Ch;

			const i3::iterator_range<const Ch*> delimit(i3::as_literal(make_generic_literal(Ch,"\\/")));

			rev_iterator rev_iter = i3::range::find_first_of(lit_rev_rng, delimit); 

			iterator it_beg = lit_rev_rng.end().base();	// 
			iterator it_end = rev_iter.base();			// 없으면 rend()인데 begin()으로 변경됨..
			
			if (it_beg == it_end)
			{
				i3::generic_clear(out);
			}
			else
			{
				i3::generic_string_copy(out, it_beg, --it_end);		// \\ / 는 제외시킨다.
			}
		}
			
	}
	
	template<class Range, class Lit> inline typename i3::enable_if_iterator<Lit>::type
	extract_directoryname(const Range& path, Lit out)
	{
		detail::extract_directoryname_impl<Lit>(out, i3::as_literal(path));
	}

	template<class Range, class Seq> inline typename i3::disable_if_iterator<Seq>::type
	extract_directoryname(const Range& path, Seq& out)
	{
		detail::extract_directoryname_impl<Seq&>(out, i3::as_literal(path));
	}

}