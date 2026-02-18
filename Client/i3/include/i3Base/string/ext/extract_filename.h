#pragma once

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/algorithm/find_first_of.h"
#include "../../itl/range/copy_range.h"

#include "make_generic_literal.h"

// 기존 SplitFileName에 대한 대응..
// 기존 함수는 엔진 곳곳에서 현재 간간이 쓰이고 있다.
// 
namespace i3
{
	namespace detail
	{
		template<class Range, class DestRange>
		void extract_filename_range_imp(const Range& path, DestRange& out)
		{
			typedef typename get_reverse_iterator_range_type<const Range>::type rev_range_type;

			rev_range_type	lit_rev_rng = i3::as_reverse_literal(path);

			typedef typename rev_range_type::iterator		rev_iterator;
			typedef typename rev_iterator::iterator_type	iterator;
			typedef typename i3::iterator_value<iterator>::type Ch;

			i3::iterator_range<const Ch*> delimitRng = i3::as_literal(make_generic_literal(Ch, "\\/"));

			rev_iterator rev_iter = i3::range::find_first_of(lit_rev_rng, delimitRng);

			out.assign( rev_iter.base(), path.end() );		
		}
		
		template<class Range, class DestRange>
		void extract_filetitle_range_imp(const Range& path, DestRange& out)
		{
			typedef typename get_reverse_iterator_range_type<const Range>::type rev_range_type;

			rev_range_type	lit_rev_rng = i3::as_reverse_literal(path);

			typedef typename rev_range_type::iterator		rev_iterator;
			typedef typename rev_iterator::iterator_type	iterator;
			typedef typename i3::iterator_value<iterator>::type Ch;

			iterator it_beg = path.begin(); 
			iterator it_end = path.end();	// 

			i3::iterator_range<const Ch*> delimitRng1 = i3::as_literal(make_generic_literal(Ch, ".\\/"));
		
			rev_iterator rev_iter = i3::range::find_first_of(lit_rev_rng, delimitRng1);

			if (rev_iter != lit_rev_rng.end() )
			{
				if ( *rev_iter == '.')
				{
					it_end = (++rev_iter).base();

					lit_rev_rng = i3::iterator_range<rev_iterator>( rev_iter, lit_rev_rng.end() );

					i3::iterator_range<const Ch*> delimitRng2 = i3::as_literal(make_generic_literal(Ch, "\\/"));

					rev_iter = i3::range::find_first_of(lit_rev_rng, delimitRng2);
				}
				it_beg = rev_iter.base();				// 없으면 rend()인데 begin()으로 변경됨..
			}
			
			out.assign(it_beg, it_end);
		}


		template<class Lit_OR_SEQ, class Range> inline
		void extract_filename_impl(const Range& path, Lit_OR_SEQ out)
		{
			Range result_rng;
			extract_filename_range_imp(path, result_rng);
			i3::generic_string_copy(out, result_rng);
		}
		
		template<class Lit_OR_SEQ, class Range> inline
		void extract_filetitle_impl(const Range& path, Lit_OR_SEQ out)
		{
			Range result_rng;
			extract_filetitle_range_imp(path, result_rng);
			i3::generic_string_copy(out, result_rng);
		}

	}

	template<class Range, class Lit> inline typename i3::enable_if_iterator<Lit>::type
	extract_filename(const Range& path, Lit out)
	{
		detail::extract_filename_impl<Lit>(i3::as_literal(path), out);
	}
		
	template<class Range, class Seq> inline typename i3::disable_if_iterator<Seq>::type
	extract_filename(const Range& path, Seq& out)
	{
		detail::extract_filename_impl<Seq&>(i3::as_literal(path), out);
	}

	template<class Range, class Lit> inline typename i3::enable_if_iterator<Lit>::type
	extract_filetitle(const Range& path, Lit out)
	{
		detail::extract_filetitle_impl<Lit>(i3::as_literal(path), out);
	}

	template<class Range, class Seq> inline typename i3::disable_if_iterator<Seq>::type
	extract_filetitle(const Range& path, Seq& out)
	{
		detail::extract_filetitle_impl<Seq&>(i3::as_literal(path), out);
	}

	template<class Range, class Ch> inline 
	void	extract_filename_iterator_range(const Range& path, i3::iterator_range<Ch*>& out)
	{
		detail::extract_filename_range_imp(i3::as_literal(path), out);
	}
	
	template<class Range, class Ch> inline 
	void	extract_filetitle_iterator_range(const Range& path, i3::iterator_range<Ch*>& out)
	{
		detail::extract_filetitle_range_imp(i3::as_literal(path), out);
	}
	
}