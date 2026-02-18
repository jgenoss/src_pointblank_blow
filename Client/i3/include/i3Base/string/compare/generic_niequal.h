#pragma once

#include "../../itl/range/algorithm/equal.h"
#include "../../itl/range/as_literal.h"

namespace i3
{
	struct generic_niequal
	{
		generic_niequal(std::size_t n) : m_size(n) {}	std::size_t m_size;

		template<class Rng1, class Rng2> inline 
		bool operator()(const Rng1& lhs, const Rng2& rhs) const 
		{
			typedef typename i3::get_iterator_range_type<const Rng1>::type	range1_type;
			typedef typename i3::get_iterator_range_type<const Rng2>::type	range2_type;
			typedef typename range1_type::difference_type					diff_t;

			range1_type r1 = as_literal(lhs);
			range2_type r2 = as_literal(rhs);
			if (diff_t(m_size) < r1.size() ) r1.advance_end( -diff_t(r1.size() - m_size) );
			if (diff_t(m_size) < r2.size() ) r2.advance_end( -diff_t(r2.size() - m_size) );
			return i3::range::equal(r1, r2, is_iequal() );	
		}
	};



}