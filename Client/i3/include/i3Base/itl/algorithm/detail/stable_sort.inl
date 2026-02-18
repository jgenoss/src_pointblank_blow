
#include "../../detail/temp_iterator.h"
#include "../copy.h"
#include "ISORT_MAX.h"
#include "../insertion_sort.h"
#include "../merge.h"
#include "buffered_merge.h"


namespace i3
{
	namespace detail
	{

		template<class bidirectional_iterator, class output_iterator, class diff> inline
		void chunked_merge(bidirectional_iterator first, bidirectional_iterator last, output_iterator dest, diff chunk, diff count)
		{
			for (diff chunk2 = chunk * 2; chunk2 <= count; count -= chunk2)
			{	// copy merging pairs of adjacent chunks
				bidirectional_iterator mid1 = first;
				i3::advance(mid1, chunk);
				bidirectional_iterator mid2 = mid1;
				i3::advance(mid2, chunk);

				dest = i3::merge(first, mid1, mid1, mid2, dest);
				first = mid2;
			}

			if (count <= chunk)
				i3::copy(first, last, dest);	// copy partial last chunk
			else
			{	// copy merging whole and partial last chunk
				bidirectional_iterator mid = first;
				i3::advance(mid, chunk);
				i3::merge(first, mid, mid, last, dest);
			}
		}

		template<class bidirectional_iterator, class output_iterator, class diff, class binary_pred> inline
		void chunked_merge(bidirectional_iterator first, bidirectional_iterator last, output_iterator dest, diff chunk, diff count, binary_pred pr)
		{
			for (diff chunk2 = chunk * 2; chunk2 <= count; count -= chunk2)
			{	// copy merging pairs of adjacent chunks
				bidirectional_iterator mid1 = first;
				i3::advance(mid1, chunk);
				bidirectional_iterator mid2 = mid1;
				i3::advance(mid2, chunk);

				dest = i3::merge(first, mid1, mid1, mid2, dest, pr);
				first = mid2;
			}

			if (count <= chunk)
				i3::copy(first, last, dest);	// copy partial last chunk
			else
			{	// copy merging whole and partial last chunk
				bidirectional_iterator mid = first;
				i3::advance(mid, chunk);
				i3::merge(first, mid, mid, last, dest, pr);
			}
		}

		template<class bidirectional_iterator, class diff, class T> inline
		void buffered_merge_sort( bidirectional_iterator first, bidirectional_iterator last, diff count, temp_pool_iterator<T>& temp_buff)
		{
			bidirectional_iterator mid = first;
			for (diff nleft = count; ISORT_MAX <= nleft; nleft -= ISORT_MAX)
			{	// sort chunks
				bidirectional_iterator midend = mid;
				i3::advance(midend, (int)ISORT_MAX);
				i3::insertion_sort(mid, midend);
				mid = midend;
			}
			i3::insertion_sort(mid, last);	// sort partial last chunk

			for (diff chunk = ISORT_MAX; chunk < count; chunk *= 2)
			{	// merge adjacent pairs of chunks to and from temp buffer
				chunked_merge(first, last, temp_buff.init(), chunk, count);
				chunked_merge(temp_buff.first(), temp_buff.last(), first, chunk*=2, count);
			}
		}

		template<class bidirectional_iterator, class diff, class T, class binary_pred> inline
		void buffered_merge_sort( bidirectional_iterator first, bidirectional_iterator last, diff count, temp_pool_iterator<T>& temp_buff, binary_pred pr)
		{
			bidirectional_iterator mid = first;
			for (diff nleft = count; ISORT_MAX <= nleft; nleft -= ISORT_MAX)
			{	// sort chunks
				bidirectional_iterator midend = mid;
				i3::advance(midend, (int)ISORT_MAX);
				i3::insertion_sort(mid, midend, pr);
				mid = midend;
			}
			i3::insertion_sort(mid, last, pr);	// sort partial last chunk

			for (diff chunk = ISORT_MAX; chunk < count; chunk *= 2)
			{	// merge adjacent pairs of chunks to and from temp buffer
				chunked_merge(first, last, temp_buff.init(), chunk, count, pr);
				chunked_merge(temp_buff.first(), temp_buff.last(), first, chunk*=2, count, pr);
			}
		}


		template<class bidirectional_iterator, class diff, class T> inline
		void stable_sort_impl(bidirectional_iterator first, bidirectional_iterator last, diff count, temp_pool_iterator<T>& temp_buff)
		{	//  sort preserving order of equivalents, using operator<
			if (count <= ISORT_MAX)
				i3::insertion_sort(first, last);	// small
			else
			{	// sort halves and merge
				diff count2 = (count + 1) / 2;
				bidirectional_iterator mid = first;
				i3::advance(mid, count2);

				if ( count2 <= temp_buff.max_length() )
				{	// temp buffer big enough, sort each half using buffer
					buffered_merge_sort(first, mid, count2, temp_buff);
					buffered_merge_sort(mid, last, count - count2, temp_buff);
				}
				else
				{	// temp buffer not big enough, divide and conquer
					stable_sort_impl(first, mid, count2, temp_buff);
					stable_sort_impl(mid, last, count - count2, temp_buff);
				}
				buffered_merge(first, mid, last, count2, count - count2, temp_buff);	// merge sorted halves
			}
		}

		template<class bidirectional_iterator, class diff, class T, class binary_pred> inline
		void stable_sort_impl(bidirectional_iterator first, bidirectional_iterator last, diff count, temp_pool_iterator<T>& temp_buff, binary_pred pr)
		{	//  sort preserving order of equivalents, using operator<
			if (count <= ISORT_MAX)
				i3::insertion_sort(first, last, pr);	// small
			else
			{	// sort halves and merge
				diff count2 = (count + 1) / 2;
				bidirectional_iterator mid = first;
				i3::advance(mid, count2);

				if ( count2 <= temp_buff.max_length() )
				{	// temp buffer big enough, sort each half using buffer
					buffered_merge_sort(first, mid, count2, temp_buff, pr);
					buffered_merge_sort(mid, last, count - count2, temp_buff, pr);
				}
				else
				{	// temp buffer not big enough, divide and conquer
					stable_sort_impl(first, mid, count2, temp_buff, pr);
					stable_sort_impl(mid, last, count - count2, temp_buff, pr);
				}
				buffered_merge(first, mid, last, count2, count - count2, temp_buff, pr);	// merge sorted halves
			}
		}



	}

	template<class bidirectional_iterator> inline
	void stable_sort( bidirectional_iterator first, bidirectional_iterator last)
	{
		if ( first == last ) return;
		typedef typename i3::iterator_traits<bidirectional_iterator>::difference_type	diff;
		typedef typename i3::iterator_traits<bidirectional_iterator>::value_type		T;
		diff count = 0;		i3::distance(first, last, count);
		temp_pool_iterator<T> temp_buff( (count + 1) / 2 );
		detail::stable_sort_impl(first, last, count, temp_buff);
	}

	template<class bidirectional_iterator, class binary_pred> inline
	void stable_sort( bidirectional_iterator first, bidirectional_iterator last, binary_pred pr)
	{
		if (first == last ) return;
		typedef typename i3::iterator_traits<bidirectional_iterator>::difference_type	diff;
		typedef typename i3::iterator_traits<bidirectional_iterator>::value_type		T;
		diff count = 0;		i3::distance(first, last, count);
		temp_pool_iterator<T> temp_buff( (count + 1) / 2 );
		detail::stable_sort_impl(first, last, count, temp_buff, pr);
	}

}
