#pragma once

namespace i3
{
	template<class RanIt> inline
	void random_shuffle(RanIt f, RanIt l);
	template<class RanIt, class Fn1> inline
	void random_shuffle(RanIt f, RanIt l, Fn1& fn);	
}

#include "detail/random_shuffle.inl"
