#pragma once

#include "../mpl_iterator_tags.h"
#include "../mpl_advance_fwd.h"
#include "../mpl_distance_fwd.h"
#include "../mpl_next_prior.h"
#include "../mpl_deref.h"
#include "../mpl_int.h"


namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			
			template< class T, int is_last_>
			struct sel_iter;

			template< class T >
			struct sel_iter<T,0>
			{
				typedef random_access_iterator_tag category;
				typedef sel_iter<T,1> next;
				typedef T type;
			};

			template< class T >
			struct sel_iter<T,1>
			{
				typedef random_access_iterator_tag category;
				typedef sel_iter<T,0> prior;
			};
		}

		template< class T, int is_last_, class Distance >
		struct advance< aux::sel_iter<T,is_last_>,Distance>
		{
			typedef aux::sel_iter<T, ( is_last_ + Distance::value ) > type;
		};

		template< class T, int l1, int l2>
		struct distance< aux::sel_iter<T,l1>, aux::sel_iter<T,l2> > : int_<( l2 - l1 )> {};

	}
}