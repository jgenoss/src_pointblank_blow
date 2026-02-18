#pragma once


#include "extract_filename.h"
#include "../../itl/range/generic_replace_range.h"
#include "../../itl/range/generic_append_range.h"
#include "generic_string_copy.h"

namespace i3
{
	
	namespace detail
	{
		template<class LitOrSeq, class Rng> inline
		void change_file_title_imp(LitOrSeq inout, const Rng& title)
		{	
			Rng old_title_rng;
			i3::extract_filetitle_iterator_range(inout, old_title_rng);
			i3::generic_replace_range(inout, 
				old_title_rng.begin(), old_title_rng.end(), title.begin(), title.end() );
		}
		
		template<class LitOrSeq, class Rng> inline
		void change_file_title_copy_imp(LitOrSeq out, const Rng& input, const Rng& title)
		{
			Rng old_title_rng;
			i3::extract_filetitle_iterator_range(input, old_title_rng);
			i3::generic_string_copy(out, input.begin(), old_title_rng.begin() );
			i3::generic_append_range(out, title);
			i3::generic_append_range(out, old_title_rng.end(), input.end());
		}
	}

	template<class Lit, class TitleRange > inline typename i3::enable_if_iterator<Lit>::type
	change_file_title(Lit inout, const TitleRange& title)
	{
		detail::change_file_title_imp<Lit>(inout, i3::as_literal(title) );
	}

	template<class Seq, class TitleRange> inline typename i3::disable_if_iterator<Seq>::type
	change_file_title(Seq& inout, const TitleRange& title)
	{
		detail::change_file_title_imp<Seq&>(inout, i3::as_literal(title) );
	}

	template<class Lit, class InputRange, class TitleRange > 
	inline typename i3::enable_if_iterator<Lit>::type
	change_file_title_copy(Lit out, const InputRange& input, const TitleRange& title)
	{
		detail::change_file_title_copy_imp<Lit>(out, i3::as_literal(input), i3::as_literal(title) );
	}

	template<class Seq, class InputRange, class TitleRange> 
	inline typename i3::disable_if_iterator<Seq>::type
	change_file_title_copy(Seq& out, const InputRange& input, const TitleRange& title)
	{
		detail::change_file_title_copy_imp<Seq&>(out, i3::as_literal(input), i3::as_literal(title) );
	}

}
