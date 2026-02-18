#pragma once

#include "detail/algorithm_impl.h"

namespace i3
{
	
	template<class InputIterator, class Fn1> inline
	Fn1 for_each(InputIterator First, InputIterator Last, Fn1 Func);
	
	template<class InputIterator, class T> inline 
	InputIterator find(InputIterator First, InputIterator Last, const T& Val); 
	
	template<class InputIterator, class Pred> inline
	InputIterator find_if(InputIterator First, InputIterator Last, Pred pr);

	template<class InputIterator, class OutputIterator, class Fn1> inline
	OutputIterator transform(InputIterator First, InputIterator Last, OutputIterator Dest, Fn1 Func);
	
	
	template<class InputIterator1, class InputIterator2, class OutputIterator, class Fn2> inline
	OutputIterator transform(InputIterator1 First1, InputIterator1 Last1, InputIterator2 First2, OutputIterator Dest, Fn2 Func);
	
		
	template<class ForwardIterator, class T> inline
	ForwardIterator lower_bound(ForwardIterator First, ForwardIterator Last, const T& Val);
	
	template<class ForwardIterator, class T, class Pred> inline
	ForwardIterator lower_bound(ForwardIterator First, ForwardIterator Last, const T& Val, Pred Pr );

	template<class ForwardIterator, class T> inline
	ForwardIterator upper_bound(ForwardIterator First, ForwardIterator Last, const T& Val);

	template<class ForwardIterator, class T, class Pred> inline
	ForwardIterator upper_bound(ForwardIterator First, ForwardIterator Last, const T& Val, Pred Pr);
	
	template<class forward_iterator, class T> inline
	i3::pair<forward_iterator, forward_iterator> equal_range(forward_iterator f, forward_iterator l, const T& val);

	template<class forward_iterator, class T, class Pred> inline
	i3::pair<forward_iterator, forward_iterator> equal_range(forward_iterator f, forward_iterator l, const T& val, Pred pr);

	template<class forward_iterator, class T> inline
	bool	binary_search(forward_iterator f, forward_iterator l, const T& val);

	template<class forward_iterator, class T, class Pred> inline
	bool	binary_search(forward_iterator f, forward_iterator l, const T& val, Pred pr);


	template<class RanIt> inline
	void push_heap(RanIt First, RanIt Last);

	template<class RanIt, class Pred> inline
	void push_heap(RanIt First, RanIt Last, Pred pred);

	template<class RanIt> inline
	void pop_heap(RanIt First, RanIt Last);
	
	template<class RanIt, class Pred> inline
	void pop_heap(RanIt First, RanIt Last, Pred pred);

	template<class RanIt> inline
	void make_heap(RanIt First, RanIt Last);
	
	template<class RanIt, class Pred> inline
	void make_heap(RanIt First, RanIt Last, Pred pred);

	template<class RanIt> inline
	void sort_heap(RanIt First, RanIt Last);
	
	template<class RanIt, class Pred> inline
	void sort_heap(RanIt First, RanIt Last, Pred pred);

	template<class BidIt> inline
	void insertion_sort(BidIt First, BidIt Last);

	template<class BidIt, class Pred> inline
	void insertion_sort(BidIt First, BidIt Last, Pred pred);

	template<class RanIt> inline
	void sort(RanIt First, RanIt Last);
	
	template<class RanIt, class Pred> inline
	void sort(RanIt First, RanIt Last, Pred pred);
	
	template<class bidirectional_iterator> inline
	void stable_sort( bidirectional_iterator first, bidirectional_iterator last);

	template<class bidirectional_iterator, class binary_pred> inline
	void stable_sort( bidirectional_iterator first, bidirectional_iterator last, binary_pred pr);



	template<class BidIt> inline
	void reverse(BidIt First, BidIt Last);

	template<class BidIt, class OutIt> inline
	OutIt reverse_copy(BidIt First, BidIt Last, OutIt Dest);

	template<class forward_iterator> inline
	forward_iterator adjacent_find(forward_iterator first, forward_iterator last);

	template<class forward_iterator, class binary_pred> inline
	forward_iterator adjacent_find(forward_iterator first, forward_iterator last, binary_pred pr);

	template<class  input_iterator, class output_iterator> inline
	output_iterator	unique_copy(input_iterator first, input_iterator last, output_iterator dest);

	template<class  input_iterator, class output_iterator, class binary_pred> inline
	output_iterator	unique_copy(input_iterator first, input_iterator last, output_iterator dest, binary_pred pr);
	
	template<class forward_iterator> inline
	forward_iterator unique(forward_iterator first, forward_iterator last);

	template<class forward_iterator, class binary_pred> inline
	forward_iterator unique(forward_iterator first, forward_iterator last, binary_pred pr);

	template<class forward_iterator> inline
	void rotate(forward_iterator first, forward_iterator mid, forward_iterator last);

	template<class forward_iterator, class out_iterator> inline
	out_iterator rotate_copy(forward_iterator first, forward_iterator mid, forward_iterator last, out_iterator dest);


	template<class input_iterator1, class input_iterator2, class output_iterator> inline
	output_iterator	merge(input_iterator1 first1, input_iterator1 last1, input_iterator2 first2, input_iterator2 last2, output_iterator dest);

	template<class input_iterator1, class input_iterator2, class output_iterator, class binary_pred> inline
	output_iterator	merge(input_iterator1 first1, input_iterator1 last1, input_iterator2 first2, input_iterator2 last2, output_iterator dest, binary_pred pr);

	template<class bidirectional_iterator1, class bidirectional_iterator2, class bidirectional_iterator3> inline
		bidirectional_iterator3 merge_backward(bidirectional_iterator1 first1, bidirectional_iterator1 last1,
		bidirectional_iterator2 first2, bidirectional_iterator2 last2, bidirectional_iterator3 dest);

	template<class bidirectional_iterator1, class bidirectional_iterator2, class bidirectional_iterator3, class binary_pred> inline
		bidirectional_iterator3 merge_backward(bidirectional_iterator1 first1, bidirectional_iterator1 last1,
		bidirectional_iterator2 first2, bidirectional_iterator2 last2, bidirectional_iterator3 dest, binary_pred pr);

	template<class InputIt, class OutputIt, class T> inline
	OutputIt remove_copy(InputIt f, InputIt l, OutputIt dest, const T& val);

	template<class InputIt, class OutputIt, class Pred> inline
	OutputIt remove_copy_if(InputIt f, InputIt l, OutputIt dest, Pred pr);

	template<class FwdIt, class T> inline
	FwdIt	remove(FwdIt f, FwdIt l, const T& val);

	template<class FwdIt, class Pred> inline
	FwdIt	remove_if(FwdIt f, FwdIt l, Pred pr);

}