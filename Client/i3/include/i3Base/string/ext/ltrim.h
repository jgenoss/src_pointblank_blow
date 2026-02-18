#pragma once

// PB소스의 StringUtil::LTrim을 일반화한다.

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/iterator/iterator_traits.h"
#include "../../itl/range/value_type.h"
#include "../../itl/range/generic_erase_begin_iter.h"
#include "../../itl/range/generic_clear.h"

#include "generic_find_first_not_of.h"
#include "generic_string_copy.h"
#include "safe_string_copy.h"
#include "make_generic_literal.h"


namespace i3
{
	template<class Literal, class DelimitRng> inline typename enable_if_iterator<Literal,
	void>::type ltrim(Literal inout, const DelimitRng& delimit)
	{
		i3::iterator_range<Literal> rng(i3::as_literal(inout));
		if (rng.empty())
			return;

		Literal it = i3::generic_find_first_not_of(rng, delimit);
		
		if (it == rng.begin() )
			return;

		if ( it != rng.end() )
			i3::generic_erase_begin_iter(inout, it);
		else
			i3::generic_clear(inout);
	}
	
	template<class Seq, class DelimitRng> inline typename disable_if_iterator<Seq,
	void>::type ltrim(Seq& inout, const DelimitRng& delimit)
	{
		typedef typename i3::range_iterator<Seq>::type	iterator;
		typedef i3::iterator_range<iterator>			RangeType;

		RangeType rng(i3::as_literal(inout) );
		if (rng.empty())
			return;

		iterator it = i3::generic_find_first_not_of(rng, delimit);

		if (it == rng.begin() )
			return;

		if (it != rng.end() )
			i3::generic_erase_begin_iter(inout, it);
		else
			i3::generic_clear(inout);
	}

	template<class OutRange, class InputRange, class DelimitRng> inline typename disable_if_iterator<OutRange,
	void>::type ltrim_copy(OutRange& out, const InputRange& input, const DelimitRng& delimit)
	{
		typedef typename i3::range_iterator<const InputRange>::type  input_iterator;
		typedef i3::iterator_range<input_iterator>					 InputItRng;

		InputItRng input_it_rng(i3::as_literal(input) );
		if ( input_it_rng.empty() )
			i3::generic_clear(out);

		input_iterator it = i3::generic_find_first_not_of(input_it_rng, delimit);
		
		if ( it == input_it_rng.begin())
			i3::generic_string_copy(out, input);
		else
		if ( it != input_it_rng.end() )
			i3::generic_string_copy(out, it, input_it_rng.end() );
		else
			i3::generic_clear(out);
	}

	template<class OutLit, class InputRange, class DelimitRng> inline typename enable_if_iterator<OutLit,
	void>::type ltrim_copy(OutLit out, size_t buff_size, const InputRange& input, const DelimitRng& delimit)
	{
		typedef typename i3::range_iterator<const InputRange>::type  input_iterator;
		typedef i3::iterator_range<input_iterator>					 InputItRng;

		InputItRng input_it_rng(i3::as_literal(input) );
		if ( input_it_rng.empty() )
			i3::generic_clear(out);

		input_iterator it = i3::generic_find_first_not_of(input_it_rng, delimit);
		
		if (it == input_it_rng.begin())
			i3::safe_string_copy(out, input, buff_size);
		else
		if ( it != input_it_rng.end() )
			i3::safe_string_copy(out, it, input_it_rng.end(), buff_size);
		else
			i3::generic_clear(out);
	}

	template<class Literal> inline typename enable_if_iterator<Literal,
	void>::type ltrim(Literal inout)
	{
		ltrim(inout, i3::make_generic_literal( typename i3::iterator_value<Literal>::type, " \t") );
	}

	template<class Seq> inline typename disable_if_iterator<Seq,
	void>::type ltrim(Seq& inout)
	{
		ltrim(inout, i3::make_generic_literal( typename i3::range_value<Seq>::type, " \t") );
	}

	template<class OutRange, class InputRange> inline typename disable_if_iterator<OutRange,
	void>::type ltrim_copy(OutRange& out, const InputRange& input)
	{
		ltrim_copy(out, input, i3::make_generic_literal( typename i3::range_value<InputRange>::type, " \t") );
	}

	template<class OutLit, class InputRange> inline typename enable_if_iterator<OutLit,
	void>::type ltrim_copy(OutLit out, size_t buff_size, const InputRange& input)
	{
		ltrim_copy(out, buff_size, input, i3::make_generic_literal( typename i3::range_value<InputRange>::type, " \t") );
	}


}
