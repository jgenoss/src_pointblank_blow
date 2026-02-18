#pragma once

namespace i3
{

	template<class forward_iterator> inline
	void rotate(forward_iterator first, forward_iterator mid, forward_iterator last);

}


#include "detail/rotate.inl"
