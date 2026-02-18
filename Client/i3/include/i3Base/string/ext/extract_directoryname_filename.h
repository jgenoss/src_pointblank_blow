#pragma once

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
		void extract_directoryname_filename_iterator_range_imp( 
			const Range& path, i3::iterator_range<Ch*>& outDir, 
			i3::iterator_range<Ch*>& outFile)
		{
			
			typedef typename get_reverse_iterator_range_type<const Range>::type rev_range_type;

			rev_range_type	lit_rev_rng = i3::as_reverse_literal(path);
				
			typedef typename rev_range_type::iterator		rev_iterator;
			typedef typename rev_iterator::iterator_type	iterator;
			
			iterator it_beg = lit_rev_rng.end().base();
			iterator it_end = lit_rev_rng.begin().base();	// 
		
			rev_iterator rev_iter = i3::range::find_first_of(lit_rev_rng, "\\/");
			
			if (rev_iter != lit_rev_rng.end() )
			{
				iterator it_mid = (++rev_iter).base();	
				outDir = i3::make_iterator_range(it_beg, it_mid);
				outFile = i3::make_iterator_range(++it_mid, it_end);
			}
			else
			{
				outDir   = i3::make_iterator_range(it_beg, it_beg);
				outFile  = i3::make_iterator_range(it_beg, it_end);
			}
		}
	}

	template<class Range, class Ch> inline
	void extract_directoryname_filename_iterator_range( 
		const Range& path, i3::iterator_range<Ch*>& outDir, 
		i3::iterator_range<Ch*>& outFile)
	{
		return detail::extract_directoryname_filename_iterator_range_imp(i3::as_literal(path), outDir, outFile);
	}

	namespace detail
	{
		template<class Lit_OR_SEQ1, class Lit_OR_SEQ2, class Range> inline
		void extract_directoryname_filename_impl(const Range& path, Lit_OR_SEQ1 outDir, Lit_OR_SEQ2 outFile)
		{
			typedef typename i3::get_iterator_range_type<const Range>::type		it_range_type;
			it_range_type	od, of;
			extract_directoryname_filename_iterator_range(path, od, of);
			i3::generic_string_copy(outDir, od);
			i3::generic_string_copy(outFile, of);
		}
	}
	

	template<class Range, class Lit1, class Lit2> inline 
	typename i3::enable_if<mpl::and_< is_iterator<Lit1>, is_iterator<Lit2> > >::type
	extract_directoryname_filename( const Range& path, Lit1 outDir, Lit2 outFile)
	{
		detail::extract_directoryname_filename_impl<Lit1, Lit2>(i3::as_literal(path), outDir, outFile);
	}
	
	template<class Range, class Lit1, class Seq2> inline 
	typename i3::enable_if<mpl::and_< is_iterator<Lit1>, mpl::not_<is_iterator<Seq2> > > >::type
	extract_directoryname_filename( const Range& path, Lit1 outDir, Seq2& outFile)
	{
		detail::extract_directoryname_filename_impl<Lit1, Seq2&>(i3::as_literal(path), outDir, outFile);
	}

	template<class Range, class Seq1, class Lit2> inline 
	typename i3::enable_if<mpl::and_< mpl::not_<is_iterator<Seq1> >, is_iterator<Lit2> > >::type
	extract_directoryname_filename( const Range& path, Seq1& outDir, Lit2 outFile)
	{
		detail::extract_directoryname_filename_impl<Seq1&, Lit2>(i3::as_literal(path), outDir, outFile);
	}	

	template<class Range, class Seq1, class Seq2> inline 
	typename i3::enable_if< mpl::not_< mpl::or_< is_iterator<Seq1>, is_iterator<Seq2> > > >::type
	extract_directoryname_filename( const Range& path, Seq1& outDir, Seq2& outFile)
	{
		detail::extract_directoryname_filename_impl<Seq1&, Seq2&>(i3::as_literal(path), outDir, outFile);
	}
		
}