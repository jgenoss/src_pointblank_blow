#pragma once


namespace i3
{
	
	template<class ForwardIterator1, class ForwardIterator2> inline
	ForwardIterator1 search(ForwardIterator1 f1, ForwardIterator1 l1, ForwardIterator2 f2, ForwardIterator2 l2);
	
	template<class ForwardIterator1, class ForwardIterator2, class Pred2> inline
	ForwardIterator1 search(ForwardIterator1 f1, ForwardIterator1 l1, ForwardIterator2 f2, ForwardIterator2 l2, Pred2 pr);


}

#include "detail/search.inl"