#pragma once

//
//  const반복자 구간만 따로 떼낸다는 점을 빼고, 별도로 바꾸는게 없는 함수객체..
// 
#include "../../itl/range/as_literal.h"

namespace i3
{
	template<class Range>
	struct const_formatF
	{
		typedef typename range_const_iterator<Range>::type	format_iterator;
		typedef iterator_range<format_iterator>				result_type;

		const_formatF(const Range& format) : m_format(begin(format), end(format) ) {} 
		
		template<class Range2>
		const result_type& operator()(const Range2&) const { return m_format; }
	private:
		result_type		m_format;
	};
	
	template<class Range> inline 
	const_formatF< iterator_range< typename range_const_iterator<Range>::type > >
	const_formatter(const Range& format)
	{
		return const_formatF< iterator_range< typename range_const_iterator<Range>::type > >(as_literal(format));
	}
	
}