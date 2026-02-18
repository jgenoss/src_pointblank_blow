#pragma once

#include "../../detail/temp_iterator.h"
#include "../copy.h"
#include "../copy_backward.h"
#include "../../iterator.h"
#include "../rotate.h"

namespace i3
{
	namespace detail
	{
		
		template<class bidirectional_iterator, class diff,	class T> inline
		bidirectional_iterator buffered_rotate(bidirectional_iterator first, bidirectional_iterator mid, bidirectional_iterator last, 
			diff count1, diff count2, temp_pool_iterator<T>& temp_buff)
		{	// rotate [_First, _Last) using temp buffer
			if (count1 <= count2 && count1 <= temp_buff.max_length() )
			{	// buffer left partition, then copy parts
				i3::copy(first, mid, temp_buff.init());
				i3::copy(mid, last, first);
				return i3::copy_backward(temp_buff.first(), temp_buff.last(), last);
			}
			else if ( count2 <= temp_buff.max_length() )
			{	// buffer right partition, then copy parts
				i3::copy(mid, last, temp_buff.init());
				i3::copy_backward(first, mid, last);
				return i3::copy(temp_buff.first(), temp_buff.last(), first);
			}
			else
			{	// buffer too small, rotate in place
				i3::rotate(first, mid, last);
				i3::advance(first, count2);
				return first;
			}
		}
		
	}
}
