#pragma once

namespace i3
{
	template <class InIt, class OutIt> inline
	OutIt copy(InIt f, InIt l, OutIt dest);

	template <class InIt, class OutIt> inline
	OutIt copy_1(InIt f, OutIt dest);

}

#include "detail/copy.inl"