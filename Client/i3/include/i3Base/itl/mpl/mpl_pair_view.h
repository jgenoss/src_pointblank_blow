#pragma once

#include "mpl_begin_end.h"
#include "mpl_iterator_category.h"
#include "mpl_advance.h"
#include "mpl_distance.h"
#include "mpl_next_prior.h"
#include "mpl_deref.h"
#include "mpl_min_max.h"
#include "mpl_pair.h"
#include "mpl_iterator_tags.h"
#include "mpl_na.h"

namespace i3
{
	namespace mpl
	{
		namespace aux {	struct pair_iter_tag; }


		template< class Iter1, class Iter2, class Category>
		struct pair_iter
		{
			typedef aux::pair_iter_tag tag;
			typedef Category category;
			typedef Iter1 first;
			typedef Iter2 second;
		};
		
		template< class Iter1, class Iter2, class C >
		struct deref< pair_iter<Iter1,Iter2,C> >
		{
			typedef pair< typename deref<Iter1>::type, typename deref<Iter2>::type > type;
		};

		template< class Iter1, class Iter2, class C >
		struct next< pair_iter<Iter1,Iter2,C> >
		{
			typedef typename mpl::next<Iter1>::type i1_;
			typedef typename mpl::next<Iter2>::type i2_;
			typedef pair_iter<i1_,i2_,C> type;
		};

		template< class Iter1, class Iter2, class C >
		struct prior< pair_iter<Iter1,Iter2,C> >
		{
			typedef typename mpl::prior<Iter1>::type i1_;
			typedef typename mpl::prior<Iter2>::type i2_;
			typedef pair_iter<i1_,i2_,C> type;
		};
		

		template<> struct advance_impl<aux::pair_iter_tag>
		{
			template< class Iter, class D > struct apply
			{
				typedef typename mpl::advance< typename Iter::first,D >::type i1_;
				typedef typename mpl::advance< typename Iter::second,D >::type i2_;
				typedef pair_iter<i1_,i2_,typename Iter::category> type;
			};
		};

		template<> struct distance_impl<aux::pair_iter_tag>
		{
			template< class Iter1, class Iter2 > struct apply
			{
				typedef typename mpl::distance< typename first<Iter1>::type, typename first<Iter2>::type >::type type;	// agurt, 10/nov/04: MSVC 6.5 ICE-s on forwarding
			};
		};

		template< class Seq1 = na, class Seq2 = na>
		struct pair_view
		{
			typedef nested_begin_end_tag tag;

			typedef typename begin<Seq1>::type iter1_;
			typedef typename begin<Seq2>::type iter2_;
			typedef typename min<typename iterator_category<iter1_>::type, typename iterator_category<iter2_>::type >::type category_;
		    
			typedef pair_iter<iter1_,iter2_,category_> begin;
		    
			typedef pair_iter< typename end<Seq1>::type, typename end<Seq2>::type, category_> end;
		};
		
	}
}

#define na_spec (2, pair_view)
#include "aux_/mpl_na_spec.h"
