#pragma once

#include "mpl_at_fwd.h"
#include "mpl_long.h"
#include "mpl_void.h"
#include "mpl_front_fwd.h"
#include "mpl_push_front_fwd.h"
#include "mpl_pop_front_fwd.h"
#include "mpl_push_back_fwd.h"
#include "mpl_pop_back_fwd.h"

#include "mpl_back_fwd.h"
#include "mpl_next_prior.h"

#include "mpl_clear_fwd.h"

#include "mpl_long.h"
#include "mpl_void.h"
#include "mpl_na.h"

#include "mpl_iterator_tags.h"
#include "mpl_plus.h"
#include "mpl_minus.h"
#include "mpl_advance_fwd.h"
#include "mpl_distance_fwd.h"
#include "mpl_next_prior.h"

#include "mpl_O1_size_fwd.h"
#include "mpl_size_fwd.h"

#include "mpl_empty_fwd.h"
#include "mpl_bool.h"

#include "mpl_integral_c.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			struct v_iter_tag;	
			template< long N > struct vector_tag;
		}
		
		template< class Vector, long n_ > struct v_at;

		template< long n_ >
		struct at_impl< aux::vector_tag<n_> >
		{
			template< class Vector, class N > 
			struct apply : v_at< Vector, N::value> {};
		};

		template< long n_ >
		struct front_impl< aux::vector_tag<n_> >
		{
			template< class Vector > 
			struct apply {	typedef typename Vector::item0 type; };
		};

		
		template< long n_ >
		struct back_impl< aux::vector_tag<n_> >
		{
			template< class Vector > 
			struct apply {	typedef typename Vector::back type;	};
		};
		

		template<class Vector, long n_>
		struct v_iter
		{
			typedef aux::v_iter_tag tag;
			typedef random_access_iterator_tag category;
			typedef typename v_at<Vector,n_>::type type;
			typedef Vector vector_;
			typedef mpl::long_<n_> pos;
		};
		
		template<class Vector, long n_>
		struct next< v_iter<Vector,n_> > {	typedef v_iter<Vector,(n_ + 1)> type;	};

		template<class Vector, long n_>
		struct prior< v_iter<Vector,n_> > {	typedef v_iter<Vector,(n_ - 1)> type;	};

		template<class Vector, long n_, class Distance>
		struct advance< v_iter<Vector,n_>,Distance> {	typedef v_iter< Vector, (n_ + Distance::value)> type; };

		template< class Vector, long n_, long m_> 
		struct distance< v_iter<Vector,n_>, v_iter<Vector,m_> > : mpl::long_<(m_ - n_)> {};
		

		template< class Dummy = na > 
		struct vector0;

		template<> struct vector0<na>
		{
			typedef aux::vector_tag<0> tag;
			typedef vector0 type;
			typedef void_ item0;
		    
			typedef v_iter<vector0<>,0> begin;
			typedef v_iter<vector0<>,0> end;
		};
		
		template< class T > 
		struct vector0_c : vector0<>
		{
			typedef vector0_c	type;
			typedef T			value_type;
		};

		template< long N >
		struct clear_impl< aux::vector_tag<N> >
		{
			template< class Vector > struct apply {	typedef vector0<> type;	};
		};
		
		template< long N >
		struct O1_size_impl< aux::vector_tag<N> >
		{
			template< class Vector > struct apply : mpl::long_<N> {};
		};
		
		template< long N >
		struct size_impl< aux::vector_tag<N> > : O1_size_impl< aux::vector_tag<N> > {};
		
		template<>
		struct empty_impl< aux::vector_tag<0> > {	template< typename Vector > struct apply : true_ {}; };
		
		template< long N >
		struct empty_impl< aux::vector_tag<N> > {   template< typename Vector > struct apply : false_ {}; };
		


	}
	

}
