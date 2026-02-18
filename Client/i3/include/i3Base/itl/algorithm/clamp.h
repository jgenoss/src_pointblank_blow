#pragma once

namespace i3
{
		
	template<class T> inline
	const T& clamp(const T& value, const T& range_min, const T& range_max) { return (value < range_min) ? range_min : ( ( range_max < value ) ? range_max : value );  }

	template<class T, class Pred> inline
	const T& clamp(const T& value, const T& range_min, const T& range_max, Pred pr) { return (pr(value, range_min)) ? range_min : (pr(range_max, value) ? range_max : value); }

}
