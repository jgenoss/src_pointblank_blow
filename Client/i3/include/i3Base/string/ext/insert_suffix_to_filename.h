#pragma once

#include "../../itl/range/generic_append_range.h"
#include "../../itl/range/copy_range.h"
#include "../../itl/range/algorithm/find.h"

namespace i3
{
	namespace detail 
	{
		template<class LIT_OR_SEQ, class SrcRange, class SuffixRange>
		void insert_suffix_to_filename_impl(LIT_OR_SEQ ctn, const SrcRange& src, 
			const SuffixRange& suffix)
		{
			typedef typename get_reverse_iterator_range_type<const SrcRange>::type 
				rev_iter_range_type;
			typedef typename rev_iter_range_type::value_type		value_type;
			typedef typename rev_iter_range_type::iterator			rev_iterator;

			rev_iter_range_type rev_rng = i3::make_reverse_iterator_range(src);
			rev_iterator rev_it = i3::range::find(rev_rng, value_type('.'));
			
			if (rev_it == rev_rng.end())
			{
				i3::generic_string_copy(ctn, src);
				i3::generic_append_range(ctn, suffix);
			}
			else
			{
				typedef typename rev_iterator::iterator_type	iterator;
				iterator it_end_title = (++rev_it).base();
				
				i3::generic_string_copy(ctn, rev_rng.end().base(), it_end_title);
				i3::generic_append_range(ctn, suffix);
				i3::generic_append_range(ctn, it_end_title, rev_rng.begin().base() );
			}
		}

	}
	
	template<class Lit, class SrcRange, class SuffixRange> inline
	typename i3::enable_if_iterator<Lit>::type
	insert_suffix_to_filename(Lit ctn, const SrcRange& src, 
		const SuffixRange& suffix)
	{
		detail::insert_suffix_to_filename_impl<Lit>(
			ctn, i3::as_literal(src), i3::as_literal(suffix) );
	}

	template<class Seq, class SrcRange, class SuffixRange> inline
	typename i3::disable_if_iterator<Seq>::type
		insert_suffix_to_filename(Seq& ctn, const SrcRange& src, 
		const SuffixRange& suffix)
	{
		detail::insert_suffix_to_filename_impl<Seq&>(
			ctn, i3::as_literal(src), i3::as_literal(suffix) );
	}

}
