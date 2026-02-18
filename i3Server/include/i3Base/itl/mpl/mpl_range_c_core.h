#pragma once

#include "mpl_front_fwd.h"
#include "mpl_back_fwd.h"
#include "mpl_next_prior.h"
#include "mpl_size_fwd.h"
#include "mpl_minus.h"
#include "mpl_O1_size_fwd.h"
#include "mpl_empty_fwd.h"
#include "mpl_equal_to.h"

#include "mpl_iterator_tags.h"
#include "mpl_advance_fwd.h"
#include "mpl_distance_fwd.h"
#include "mpl_next_prior.h"
#include "mpl_deref.h"
#include "mpl_plus.h"
#include "mpl_minus.h"


namespace i3
{
	namespace mpl
	{
		namespace aux 
		{
			struct half_open_range_tag;	
			struct r_iter_tag;
		}

		template<>
		struct front_impl< aux::half_open_range_tag >
		{
			template< class Range > 
			struct apply
			{
				typedef typename Range::start type;
			};
		};
		
		template<>
		struct back_impl< aux::half_open_range_tag >
		{
			template< class Range > 
			struct apply
			{
				typedef typename prior< typename Range::finish >::type type;
			};
		};

		template<>
		struct size_impl< aux::half_open_range_tag >
		{
			template< class Range > 
			struct apply : minus<typename Range::finish, typename Range::start>	{};
		};

		template<>
		struct O1_size_impl< aux::half_open_range_tag > : size_impl< aux::half_open_range_tag >	{};
		
		template<>
		struct empty_impl< aux::half_open_range_tag >
		{
			template< class Range > struct apply : equal_to<typename Range::start, typename Range::finish>{};
		};
		
		template< class N > 
		struct r_iter
		{
			typedef aux::r_iter_tag tag;
			typedef random_access_iterator_tag category;
			typedef N type;
		};
		
		template<class N>
		struct next< r_iter<N> >
		{
			typedef r_iter< typename mpl::next<N>::type > type;
		};

		template<class N>
		struct prior< r_iter<N> >
		{
			typedef r_iter< typename mpl::prior<N>::type > type;
		};
		
		template<> 
		struct advance_impl<aux::r_iter_tag>
		{
			template< class Iter, class Dist > 
			struct apply
			{
				typedef typename deref<Iter>::type n_;
				typedef typename plus<n_,Dist>::type m_;
				typedef integral_c< typename n_::value_type, m_::value> result_;
		        typedef r_iter<result_> type;
			};
		};

		template<> 
		struct distance_impl<aux::r_iter_tag>
		{
			template< class Iter1, class Iter2 > 
			struct apply : minus<typename Iter2::type, typename Iter1::type>{};
		};

	}
}
