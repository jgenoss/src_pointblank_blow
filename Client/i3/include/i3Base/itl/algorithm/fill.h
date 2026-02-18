#pragma once

namespace i3
{
	template<class FwdIt, class T> inline
	void	fill(FwdIt f, FwdIt l, const T& val);
}

#include "detail/fill.inl"
