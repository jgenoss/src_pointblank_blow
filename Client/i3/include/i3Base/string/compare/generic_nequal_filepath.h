#pragma once

#include <cstddef>
#include "../../itl/range/algorithm/equal.h"
#include "../../itl/range/as_n_literal.h"
#include "is_equal_filechar.h"

namespace i3
{
	struct generic_nequal_filepath
	{
		generic_nequal_filepath(std::size_t n) : m_size(n) {}	std::size_t m_size;

		template<class Rng1, class Rng2> inline
		bool operator()(const Rng1& lhs, const Rng2& rhs) const
		{
			return i3::range::equal(i3::as_n_literal(lhs, m_size), i3::as_n_literal(rhs, m_size), 
				is_equal_filechar());
		}
	};



}
