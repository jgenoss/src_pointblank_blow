#pragma once

#include "../undef_minmax_begin.h"

namespace i3
{
	
	template<class T> inline
	const T&  min(const T& Left, const T& Right) {	return ( Right < Left) ? Right : Left;	}

	template<class T, class Pred> inline
	const T&  min(const T& Left, const T& Right, Pred pr) {	return pr(Right, Left) ? Right : Left; }

	template<class T> inline
	const T& min(const T& arg1, const T& arg2, const T& arg3 ) { return i3::min( i3::min( arg1, arg2), arg3 );	}

	template<class T, class Pred> inline
	const T& min(const T& arg1, const T& arg2, const T& arg3, Pred pr) { return i3::min( i3::min(arg1, arg2, pr), arg3, pr); }
	
}

#include "../undef_minmax_end.h"