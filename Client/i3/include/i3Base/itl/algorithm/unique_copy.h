#pragma once


namespace i3
{

	template<class  input_iterator, class output_iterator> inline
	output_iterator	unique_copy(input_iterator first, input_iterator last, output_iterator dest);

	template<class  input_iterator, class output_iterator, class binary_pred> inline
	output_iterator	unique_copy(input_iterator first, input_iterator last, output_iterator dest, binary_pred pr);

}

#include "detail/unique_copy.inl"
