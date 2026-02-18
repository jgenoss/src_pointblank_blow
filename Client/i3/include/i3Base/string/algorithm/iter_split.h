#pragma once

#include "split_iterator.h"

#include "../../itl/range/as_literal.h"
#include "../../itl/range/copy_range.h"
#include "../../itl/range/value_type.h"
#include "../../itl/iterator/transform_iterator.h"
#include "../../itl/utility/copy_or_swap.h"

namespace i3
{
	template<class SeqSeq, class Range, class Finder> inline
	SeqSeq& iter_split( SeqSeq& Result, const Range& Input, Finder Fnd)
	{
		typedef typename range_iterator<Range>::type		input_iterator_type;

		iterator_range<input_iterator_type> lit_input( as_literal(Input) );

		typedef split_iterator<input_iterator_type, Finder>				find_iterator_type;
		typedef copy_range_functor< typename range_value<SeqSeq>::type> copy_range_functor_type;
		
		input_iterator_type InputBegin = i3::begin(lit_input);
		input_iterator_type InputEnd = i3::end(lit_input);
		
		typedef transform_iterator<copy_range_functor_type, find_iterator_type> transform_iterator_type;

		transform_iterator_type ResultBegin = i3::make_transform_iterator<copy_range_functor_type>
												( find_iterator_type( InputBegin, InputEnd, Fnd) );
		transform_iterator_type ResultEnd   = i3::make_transform_iterator<copy_range_functor_type>
												( find_iterator_type( InputEnd, Fnd) );

		SeqSeq tmp(ResultBegin, ResultEnd);
		i3::copy_or_swap( Result, tmp);
		return Result;
	}

	
}