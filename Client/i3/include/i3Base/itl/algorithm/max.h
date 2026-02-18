#pragma once

#include "../undef_minmax_begin.h"

namespace i3
{

	template<class T> inline 
	const T&  max(const T& Left, const T& Right) {	return (Left < Right) ? Right : Left;	}

	template<class T, class Pred> inline
	const T&  max(const T& Left, const T& Right, Pred pr) {	return pr(Left, Right) ? Right : Left; }

	template<class T> inline
	const T& max( const T& arg1, const T& arg2, const T& arg3 ) { return i3::max( i3::max( arg1, arg2 ), arg3 );   }

	template<class T, class Pred> inline
	const T& max( const T& arg1, const T& arg2, const T& arg3, Pred pr) { return i3::max( i3::max(arg1, arg2, pr), arg3, pr); }

}

#include "../undef_minmax_end.h"
