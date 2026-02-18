#pragma once

namespace i3
{
	template <bool b> struct ice_not { static const bool value = true; };
	template <> struct ice_not<true> { static const bool value = false; };
}
