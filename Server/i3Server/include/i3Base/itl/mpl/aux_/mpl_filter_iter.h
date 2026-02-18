#pragma once

#include "../mpl_find_if.h"
#include "../mpl_iterator_range.h"
#include "../mpl_iterator_tags.h"
#include "../mpl_deref.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class Iterator, class LastIterator, class Pred> 
			struct filter_iter;

			template<class Iterator, class LastIterator, class Pred>
			struct next_filter_iter
			{
				typedef typename find_if<iterator_range<Iterator,LastIterator>, Pred>::type base_iter_;
			 	typedef filter_iter<base_iter_,LastIterator,Pred> type;
			};
			
			template<class Iterator, class LastIterator, class Pred>
			struct filter_iter
			{	
				typedef Iterator base;
				typedef forward_iterator_tag category;
				typedef typename aux::next_filter_iter< typename mpl::next<base>::type, LastIterator, Pred>::type next;
		    	typedef typename deref<base>::type type;
			};

			template<class LastIterator, class Pred>
			struct filter_iter< LastIterator,LastIterator,Pred >
			{
				typedef LastIterator base;
				typedef forward_iterator_tag category;
			};		
			
		}


	}
}

#define lambda_spec (3, aux::filter_iter)
#include "mpl_lambda_spec.h"
