#pragma once


namespace i3
{

	template<class forward_iterator, class out_iterator> inline
	out_iterator rotate_copy(forward_iterator first, forward_iterator mid, forward_iterator last, out_iterator dest);

}

#include "detail/rotate_copy.inl"
