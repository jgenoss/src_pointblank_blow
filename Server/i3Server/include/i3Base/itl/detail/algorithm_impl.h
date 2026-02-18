#pragma once

#include "temp_iterator.h"

namespace i3 
{
	
	namespace detail
	{
		// TEMPLATE FUNCTION find
		template<class InputIterator, class T> inline 	
			InputIterator Find(InputIterator First, InputIterator Last, const T& Val)
		{	// find first matching val
			for (; First != Last; ++First)
				if (*First == Val)
					break;
			return First;
		}

		inline const char * Find(const char *First, const char* Last, int Val)
		{	// find first char that matches val
			First = (const char *)::memchr(First, Val, Last - First);
			return (First == 0 ? Last : First);
		}

		inline const signed char * Find(const signed char* First, const signed char *Last, int Val)
		{	// find first signed char that matches val
			First = (const signed char *)::memchr(First, Val, Last - First);
			return (First == 0 ? Last : First);
		}

		inline const unsigned char * Find(const unsigned char *First, const unsigned char *Last, int Val)
		{	// find first unsigned char that matches val
			First = (const unsigned char *)::memchr(First, Val, Last - First);
			return (First == 0 ? Last : First);
		}
		
		template<class RanIt, class Diff, class T> inline
		void Push_heap(RanIt First, Diff Hole, Diff Top, T Val)
		{	// percolate _Hole to _Top or where val belongs, using operator<
			for (Diff Idx = (Hole - 1) / 2; Top < Hole && *(First + Idx) < Val ; Idx = (Hole - 1) / 2)
			{	// move _Hole up to parent
				*(First + Hole) = *(First + Idx);
				Hole = Idx;
			}
			*(First + Hole) = Val;	// drop val into final hole
		}
		
		template<class RanIt, class Diff, class T, class Pred> inline
		void Push_heap(RanIt First, Diff Hole, Diff Top, T Val, Pred pred)
		{	// percolate _Hole to _Top or where val belongs, using operator<
			for (Diff Idx = (Hole - 1) / 2; Top < Hole &&  pred( *(First + Idx) , Val) ; Idx = (Hole - 1) / 2)
			{	// move _Hole up to parent
				*(First + Hole) = *(First + Idx);
				Hole = Idx;
			}
			*(First + Hole) = Val;	// drop val into final hole
		}
		
		
		template<class RanIt> inline
		void Push_heap_0(RanIt First, RanIt Last )
		{	// push *_Last onto heap at [_First, _Last), using operator<
			typedef typename i3::iterator_traits<RanIt>::difference_type Diff;
			Diff Count = Last - First;
			if ( Count > 0 )		
			{
				typedef typename i3::iterator_traits<RanIt>::value_type value_type;
				Push_heap(First, Count, 0, value_type(*Last));
			}
		}

		template<class RanIt, class Pred> inline
		void Push_heap_0(RanIt First, RanIt Last, Pred pred )
		{	// push *_Last onto heap at [_First, _Last), using _Pred
			typedef typename i3::iterator_traits<RanIt>::difference_type Diff;
			Diff Count = Last - First;
			if ( Count > 0 )		
			{
				typedef typename i3::iterator_traits<RanIt>::value_type value_type;
				Push_heap(First, Count, 0, value_type(*Last), pred);			
			}
		}

		template<class RanIt, class Diff,	class T> inline
		void Adjust_heap(RanIt First, Diff Hole, Diff Bottom, T Val)
		{	// percolate _Hole to _Bottom, then push val, using operator<
			Diff Top = Hole;
			Diff Idx = 2 * Hole + 2;

			for (; Idx < Bottom; Idx = 2 * Idx + 2)
			{	// move _Hole down to larger child
				if ( *(First + Idx) < *(First + (Idx - 1) ) )
					--Idx;
				*(First + Hole) = *(First + Idx), Hole = Idx;
			}

			if (Idx == Bottom)
			{	// only child at bottom, move _Hole down to it
				*(First + Hole) = *(First + (Bottom - 1));
				Hole = Bottom - 1;
			}

			Push_heap(First, Hole, Top, Val);
		}
		
		template<class RanIt, class Diff, class T, class Pred> inline
		void Adjust_heap(RanIt First, Diff Hole, Diff Bottom, T Val, Pred pred)
		{	// percolate _Hole to _Bottom, then push val, using operator<
			Diff Top = Hole;
			Diff Idx = 2 * Hole + 2;

			for (; Idx < Bottom; Idx = 2 * Idx + 2)
			{	// move _Hole down to larger child
				if ( pred( *(First + Idx) , *(First + (Idx - 1) ) ) )
					--Idx;
				*(First + Hole) = *(First + Idx), Hole = Idx;
			}

			if (Idx == Bottom)
			{	// only child at bottom, move _Hole down to it
				*(First + Hole) = *(First + (Bottom - 1));
				Hole = Bottom - 1;
			}

			Push_heap(First, Hole, Top, Val, pred);
		}


		template<class RanIt, class T> inline
		void Pop_heap(RanIt First, RanIt Last, RanIt Dest, T Val )
		{	// pop *_First to *_Dest and reheap, using operator<
			*Dest = *First;
			typedef typename i3::iterator_traits<RanIt>::difference_type Diff;
			Adjust_heap(First, Diff(0), Diff(Last - First), Val);
		}

		template<class RanIt, class T, class Pred> inline
		void Pop_heap(RanIt First, RanIt Last, RanIt Dest, T Val, Pred pred )
		{	// pop *_First to *_Dest and reheap, using operator<
			*Dest = *First;
			typedef typename i3::iterator_traits<RanIt>::difference_type Diff;
			Adjust_heap(First, Diff(0), Diff(Last - First), Val, pred);
		}

		template<class RanIt> inline
		void Pop_heap_0(RanIt First, RanIt Last )
		{	// pop *_First to *(_Last - 1) and reheap, using operator<
			typedef typename i3::iterator_traits<RanIt>::value_type value_type;
			Pop_heap(First, Last - 1, Last - 1, value_type(*(Last - 1) )  );
		}
		
		template<class RanIt, class Pred> inline
		void Pop_heap_0(RanIt First, RanIt Last, Pred pred )
		{	// pop *_First to *(_Last - 1) and reheap, using operator<
			typedef typename i3::iterator_traits<RanIt>::value_type value_type;
			Pop_heap(First, Last - 1, Last - 1, value_type(*(Last - 1) ), pred  );
		}



		template<class RanIt> inline
		void Med3(RanIt First, RanIt Mid, RanIt Last)
		{	// sort median of three elements to middle
			if (*Mid < *First) i3::iter_swap(Mid, First);
			if (*Last < *Mid)  i3::iter_swap(Last, Mid);
			if (*Mid < *First) i3::iter_swap(Mid, First);
		}
		
		template<class RanIt, class Pred> inline
		void Med3(RanIt First, RanIt Mid, RanIt Last, Pred pred)
		{	// sort median of three elements to middle
			if ( pred(*Mid, *First)) i3::iter_swap(Mid, First);
			if ( pred(*Last, *Mid)) i3::iter_swap(Last, Mid);
			if ( pred(*Mid, *First)) i3::iter_swap(Mid, First);
		}
		
		template<class RanIt> inline
		void Median(RanIt First, RanIt Mid, RanIt Last)
		{	// sort median element to middle
			if (40 < Last - First)
			{	// median of nine
				size_t Step = (Last - First + 1) / 8;
				Med3(First, First + Step, First + 2 * Step);
				Med3(Mid - Step, Mid, Mid + Step);
				Med3(Last - 2 * Step, Last - Step, Last);
				Med3(First + Step, Mid, Last - Step);
			}
			else
				Med3(First, Mid, Last);
		}

		template<class RanIt, class Pred> inline
		void Median(RanIt First, RanIt Mid, RanIt Last, Pred pred)
		{	// sort median element to middle
			if (40 < Last - First)
			{	// median of nine
				size_t Step = (Last - First + 1) / 8;
				Med3(First, First + Step, First + 2 * Step, pred);
				Med3(Mid - Step, Mid, Mid + Step, pred);
				Med3(Last - 2 * Step, Last - Step, Last, pred);
				Med3(First + Step, Mid, Last - Step, pred);
			}
			else
				Med3(First, Mid, Last, pred);
		}
		

		template<class RanIt> inline
		i3::pair<RanIt, RanIt> Unguarded_partition(RanIt First, RanIt Last)
		{	// partition [First, Last), using operator<
			RanIt Mid = First + (Last - First) / 2;	// sort median to _Mid
			Median(First, Mid, Last - 1);
			RanIt Pfirst = Mid;
			RanIt Plast = Pfirst + 1;

			while (First < Pfirst && !( *(Pfirst - 1) < *Pfirst ) && !(*Pfirst < *(Pfirst - 1) ) )
				--Pfirst;
			while (Plast < Last   && !(*Plast < *Pfirst) && !(*Pfirst < *Plast) )
				++Plast;

			RanIt Gfirst = Plast;
			RanIt Glast = Pfirst;

			for ( ; ; )
			{	// partition
				for (; Gfirst < Last; ++Gfirst)
					if (*Pfirst < *Gfirst)
						;
					else if (*Gfirst < *Pfirst)
						break;
					else
						i3::iter_swap(Plast++, Gfirst);

				for (; First < Glast; --Glast)
					if (*(Glast - 1) < *Pfirst)
						;
					else if (*Pfirst < *(Glast - 1))
						break;
					else
						i3::iter_swap(--Pfirst, Glast - 1);
				
				if (Glast == First && Gfirst == Last)
					return i3::pair<RanIt, RanIt>(Pfirst, Plast);

				if (Glast == First)
				{	// no room at bottom, rotate pivot upward
					if (Plast != Gfirst)
						i3::iter_swap(Pfirst, Plast);
					++Plast;
					i3::iter_swap(Pfirst++, Gfirst++);
				}
				else if (Gfirst == Last)
				{	// no room at top, rotate pivot downward
					if (--Glast != --Pfirst)
						i3::iter_swap(Glast, Pfirst);
					i3::iter_swap(Pfirst, --Plast);
				}
				else
					i3::iter_swap(Gfirst++, --Glast);
			}
		}
		
		template<class RanIt, class Pred> inline
		i3::pair<RanIt, RanIt> Unguarded_partition(RanIt First, RanIt Last, Pred pred)
		{	// partition [_First, _Last), using _Pred
			RanIt Mid = First + (Last - First) / 2;
			Median(First, Mid, Last - 1, pred);
			RanIt Pfirst = Mid;
			RanIt Plast = Pfirst + 1;

			while (First < Pfirst && !pred( *(Pfirst - 1), *Pfirst)  && !pred(*Pfirst, *(Pfirst - 1) ) )
				--Pfirst;
			while (Plast < Last   && !pred( *Plast, *Pfirst) && !pred(*Pfirst, *Plast) )
				++Plast;

			RanIt Gfirst = Plast;
			RanIt Glast = Pfirst;

			for (; ; )
			{	// partition
				for (; Gfirst < Last; ++Gfirst)
					if ( pred(*Pfirst, *Gfirst) )
						;
					else if (pred(*Gfirst, *Pfirst))
						break;
					else
						i3::iter_swap(Plast++, Gfirst);
				for (; First < Glast; --Glast)
					if (pred( *(Glast - 1), *Pfirst) )
							;
					else if (pred(*Pfirst, *(Glast - 1)))
						break;
					else
						i3::iter_swap(--Pfirst, Glast - 1);
				
				if (Glast == First && Gfirst == Last)
					return i3::pair<RanIt, RanIt>(Pfirst, Plast);

				if (Glast == First)
				{	// no room at bottom, rotate pivot upward
					if (Plast != Gfirst)
						i3::iter_swap(Pfirst, Plast);
					++Plast;
					i3::iter_swap(Pfirst++, Gfirst++);
				}
				else if (Gfirst == Last)
				{	// no room at top, rotate pivot downward
					if (--Glast != --Pfirst)
						i3::iter_swap(Glast, Pfirst);
					i3::iter_swap(Pfirst, --Plast);
				}
				else
					i3::iter_swap(Gfirst++, --Glast);
			}
		}

		
		const int ISORT_MAX = 32;	// maximum size for insertion sort

		template<class RanIt, class Diff> inline
		void Sort(RanIt First, RanIt Last, Diff Ideal)
		{	// order [_First, _Last), using operator<
			Diff Count;
			for (; ISORT_MAX < (Count = Last - First) && 0 < Ideal; )
			{	// divide and conquer by quicksort
				i3::pair<RanIt, RanIt> Mid = Unguarded_partition(First, Last);
				Ideal /= 2, Ideal += Ideal / 2;	// allow 1.5 log2(N) divisions

				if (Mid.first - First < Last - Mid.second)
				{	// loop on second half
					Sort(First, Mid.first, Ideal);
					First = Mid.second;
				}
				else
				{	// loop on first half
					Sort(Mid.second, Last, Ideal);
					Last = Mid.first;
				}
			}

			if (ISORT_MAX < Count)
			{	// heap sort if too many divisions
				i3::make_heap(First, Last);
				i3::sort_heap(First, Last);
			}
			else if (1 < Count)
				i3::insertion_sort(First, Last);	// small
		}

		template<class RanIt, class Diff, class Pred> inline
		void Sort(RanIt First, RanIt Last, Diff Ideal, Pred pred)
		{	// order [_First, _Last), using operator<
			Diff Count;
			for (; ISORT_MAX < (Count = Last - First) && 0 < Ideal; )
			{	// divide and conquer by quicksort
				i3::pair<RanIt, RanIt> Mid = Unguarded_partition(First, Last, pred);
				Ideal /= 2, Ideal += Ideal / 2;	// allow 1.5 log2(N) divisions

				if (Mid.first - First < Last - Mid.second)
				{	// loop on second half
					Sort(First, Mid.first, Ideal, pred);
					First = Mid.second;
				}
				else
				{	// loop on first half
					Sort(Mid.second, Last, Ideal, pred);
					Last = Mid.first;
				}
			}

			if (ISORT_MAX < Count)
			{	// heap sort if too many divisions
				i3::make_heap(First, Last, pred);
				i3::sort_heap(First, Last, pred);
			}
			else if (1 < Count)
				i3::insertion_sort(First, Last, pred);	// small
		}

		template<class BidIt> inline
		void Reverse(BidIt First, BidIt Last, i3::bidirectional_iterator_tag)
		{
			for (; First != Last && First != --Last; ++First)
				i3::iter_swap(First, Last);
		}

		template<class BidIt> inline
		void Reverse(BidIt First, BidIt Last, i3::random_access_iterator_tag)
		{
			for (; First < Last; ++First)
				i3::iter_swap(First, --Last);
		}
		
		template<class forward_iterator> inline
		void rotate_impl( forward_iterator first, forward_iterator mid, forward_iterator last, i3::forward_iterator_tag )
		{
			for (forward_iterator next = mid; ; )
			{	// swap [_First, ...) into place
				i3::iter_swap(first, next);
				if (++first == mid)
					if (++next == last) break;	// done, quit
					else mid = next;	// mark end of next interval
				else if (++next == last)
					next = mid;	// wrap to last end
			}
		}
		
		template<class bidirectional_iterator> inline
		void rotate_impl(bidirectional_iterator first, bidirectional_iterator mid, bidirectional_iterator last, i3::bidirectional_iterator_tag)
		{	// rotate [_First, _Last), bidirectional iterators
			i3::reverse(first, mid);
			i3::reverse(mid, last);
			i3::reverse(first, last);
		}
		
		template<class random_access_iterator> inline
		void rotate_impl(random_access_iterator first, random_access_iterator mid, random_access_iterator last, i3::random_access_iterator_tag)
		{
			typedef typename i3::iterator_traits<random_access_iterator>::difference_type diff;
			typedef typename i3::iterator_traits<random_access_iterator>::value_type		T;

			diff shift = mid - first;			diff count = last - first;

			for (diff factor = shift; factor != 0; )
			{	// find subcycle count as GCD of shift count and length
				diff tmp = count % factor;
				count = factor, factor = tmp;
			}

			if (count < last - first)
			{
				for (; 0 < count; --count)
				{	// rotate each subcycle
					random_access_iterator hole = first + count;
					random_access_iterator next = hole;
					T holeval = *hole;
					random_access_iterator next1 = next + shift == last ? first : next + shift;
					while (next1 != hole)
					{	// percolate elements back around subcycle
						*next = *next1;
						next = next1;
						next1 = shift < last - next1 ? next1 + shift : first + (shift - (last - next1));
					}
					*next = holeval;
				}
			}
		}


		template<class input_iterator, class output_iterator> inline
		output_iterator unique_copy_impl( input_iterator first, input_iterator last, output_iterator dest, i3::input_iterator_tag)
		{
			typename i3::iterator_traits<input_iterator>::value_type val = *first;
			for ( *dest = val; ++first != last ; )
				if ( !(val == *first ) )
					val = *first, *++dest = val;
			return ++dest;
		}
		// outputiter는 inputiter와 동일하게..
		template<class input_iterator, class output_iterator> inline
		output_iterator unique_copy_impl( input_iterator first, input_iterator last, output_iterator dest, i3::output_iterator_tag)
		{
			typename i3::iterator_traits<input_iterator>::value_type val = *first;
			for ( *dest = val; ++first != last ; )
				if ( !(val == *first ) )
					val = *first, *++dest = val;
			return ++dest;
		}

		template<class input_iterator, class output_iterator> inline
		output_iterator unique_copy_impl( input_iterator first, input_iterator last, output_iterator dest, i3::forward_iterator_tag)
		{
			for ( *dest = *first ; ++first != last ; )
				if ( !(*dest == *first) )
					*++dest = *first;
			return ++dest;
		}
		
		template<class input_iterator, class output_iterator, class binary_pred> inline
		output_iterator unique_copy_impl( input_iterator first, input_iterator last, output_iterator dest, binary_pred pr, i3::input_iterator_tag)
		{
			typename i3::iterator_traits<input_iterator>::value_type val = *first;
			for ( *dest = val; ++first != last ; )
				if ( !pr(val, *first ) )
					val = *first, *++dest = val;
			return ++dest;
		}
		// outputiter는 inputiter와 동일하게..
		template<class input_iterator, class output_iterator, class binary_pred> inline
		output_iterator unique_copy_impl( input_iterator first, input_iterator last, output_iterator dest, binary_pred pr, i3::output_iterator_tag)
		{
			typename i3::iterator_traits<input_iterator>::value_type val = *first;
			for ( *dest = val; ++first != last ; )
				if ( !pr(val, *first ) )
					val = *first, *++dest = val;
			return ++dest;
		}

		template<class input_iterator, class output_iterator, class binary_pred> inline
		output_iterator unique_copy_impl( input_iterator first, input_iterator last, output_iterator dest, binary_pred pr, i3::forward_iterator_tag)
		{
			for ( *dest = *first ; ++first != last ; )
				if ( !pr(*dest, *first) )
					*++dest = *first;
			return ++dest;
		}
		
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

	template<class InputIterator, class Fn1> inline
	Fn1 for_each(InputIterator First, InputIterator Last, Fn1 Func)
	{	
		for (; First != Last; ++First)
			Func(*First);
		return Func;
	}

	template<class InputIterator, class T> inline 
	InputIterator find(InputIterator First, InputIterator Last, const T& Val) {	return  detail::Find(First, Last, Val); }	

	template<class InputIterator, class Pred> inline
	InputIterator find_if(InputIterator First, InputIterator Last, Pred pr)
	{
		for (; First != Last; ++First) if (pr(*First)) break;
		return First;
	}

	template<class InputIterator, class OutputIterator, class Fn1> inline
	OutputIterator transform(InputIterator First, InputIterator Last, OutputIterator Dest, Fn1 Func)
	{
		for (; First != Last; ++First, ++Dest)
			*Dest = Func(*First);
		return (Dest);
	}

	template<class InputIterator1, class InputIterator2, class OutputIterator, class Fn2> inline
	OutputIterator transform(InputIterator1 First1, InputIterator1 Last1, InputIterator2 First2, OutputIterator Dest, Fn2 Func)
	{
		for (; First1 != Last1; ++First1, ++First2, ++Dest)
			*Dest = Func(*First1, *First2);
		return (Dest);
	}

	template<class ForwardIterator, class T> inline
	ForwardIterator lower_bound(ForwardIterator First, ForwardIterator Last, const T& Val)
	{	
		typedef typename iterator_traits<ForwardIterator>::difference_type		difference_type;

		difference_type Count = 0;
		i3::distance( First, Last, Count );

		for (; 0 < Count; )
		{	// divide and conquer, find half that contains answer
			difference_type Count2 = Count / 2;
			ForwardIterator Mid = First;
			i3::advance(Mid, Count2);

			if ( *Mid < Val )
				First = ++Mid, Count -= Count2 + 1;
			else
				Count = Count2;
		}

		return First;
	}

	template<class ForwardIterator, class T, class Pred> inline
	ForwardIterator lower_bound(ForwardIterator First, ForwardIterator Last, const T& Val, Pred Pr )
	{	
		typedef typename iterator_traits<ForwardIterator>::difference_type		difference_type;

		difference_type Count = 0;
		i3::distance(First, Last, Count);

		for (; 0 < Count; )
		{	// divide and conquer, find half that contains answer
			difference_type Count2 = Count / 2;
			ForwardIterator Mid = First;
			i3::advance(Mid, Count2);

			if ( Pr(*Mid, Val) )
				First = ++Mid, Count -= Count2 + 1;
			else
				Count = Count2;
		}
		return First;
	}

	template<class ForwardIterator, class T> inline
	ForwardIterator upper_bound(ForwardIterator First, ForwardIterator Last, const T& Val)
	{	
		typedef typename iterator_traits<ForwardIterator>::difference_type		difference_type;

		difference_type Count = 0;
		i3::distance(First, Last, Count);

		for (; 0 < Count; )
		{	// divide and conquer, find half that contains answer
			difference_type Count2 = Count / 2;
			ForwardIterator Mid = First;
			i3::advance(Mid, Count2);
			if ( !(Val < *Mid) )
				First = ++Mid, Count -= Count2 + 1;
			else
				Count = Count2;
		}
		return First;
	}

	template<class ForwardIterator, class T, class Pred> inline
	ForwardIterator upper_bound(ForwardIterator First, ForwardIterator Last, const T& Val, Pred Pr)
	{	
		typedef typename iterator_traits<ForwardIterator>::difference_type		difference_type;
		
		difference_type Count = 0;
		i3::distance(First, Last, Count);
		
		for (; 0 < Count; )
		{	// divide and conquer, find half that contains answer
			difference_type Count2 = Count / 2;
			ForwardIterator Mid = First;
			i3::advance(Mid, Count2);
			if ( !Pr( Val, *Mid) )
				First = ++Mid, Count -= Count2 + 1;
			else
				Count = Count2;
		}
		return First;
	}

	template<class forward_iterator, class T> inline
	i3::pair<forward_iterator, forward_iterator> equal_range(forward_iterator f, forward_iterator l, const T& val)
	{
		typedef typename iterator_traits<forward_iterator>::difference_type diff;
	
		diff count = 0;
		i3::distance(f, l, count);

		for (; 0 < count; )
		{	
			diff count2 = count / 2;
			forward_iterator mid = f;
			i3::advance(mid, count2);
			
			if (*mid < val)
			{	
				f = ++mid;		count -= count2 + 1;
			}
			else if (val < *mid)
				count = count2;	
			else
			{	
				forward_iterator f2 = lower_bound(f, mid, val);
				i3::advance(f, count);
				forward_iterator l2 = upper_bound(++mid, f, val);
				return i3::pair<forward_iterator, forward_iterator>(f2, l2);
			}
		}

		return i3::pair<forward_iterator, forward_iterator>(f, f);	
	}

	template<class forward_iterator, class T, class Pred> inline
	i3::pair<forward_iterator, forward_iterator> equal_range(forward_iterator f, forward_iterator l, const T& val, Pred pr)
	{
		typedef typename iterator_traits<forward_iterator>::difference_type diff;

		diff count = 0;
		i3::distance(f, l, count);

		for (; 0 < count; )
		{	
			diff count2 = count / 2;
			forward_iterator mid = f;
			i3::advance(mid, count2);

			if (pr(*mid, val) )
			{	
				f = ++mid;		count -= count2 + 1;
			}
			else if (pr(val, *mid) )
				count = count2;	
			else
			{	
				forward_iterator f2 = lower_bound(f, mid, val, pr);
				i3::advance(f, count);
				forward_iterator l2 = upper_bound(++mid, f, val, pr);
				return i3::pair<forward_iterator, forward_iterator>(f2, l2);
			}
		}

		return i3::pair<forward_iterator, forward_iterator>(f, f);	
	}

	template<class forward_iterator, class T> inline
	bool	binary_search(forward_iterator f, forward_iterator l, const T& val)
	{
		f = i3::lower_bound(f, l, val);
		return f != l && !(val < *f);
	}

	template<class forward_iterator, class T, class Pred> inline
	bool	binary_search(forward_iterator f, forward_iterator l, const T& val, Pred pr)
	{
		f = i3::lower_bound(f, l, val, pr);
		return f != l && !pr(val, *f);
	}


	template<class RanIt> inline
	void push_heap(RanIt First, RanIt Last)
	{	// push *(_Last - 1) onto heap at [_First, _Last - 1), using operator<
		if (First != Last) 	// check and push to nontrivial heap
			detail::Push_heap_0(First, --Last);
	}
	
	template<class RanIt, class Pred> inline
	void push_heap(RanIt First, RanIt Last, Pred pred)
	{	// push *(_Last - 1) onto heap at [_First, _Last - 1), using _Pred
		if (First != Last)	// check and push to nontrivial heap
			detail::Push_heap_0(First, --Last, pred );
	}
	
	template<class RanIt> inline
	void pop_heap(RanIt First, RanIt Last)
	{	// pop *_First to *(_Last - 1) and reheap, using operator<
		if (1 < Last - First)
			detail::Pop_heap_0(First, Last);
	}

	template<class RanIt, class Pred> inline
		void pop_heap(RanIt First, RanIt Last, Pred pred)
	{	// pop *_First to *(_Last - 1) and reheap, using _Pred
		if (1 < Last - First)
			detail::Pop_heap_0(First, Last, pred);
	}


	template<class RanIt> inline
	void make_heap(RanIt First, RanIt Last)
	{	// make [_First, _Last) into a heap, using operator<
		if (1 < Last - First)
		{
			typedef typename i3::iterator_traits<RanIt>::difference_type Diff;
			typedef typename i3::iterator_traits<RanIt>::value_type value_type;
			
			Diff Bottom = Last - First;

			for (Diff Hole = Bottom / 2; 0 < Hole; )
			{	// reheap top half, bottom to top
				--Hole;
				detail::Adjust_heap(First, Hole, Bottom, value_type(*(First + Hole) ) );
			}
		}
	}
	
	template<class RanIt, class Pred> inline
	void make_heap(RanIt First, RanIt Last, Pred pred)
	{	// make [_First, _Last) into a heap, using operator<
		if (1 < Last - First)
		{
			typedef typename i3::iterator_traits<RanIt>::difference_type Diff;
			typedef typename i3::iterator_traits<RanIt>::value_type value_type;

			Diff Bottom = Last - First;

			for (Diff Hole = Bottom / 2; 0 < Hole; )
			{	// reheap top half, bottom to top
				--Hole;
				detail::Adjust_heap(First, Hole, Bottom, value_type(*(First + Hole) ), pred );
			}
		}
	}

	template<class RanIt> inline
	void sort_heap(RanIt First, RanIt Last)
	{	// order heap by repeatedly popping, using operator<
		for (; 1 < Last - First; --Last)
			i3::pop_heap(First, Last);
	}
	
	template<class RanIt, class Pred> inline
	void sort_heap(RanIt First, RanIt Last, Pred pred)
	{	// order heap by repeatedly popping, using operator<
		for (; 1 < Last - First; --Last)
			i3::pop_heap(First, Last, pred);
	}
	
	template<class BidIt> inline
	void insertion_sort(BidIt First, BidIt Last)
	{
		typedef typename i3::iterator_traits<BidIt>::value_type value_type;

		if (First != Last)
			for (BidIt Next = First; ++Next != Last; )
			{	// order next element
				BidIt Next1 = Next;
				value_type Val = *Next;

				if (Val < *First)
				{	// found new earliest element, move to front
					i3::copy_backward(First, Next, ++Next1 );
					*First = Val;
				}
				else
				{	// look for insertion point after first
					for (BidIt First1 = Next1; Val < *--First1 ; Next1 = First1)
						*Next1 = *First1;	// move hole down
					*Next1 = Val;	// insert element in hole
				}
			}
		
	}

	template<class BidIt, class Pred> inline
	void insertion_sort(BidIt First, BidIt Last, Pred pred)
	{
				
		typedef typename i3::iterator_traits<BidIt>::value_type value_type;

		if (First != Last)
			for (BidIt Next = First; ++Next != Last; )
			{	// order next element
				BidIt Next1 = Next;
				value_type Val = *Next;

				if ( pred(Val, *First) )
				{	// found new earliest element, move to front
					i3::copy_backward(First, Next, ++Next1 );
					*First = Val;
				}
				else
				{	// look for insertion point after first
					for (BidIt First1 = Next1; pred(Val, *--First1 ) ; Next1 = First1)
						*Next1 = *First1;	// move hole down
					*Next1 = Val;	// insert element in hole
				}
			}
	}
	
	template<class RanIt> inline
	void sort(RanIt First, RanIt Last)
	{	
		detail::Sort(First, Last, Last - First);
	}
	
	template<class RanIt, class Pred> inline
	void sort(RanIt First, RanIt Last, Pred pred)
	{	
		detail::Sort(First, Last, Last - First, pred);
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


	template<class BidIt> inline
	void reverse(BidIt First, BidIt Last)
	{
		detail::Reverse( First, Last,  typename i3::iterator_traits<BidIt>::iterator_category() );
	}

	template<class BidIt, class OutIt> inline
	OutIt reverse_copy(BidIt First, BidIt Last, OutIt Dest)
	{
		for (; First != Last; ++Dest)
			*Dest = *--Last;
		return Dest;
	}
	
	// a_find와 unique 쪽은 GCC쪽과 다소 혼용...
	template<class forward_iterator> inline
	forward_iterator adjacent_find(forward_iterator first, forward_iterator last)
	{
		if ( first == last ) return last;
		for ( forward_iterator next = first; ++next != last ; first = next )
			if ( *first == *next ) return first;
		return last;
	}

	template<class forward_iterator, class binary_pred> inline
	forward_iterator adjacent_find(forward_iterator first, forward_iterator last, binary_pred pr)
	{
		if ( first == last ) return last;
		for ( forward_iterator next = first; ++next != last ; first = next )
			if ( pr(*first,*next ) ) return first;
		return last;
	}

	template<class  input_iterator, class output_iterator> inline
	output_iterator	unique_copy(input_iterator first, input_iterator last, output_iterator dest)
	{
		return ( first == last ) ? dest : detail::unique_copy_impl(first, last, dest, typename i3::iterator_traits<input_iterator>::iterator_category() );
	}
	
	template<class  input_iterator, class output_iterator, class binary_pred> inline
	output_iterator	unique_copy(input_iterator first, input_iterator last, output_iterator dest, binary_pred pr)
	{
		return ( first == last ) ? dest : detail::unique_copy_impl(first, last, dest, pr, typename i3::iterator_traits<input_iterator>::iterator_category() );
	}

	template<class forward_iterator> inline
	forward_iterator unique(forward_iterator first, forward_iterator last)
	{
		first = i3::adjacent_find(first, last);
		return i3::unique_copy(first, last, first);
	}

	template<class forward_iterator, class binary_pred> inline
	forward_iterator unique(forward_iterator first, forward_iterator last, binary_pred pr)
	{
		first = i3::adjacent_find(first, last, pr);
		return i3::unique_copy(first, last, first, pr);
	}
	

	template<class forward_iterator> inline
	void rotate(forward_iterator first, forward_iterator mid, forward_iterator last)
	{
		if ( first != mid && mid != last )
			detail::rotate_impl(first, mid, last, typename i3::iterator_traits<forward_iterator>::iterator_category() );
	}

	template<class forward_iterator, class out_iterator> inline
	out_iterator rotate_copy(forward_iterator first, forward_iterator mid, forward_iterator last, out_iterator dest)
	{
		dest = i3::copy(mid, last, dest);
		return i3::copy(first, mid, dest);
	}


	template<class input_iterator1, class input_iterator2, class output_iterator> inline
	output_iterator	merge(input_iterator1 first1, input_iterator1 last1, input_iterator2 first2, input_iterator2 last2, output_iterator dest)
	{
		for ( ; first1 != last1 && first2 != last2 ; ++dest )
			if ( *first2 < *first1 )
				*dest = *first2, ++first2;
			else
				*dest = *first1, ++first1;

		dest = i3::copy(first1, last1, dest);
		return i3::copy(first2, last2, dest);
	}

	template<class input_iterator1, class input_iterator2, class output_iterator, class binary_pred> inline
	output_iterator	merge(input_iterator1 first1, input_iterator1 last1, input_iterator2 first2, input_iterator2 last2, output_iterator dest, binary_pred pr)
	{
		for ( ; first1 != last1 && first2 != last2 ; ++dest )
			if ( pr(*first2, *first1) )
				*dest = *first2, ++first2;
			else
				*dest = *first1, ++first1;
		dest = i3::copy(first1, last1, dest);
		return i3::copy(first2, last2, dest);
	}
	
	template<class bidirectional_iterator1, class bidirectional_iterator2, class bidirectional_iterator3> inline
	bidirectional_iterator3 merge_backward(bidirectional_iterator1 first1, bidirectional_iterator1 last1,
		bidirectional_iterator2 first2, bidirectional_iterator2 last2, bidirectional_iterator3 dest)
	{	// merge backwards to _Dest, using operator<
		for ( ; ; )
			if (first1 == last1)
				return i3::copy_backward(first2, last2, dest);
			else if (first2 == last2)
				return i3::copy_backward(first1, last1, dest);
			else if (*--last2 < *--last1 )
				*--dest = *last1, ++last2;
			else
				*--dest = *last2, ++last1;
	}
	
	template<class bidirectional_iterator1, class bidirectional_iterator2, class bidirectional_iterator3, class binary_pred> inline
		bidirectional_iterator3 merge_backward(bidirectional_iterator1 first1, bidirectional_iterator1 last1,
		bidirectional_iterator2 first2, bidirectional_iterator2 last2, bidirectional_iterator3 dest, binary_pred pr)
	{	// merge backwards to _Dest, using operator<
		for ( ; ; )
			if (first1 == last1)
				return i3::copy_backward(first2, last2, dest);
			else if (first2 == last2)
				return i3::copy_backward(first1, last1, dest);
			else if (pr(*--last2, *--last1))
				*--dest = *last1, ++last2;
			else
				*--dest = *last2, ++last1;
	}
	
	template<class InputIt, class OutputIt, class T> inline
	OutputIt remove_copy(InputIt f, InputIt l, OutputIt dest, const T& val)
	{
		for (; f != l; ++f)
			if (!(*f == val)) *dest++ = *f;
		return dest;
	}
	
	template<class InputIt, class OutputIt, class Pred> inline
	OutputIt remove_copy_if(InputIt f, InputIt l, OutputIt dest, Pred pr)
	{
		for (; f != l; ++f)
			if (!pr(*f)) *dest++ = *f;
		return dest;
	}
	
	template<class FwdIt, class T> inline
	FwdIt	remove(FwdIt f, FwdIt l, const T& val)
	{
		f = find(f, l, val);
		if (f == l) return f;	// empty sequence, all done
		else
		{	// nonempty sequence, worth doing
			FwdIt f1 = f;
			return remove_copy(++f1, l, f, val);
		}
	}

	template<class FwdIt, class Pred> inline
	FwdIt	remove_if(FwdIt f, FwdIt l, Pred pr)
	{
		f = find_if(f, l, pr);
		if (f == l) return f;	// empty sequence, all done
		else
		{	// nonempty sequence, worth doing
			FwdIt f1 = f;
			return remove_copy_if(++f1, l, f, pr);
		}
	}
	

}