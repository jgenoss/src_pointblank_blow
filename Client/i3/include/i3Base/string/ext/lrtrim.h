#pragma once

// PB소스의 StringUtil::Trim을 일반화한다.

#include "../../itl/range/detail/enable_if_iterator.h"
#include "../../itl/iterator/iterator_traits.h"
#include "../../itl/range/value_type.h"
#include "../../itl/range/generic_erase_begin_iter.h"
#include "../../itl/range/generic_erase_iter_end.h"
#include "../../itl/range/generic_clear.h"

#include "generic_find_first_not_of.h"
#include "generic_find_last_not_of.h"
#include "generic_string_copy.h"
#include "safe_string_copy.h"
#include "make_generic_literal.h"

namespace i3
{
	template<class Literal, class DelimitRng> inline typename enable_if_iterator<Literal,
	void>::type lrtrim(Literal inout, const DelimitRng& delimit)
	{
		i3::iterator_range<Literal> rng(i3::as_literal(inout));
		typedef typename i3::get_iterator_range_type<const DelimitRng>::type DelimitLitRng;
		
		if (rng.empty())
			return;

		DelimitLitRng delimit_rng(i3::as_literal(delimit));

		Literal left_it = i3::generic_find_first_not_of(rng, delimit_rng);
		Literal right_it = i3::generic_find_last_not_of(rng, delimit_rng);
		Literal it_end_m1 = rng.end();	--it_end_m1;


		if (left_it == rng.begin() && right_it == it_end_m1 )
			return;
		
		if ( left_it == rng.end() )
			i3::generic_clear(inout);
		else
		if ( left_it == rng.begin() )
			i3::generic_erase_iter_end(inout, ++right_it);
		else
		if ( right_it == it_end_m1 )
			i3::generic_erase_begin_iter(inout, left_it);
		else
			i3::generic_string_copy(inout, left_it, ++right_it);
	}

	template<class Seq, class DelimitRng> inline typename disable_if_iterator<Seq,
	void>::type lrtrim(Seq& inout, const DelimitRng& delimit)
	{
		typedef typename i3::range_iterator<Seq>::type	iterator;
		typedef i3::iterator_range<iterator>			RangeType;
		RangeType rng(i3::as_literal(inout) );
		if (rng.empty())
			return;

		typedef typename i3::get_iterator_range_type<const DelimitRng>::type DelimitLitRng;
		DelimitLitRng delimit_rng(i3::as_literal(delimit));

		iterator left_it = i3::generic_find_first_not_of(rng, delimit_rng);
		iterator right_it = i3::generic_find_last_not_of(rng, delimit_rng);
		iterator it_end_m1 = rng.end();	--it_end_m1;

		if (left_it == rng.begin() && right_it == it_end_m1 )
			return;

		if ( left_it == rng.end() )
			i3::generic_clear(inout);
		else
		if ( left_it == rng.begin() )
			i3::generic_erase_iter_end(inout, ++right_it);
		else
		if ( right_it == it_end_m1 )
			i3::generic_erase_begin_iter(inout, left_it);
		else
			i3::generic_string_copy(inout, left_it, ++right_it);

	}

	template<class OutRange, class InputRange, class DelimitRng> inline typename disable_if_iterator<OutRange,
	void>::type lrtrim_copy(OutRange& out, const InputRange& input, const DelimitRng& delimit)
	{
		typedef typename i3::range_iterator<const InputRange>::type  input_iterator;
		typedef i3::iterator_range<input_iterator>					 InputItRng;

		InputItRng input_it_rng(i3::as_literal(input) );
		if (input_it_rng.empty())
			i3::generic_clear(out);

		typedef typename i3::get_iterator_range_type<const DelimitRng>::type DelimitLitRng;
		DelimitLitRng delimit_rng(i3::as_literal(delimit));

		input_iterator left_it = i3::generic_find_first_not_of(input_it_rng, delimit_rng);
		input_iterator right_it = i3::generic_find_last_not_of(input_it_rng, delimit_rng);

		input_iterator it_end_m1 = input_it_rng.end();	--it_end_m1;


		if (left_it == input_it_rng.begin() && right_it == it_end_m1 )
			i3::generic_string_copy(out, input);
		else
		if ( left_it == input_it_rng.end() )
			i3::generic_clear(out);
		else
			i3::generic_string_copy(out, left_it, ++right_it);

	}

	template<class OutLit, class InputRange, class DelimitRng> inline typename enable_if_iterator<OutLit,
	void>::type lrtrim_copy(OutLit out, size_t buff_size, const InputRange& input, const DelimitRng& delimit)
	{
		typedef typename i3::range_iterator<const InputRange>::type  input_iterator;
		typedef i3::iterator_range<input_iterator>					 InputItRng;

		InputItRng input_it_rng(i3::as_literal(input) );
		if (input_it_rng.empty())
			i3::generic_clear(out);

		typedef typename i3::get_iterator_range_type<const DelimitRng>::type DelimitLitRng;
		DelimitLitRng delimit_rng(i3::as_literal(delimit));

		input_iterator left_it = i3::generic_find_first_not_of(input_it_rng, delimit_rng);
		input_iterator right_it = i3::generic_find_last_not_of(input_it_rng, delimit_rng);

		input_iterator it_end_m1 = input_it_rng.end();	--it_end_m1;


		if (left_it == input_it_rng.begin() && right_it == it_end_m1 )
			i3::safe_string_copy(out, input, buff_size);
		else
		if ( left_it == input_it_rng.end() )
			i3::generic_clear(out);
		else
			i3::safe_string_copy(out, left_it, ++right_it, buff_size);
	}


	template<class Literal> inline typename enable_if_iterator<Literal,
	void>::type lrtrim(Literal inout)
	{
		lrtrim(inout, i3::make_generic_literal( typename i3::iterator_value<Literal>::type, " \t") );
	}

	template<class Seq> inline typename disable_if_iterator<Seq,
	void>::type lrtrim(Seq& inout)
	{
		lrtrim(inout, i3::make_generic_literal( typename i3::range_value<Seq>::type, " \t") );
	}

	template<class OutRange, class InputRange> inline typename disable_if_iterator<OutRange,
	void>::type lrtrim_copy(OutRange& out, const InputRange& input)
	{
		lrtrim_copy(out, input, i3::make_generic_literal( typename i3::range_value<InputRange>::type, " \t") );
	}
	
	template<class OutLit, class InputRange> inline typename enable_if_iterator<OutLit,
	void>::type lrtrim_copy(OutLit out, size_t buff_size, const InputRange& input)
	{
		lrtrim_copy(out, buff_size, input, i3::make_generic_literal( typename i3::range_value<InputRange>::type, " \t") );
	}
	
}