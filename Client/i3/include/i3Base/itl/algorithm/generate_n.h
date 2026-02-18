#pragma once

namespace i3
{
	template<class OutIt, class Diff, class Fn0> inline
	void	generate_n(OutIt dest, Diff count, Fn0 fn);
}

#include "detail/generate_n.inl"
