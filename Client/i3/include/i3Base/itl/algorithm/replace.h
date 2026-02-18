#pragma once

namespace i3
{
	template<class FwdIt, class T> inline
	void replace(FwdIt f, FwdIt l, const T& old_val, const T& new_val);
}

#include "detail/replace.inl"
