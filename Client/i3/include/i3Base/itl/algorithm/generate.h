#pragma once

namespace i3
{
	template<class FwdIt, class Fn0> inline
	void generate(FwdIt f, FwdIt l, Fn0 fn);

}

#include "detail/generate.inl"
