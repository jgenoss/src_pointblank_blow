#pragma once

namespace i3
{
	template <bool b1, bool b2, bool b3 = false, bool b4 = false, bool b5 = false, bool b6 = false, bool b7 = false> 
	struct ice_or;
	template <bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7> struct ice_or { static const bool value = true; };
	template <>	struct ice_or<false, false, false, false, false, false, false> {	static const bool value = false; };
}
