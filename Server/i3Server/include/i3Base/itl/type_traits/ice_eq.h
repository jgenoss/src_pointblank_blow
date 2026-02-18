#pragma once

namespace i3
{
	template <int b1, int b2> struct ice_eq {	static const bool value = (b1 == b2);	};
	template <int b1, int b2> struct ice_ne	{	static const bool value = (b1 != b2);	};
}
