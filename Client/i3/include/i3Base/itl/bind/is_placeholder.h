#pragma once

namespace i3
{
	template<class T>
	struct is_placeholder : integral_constant<int, 0> {};
}

