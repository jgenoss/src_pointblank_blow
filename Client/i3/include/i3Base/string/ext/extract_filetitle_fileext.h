#pragma once

//
// _splitpath스타일 구현도 나쁘진 않지만..
// 일단은 개별함수들을 구현해둔다..
// 
#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/range/as_literal.h"
#include "../../itl/range/algorithm/find_first_of.h"
#include "../../itl/range/copy_range.h"
#include "generic_string_copy.h"

namespace i3
{
	namespace detail
	{
		template<class Range, class Ch> 
		void extract_filetitle_fileext_iterator_range_imp( const Range& path, i3::iterator_range<Ch*>& outtitle, i3::iterator_range<Ch*>& outext)
		{
			typedef typename get_reverse_iterator_range_type<const Range>::type rev_range_type;

			rev_range_type	lit_rev_rng = i3::as_reverse_literal(path);
				
			typedef typename rev_range_type::iterator		rev_iterator;
			typedef typename rev_iterator::iterator_type	iterator;
			
			iterator it_beg = lit_rev_rng.end().base();
			iterator it_end = lit_rev_rng.begin().base();	// 
		
			rev_iterator rev_iter = i3::range::find_first_of(lit_rev_rng, ".\\/");
			
			if (rev_iter != lit_rev_rng.end() )
			{
				if ( *rev_iter == '.')
				{
					outext = i3::make_iterator_range( rev_iter.base(), it_end);
					
					it_end = (++rev_iter).base();

					lit_rev_rng = i3::iterator_range<rev_iterator>( rev_iter, lit_rev_rng.end() );
				
					rev_iter = i3::range::find_first_of(lit_rev_rng, "\\/");
				}
				else
				{
					outext = i3::make_iterator_range(it_end, it_end);
				}
				it_beg = rev_iter.base();				// 없으면 rend()인데 begin()으로 변경됨..
			}
			else
			{
				outext = i3::make_iterator_range(it_end, it_end);
			}

			outtitle = i3::make_iterator_range(it_beg, it_end);
		}
	}	

	template<class Range, class Ch> inline
	void extract_filetitle_fileext_iterator_range( const Range& path, i3::iterator_range<Ch*>& outtitle, i3::iterator_range<Ch*>& outext)
	{
		detail::extract_filetitle_fileext_iterator_range_imp(i3::as_literal(path), outtitle, outext);
	}

	namespace detail
	{
		
		template< class Lit_OR_SEQ1, class Lit_OR_SEQ2, class Range> inline
		void extract_filetitle_fileext_impl(const Range& path, Lit_OR_SEQ1 outTitle, Lit_OR_SEQ2 outExt)
		{
			typedef typename i3::get_iterator_range_type<const Range>::type		it_range_type;
			it_range_type	ot, oe;
			extract_filetitle_fileext_iterator_range(path, ot, oe);

			i3::generic_string_copy(outTitle, ot);
			i3::generic_string_copy(outExt, oe);
		}
	}
	
	
	template<class Range, class Lit1, class Lit2> inline 
	typename i3::enable_if<mpl::and_< is_iterator<Lit1>, is_iterator<Lit2> > >::type
	extract_filetitle_fileext( const Range& path, Lit1 outTitle, Lit2 outExt)
	{
		detail::extract_filetitle_fileext_impl<Lit1, Lit2>(i3::as_literal(path), outTitle, outExt);
	}
	
	template<class Range, class Lit1, class Seq2> inline 
	typename i3::enable_if<mpl::and_< is_iterator<Lit1>, mpl::not_<is_iterator<Seq2> > > >::type
	extract_filetitle_fileext( const Range& path, Lit1 outTitle, Seq2& outExt)
	{
		detail::extract_filetitle_fileext_impl<Lit1, Seq2&>(i3::as_literal(path), outTitle, outExt);
	}

	template<class Range, class Seq1, class Lit2> inline 
	typename i3::enable_if<mpl::and_< mpl::not_<is_iterator<Seq1> >, is_iterator<Lit2> > >::type
	extract_filetitle_fileext( const Range& path, Seq1& outTitle, Lit2 outExt)
	{
		detail::extract_filetitle_fileext_impl<Seq1&, Lit2>(i3::as_literal(path), outTitle, outExt);
	}	

	template<class Range, class Seq1, class Seq2> inline 
	typename i3::enable_if< mpl::not_< mpl::or_< is_iterator<Seq1>, is_iterator<Seq2> > > >::type
	extract_filetitle_fileext( const Range& path, Seq1& outTitle, Seq2& outExt)
	{
		detail::extract_filetitle_fileext_impl<Seq1&, Seq2&>(i3::as_literal(path), outTitle, outExt);
	}
	
	
}