#pragma once


namespace i3
{

	template<class FwdIt, class T> inline
	FwdIt	remove(FwdIt f, FwdIt l, const T& val);
}

#include "detail/remove.inl"

