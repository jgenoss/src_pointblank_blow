#pragma once

#include "../../detail/temp_iterator.h"
#include "../copy.h"
#include "../../iter_swap.h"
#include "../merge.h"
#include "../merge_backward.h"
#include "buffered_rotate.h"
#include "../lower_bound.h"
#include "../upper_bound.h"


namespace i3
{
	namespace detail
	{
		
		template<class bidirectional_iterator, class diff, class T> inline
		void buffered_merge(bidirectional_iterator first, bidirectional_iterator mid, bidirectional_iterator last, 
			diff count1, diff count2, temp_pool_iterator<T>& temp_buff)
		{	// merge [_First, _Mid) with [_Mid, _Last), using operator<
			if (count1 + count2 == 2)
			{	// order two one-element partitions
				if (*mid < *first) i3::iter_swap(first, mid);
			}
			else if (count1 <= count2 && count1 <= temp_buff.max_length() )
			{	// buffer left partition, then merge
				i3::copy(first, mid, temp_buff.init());
				i3::merge(temp_buff.first(), temp_buff.last(), mid, last, first);
			}
			else if (count2 <= temp_buff.max_length())
			{	// buffer right partition, then merge
				i3::copy(mid, last, temp_buff.init());
				i3::merge_backward(first, mid, temp_buff.first(), temp_buff.last(), last);
			}
			else
			{	// buffer too small, divide and conquer
				bidirectional_iterator	firstn, lastn;
				diff					count1n, count2n;

				if (count2 < count1)
				{	// left larger, cut it in half and partition right to match
					count1n = count1 / 2, count2n = 0;
					firstn = first;
					i3::advance(firstn, count1n);
					lastn = i3::lower_bound(mid, last, *firstn);
					i3::distance(mid, lastn, count2n);
				}
				else
				{	// right larger, cut it in half and partition left to match
					count1n = 0, count2n = count2 / 2;
					lastn = mid;
					i3::advance(lastn, count2n);
					firstn = i3::upper_bound(first, mid, *lastn);
					i3::distance(first, firstn, count1n);
				}

				bidirectional_iterator midn = buffered_rotate(firstn, mid, lastn, count1 - count1n, count2n, temp_buff);	// rearrange middle
				buffered_merge(first, firstn, midn,	count1n, count2n, temp_buff);	// merge each new part
				buffered_merge(midn, lastn, last, count1 - count1n, count2 - count2n, temp_buff);
			}
		}

		template<class bidirectional_iterator, class diff, class T, class binary_pred> inline
		void buffered_merge(bidirectional_iterator first, bidirectional_iterator mid, bidirectional_iterator last, 
			diff count1, diff count2, temp_pool_iterator<T>& temp_buff, binary_pred pr)
		{	// merge [_First, _Mid) with [_Mid, _Last), using operator<
			if (count1 + count2 == 2)
			{	// order two one-element partitions
				if ( pr(*mid, *first) ) i3::iter_swap(first, mid);
			}
			else if (count1 <= count2 && count1 <= temp_buff.max_length() )
			{	// buffer left partition, then merge
				i3::copy(first, mid, temp_buff.init());
				i3::merge(temp_buff.first(), temp_buff.last(), mid, last, first, pr);
			}
			else if (count2 <= temp_buff.max_length())
			{	// buffer right partition, then merge
				i3::copy(mid, last, temp_buff.init());
				i3::merge_backward(first, mid, temp_buff.first(), temp_buff.last(), last, pr);
			}
			else
			{	// buffer too small, divide and conquer
				bidirectional_iterator	firstn, lastn;
				diff					count1n, count2n;

				if (count2 < count1)
				{	// left larger, cut it in half and partition right to match
					count1n = count1 / 2, count2n = 0;
					firstn = first;
					i3::advance(firstn, count1n);
					lastn = i3::lower_bound(mid, last, *firstn, pr);
					i3::distance(mid, lastn, count2n);
				}
				else
				{	// right larger, cut it in half and partition left to match
					count1n = 0, count2n = count2 / 2;
					lastn = mid;
					i3::advance(lastn, count2n);
					firstn = i3::upper_bound(first, mid, *lastn, pr);
					i3::distance(first, firstn, count1n);
				}

				bidirectional_iterator midn = buffered_rotate(firstn, mid, lastn, count1 - count1n, count2n, temp_buff);	// rearrange middle
				buffered_merge(first, firstn, midn,	count1n, count2n, temp_buff, pr);	// merge each new part
				buffered_merge(midn, lastn, last, count1 - count1n, count2 - count2n, temp_buff, pr);
			}
		}


	}
		
}