#pragma once

#include "../../itl/range/as_literal.h"
#include "../../itl/range/generic_replace_range.h"
#include "../../itl/range/rbegin.h"
#include "../../itl/range/rend.h"
#include "../../itl/range/algorithm/find.h"

#include "generic_string_copy.h"
#include "generic_string_cat.h"

namespace i3
{
	// ext의 .은 조정이 필요함..
	namespace detail
	{
		template<class Lit, class Rng> 
		void change_file_ext_if_lit(Lit name, const Rng& ext)
		{
			i3::iterator_range<Lit> name_range = i3::as_literal(name);

			typedef i3::reverse_iterator<Lit> rev_iterator;
			i3::iterator_range< rev_iterator > rev_rng = i3::make_iterator_range( i3::rbegin(name_range), i3::rend(name_range) );
			rev_iterator rev_it = i3::range::find(rev_rng, '.');

			if ( rev_it == rev_rng.end() )
				return;

			Lit it = rev_it.base();		// '.' 다음..
		
			typedef typename i3::range_iterator<Rng>::type ext_iterator;
			i3::iterator_range< ext_iterator> ext_range = ext;
			
			if (!ext_range.empty() && *ext_range.begin() == '.' )
			{
				ext_range.advance_begin(1);
			}
			
			i3::generic_replace_range(name, it, name_range.end(), 
				ext_range.begin(), ext_range.end() );				
		}
		
		template<class Seq, class Rng> 
		void change_file_ext_if_seq(Seq& name, const Rng& ext)
		{
			typedef typename i3::range_reverse_iterator<Seq>::type rev_name_iterator;
			typedef typename i3::range_iterator<Seq>::type		  name_iterator;

			rev_name_iterator rev_it = i3::range::find(i3::make_iterator_range( rbegin(name), rend(name) ), '.');

			if (rev_it == rend(name))	// .이 없으면 그냥 종료..
				return;
			
			name_iterator it = rev_it.base();		// . 다음..

			typedef typename i3::range_iterator<Rng>::type ext_iterator;
			i3::iterator_range< ext_iterator> ext_range = ext;
			if (!ext_range.empty() && *ext_range.begin() == '.' )
			{
				ext_range.advance_begin(1);
			}

			i3::generic_replace_range(name, it, end(name), 
				ext_range.begin(), ext_range.end() );	
		}
	}

	template<class Lit, class Rng> inline typename enable_if_iterator<Lit>::type 
	change_file_ext(Lit name, const Rng& ext)
	{
		detail::change_file_ext_if_lit(name, i3::as_literal(ext));
	}

	template<class Seq, class Rng> inline typename disable_if_iterator<Seq>::type 
	change_file_ext(Seq& name, const Rng& ext)
	{
		detail::change_file_ext_if_seq(name, i3::as_literal(ext));
	}

/////////////////////////////////////////////////////////

	namespace detail
	{

		template<class Lit, class InRng, class ExtRng>
		void change_file_ext_copy_if_lit(Lit out, const InRng& src, const ExtRng& ext)
		{
			typedef typename i3::range_iterator<const InRng>::type		src_iterator;

			typedef i3::reverse_iterator<src_iterator> rev_iterator;
			i3::iterator_range< rev_iterator > rev_rng = i3::make_iterator_range( i3::rbegin(src), i3::rend(src) );
			rev_iterator rev_it = i3::range::find(rev_rng, '.');

			if ( rev_it == rev_rng.end() )
			{
				i3::generic_string_copy(out, src );
				return;
			}

			src_iterator it = rev_it.base();		// '.' 다음..
		
			typedef typename i3::range_iterator<ExtRng>::type ext_iterator;
			i3::iterator_range< ext_iterator> ext_range = ext;
			
			if (!ext_range.empty() && *ext_range.begin() == '.' )
			{
				ext_range.advance_begin(1);
			}
			
			size_t num_end = i3::generic_string_copy(out, i3::begin(src), it);
			i3::generic_string_copy(out + num_end, ext_range.begin(), ext_range.end());
				
		}
	
		template<class Seq, class InRng, class ExtRng> 
		void change_file_ext_copy_if_seq(Seq& out, const InRng& src, const ExtRng& ext)
		{
			typedef typename i3::range_iterator<const InRng>::type		src_iterator;

			typedef i3::reverse_iterator<src_iterator> rev_iterator;
			i3::iterator_range< rev_iterator > rev_rng = i3::make_iterator_range( i3::rbegin(src), i3::rend(src) );
			rev_iterator rev_it = i3::range::find(rev_rng, '.');

			if (rev_it == rev_rng.end() )	// .이 없으면 복사 종료..
			{
				i3::generic_string_copy(out, src );
				return;
			}
			
			src_iterator it = rev_it.base();		// . 다음..

			typedef typename i3::range_iterator<ExtRng>::type ext_iterator;
			i3::iterator_range< ext_iterator> ext_range = ext;
			if (!ext_range.empty() && *ext_range.begin() == '.' )
			{
				ext_range.advance_begin(1);
			}

			i3::generic_string_copy(out, i3::begin(src), it);
			i3::generic_string_cat(out, ext_range);
		}
	}

	template<class Lit, class InRng, class ExtRng> inline typename enable_if_iterator<Lit>::type 
	change_file_ext_copy(Lit out, const InRng& src, const ExtRng& ext)
	{
		detail::change_file_ext_copy_if_lit(out, i3::as_literal(src), i3::as_literal(ext));
	}

	template<class Seq, class InRng, class ExtRng> inline typename disable_if_iterator<Seq>::type 
	change_file_ext_copy(Seq& out, const InRng& src, const ExtRng& ext)
	{
		detail::change_file_ext_copy_if_seq(out, i3::as_literal(src), i3::as_literal(ext));
	}

}
