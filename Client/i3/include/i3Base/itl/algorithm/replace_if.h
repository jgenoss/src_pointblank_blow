#pragma once

namespace i3
{

	template<class FwdIt, class T, class Pred1> inline
	void replace_if(FwdIt f, FwdIt l, Pred1 pr, const T& new_val);

}

#include "detail/replace_if.inl"
