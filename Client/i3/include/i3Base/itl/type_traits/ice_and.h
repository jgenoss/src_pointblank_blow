#pragma once

namespace i3
{
	template <bool b1, bool b2, bool b3 = true, bool b4 = true, bool b5 = true, bool b6 = true, bool b7 = true>	
	struct ice_and;
	template <bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7> struct ice_and	{	static const bool value = false;	};
	template <>	struct ice_and<true, true, true, true, true, true, true> {	static const bool value = true;	};
}
