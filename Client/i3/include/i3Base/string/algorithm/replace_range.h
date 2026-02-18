#pragma once

#include "const_formatter.h"
#include "range_finder.h"
#include "find_format.h"

// all알고리즘이 무겁기 때문에 헤더 분리가 필요함..
// 일반 replace_range는 구간을 교체하는 것이지, 탐색기능이 있지는 않다..
// 헤더 이름을 교체할 필요가 있다.( replace_range.h)

namespace i3
{
	template<class OutIt, class SrcRange, class ReplaceRange> inline
		OutIt replace_range_copy(OutIt out, const SrcRange& in, 
		typename range_const_iterator<SrcRange>::type from, 
		typename range_const_iterator<SrcRange>::type to, 
		, const ReplaceRange& rep )
	{
		return find_format_copy(out, in, range_finder(from, to), const_formatter(rep));
	}
	
	template<class OutIt, class SrcRange, class ReplaceRange> inline
		OutIt replace_range_copy(OutIt out, const SrcRange& in, 
		const typename get_const_iterator_range_type<SrcRange>::type& in_rng, const ReplaceRange& rep)
	{
		return find_format_copy(out, in, range_finder(in_rng), const_formatter(rep));
	}

	template< class Seq, class ReplaceRange> inline
		Seq replace_range_copy( const Seq& in, 
		typename range_const_iterator<Seq>::type from, 
		typename range_const_iterator<Seq>::type to, 
		, const ReplaceRange& rep )
	{
		return find_format_copy(in, range_finder(from, to), const_formatter(rep));
	}

	template< class Seq, class ReplaceRange> inline
		Seq replace_range_copy( const Seq& in, 
		const typename get_const_iterator_range_type<SrcRange>::type& in_rng, const ReplaceRange& rep)
	{
		return find_format_copy(in, range_finder(in_rng), const_formatter(rep));
	}


	template< class Seq, class ReplaceRange> inline
		void replace_range( Seq& inout, 
		typename range_const_iterator<Seq>::type from, 
		typename range_const_iterator<Seq>::type to, 
		const ReplaceRange& rep )
	{
		find_format(inout, range_finder(from, to), const_formatter(rep));
	}

	template< class Seq, class ReplaceRange> inline
		void replace_range( Seq& inout, 
		const typename get_const_iterator_range_type<SrcRange>::type& in_rng,
		const ReplaceRange& rep )
	{
		find_format(inout, range_finder(in_rng), const_formatter(rep));
	}


}