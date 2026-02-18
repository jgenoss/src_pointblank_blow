#pragma once

#include "../type_traits/is_stateless.h"

namespace i3
{
	
	struct blank {};		//

	template <> struct is_pod< blank > : mpl::true_ {};
	template <> struct is_empty< blank > : mpl::true_{};
	template <> struct is_stateless< blank > : mpl::true_{};

	inline bool operator==(const blank&, const blank&) { return true; }
	inline bool operator<=(const blank&, const blank&) { return true; }
	inline bool operator>=(const blank&, const blank&) { return true; }
	inline bool operator!=(const blank&, const blank&) { return false; }
	inline bool operator<(const blank&, const blank&) {	return false; }
	inline bool operator>(const blank&, const blank&) {	return false; }

}

