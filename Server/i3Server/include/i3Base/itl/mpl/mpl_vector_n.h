#if !defined(pp_is_iterating)

#ifndef mpl_vector_n_h
#define mpl_vector_n_h

#include "mpl_vector_core.h"

#include "../preprocessor/repetition/pp_enum_shifted_params.h"
#include "../preprocessor/pp_iterate.h"


#define		pp_iteration_limits ( 0, mpl_vector_limit )		// 0 ~39까지는 특화 / 자체재귀..(양개구간)
#define		pp_filename_1	"mpl_vector_n.h"
#include	pp_iterate()


#endif //mpl_vector_n_h

#else // pp_is_iterating

#define		i_			pp_iteration()

namespace i3
{
	namespace mpl
	{

#if i_ > 0

		template< pp_enum_params(i_, class T)>
		struct pp_cat(vector,i_)
		{
			typedef aux::vector_tag<i_> tag;
			typedef pp_cat(vector,i_) type;

			#define aux_vector_item(unused, i_, unused2) typedef pp_cat(T,i_) pp_cat(item,i_); 
			pp_repeat(i_, aux_vector_item, unused)
			#undef aux_vector_item

			typedef void_					pp_cat(item,i_);
			typedef pp_cat(T,pp_dec(i_))	back;
			typedef v_iter<type,0>			begin;
			typedef v_iter<type,i_>			end;
		};
		
		template<>
		struct push_front_impl< aux::vector_tag<pp_dec(i_)> >
		{
			template< class Vector, class T > 
			struct apply
			{
				typedef pp_cat(vector,i_)< T pp_comma_if(pp_dec(i_)) pp_enum_params(pp_dec(i_), typename Vector::item)> type;
			};
		};

		template<>
		struct pop_front_impl< aux::vector_tag<i_> >
		{
			template< class Vector > 
			struct apply
			{
				typedef pp_cat(vector,pp_dec(i_))< pp_enum_shifted_params(i_, typename Vector::item) > type;
			};
		};

		template<>
		struct push_back_impl< aux::vector_tag<pp_dec(i_)> >
		{
			template< class Vector, class T > 
			struct apply
			{
				typedef pp_cat(vector,i_)< pp_enum_params(pp_dec(i_), typename Vector::item) pp_comma_if(pp_dec(i_)) T > type;
			};
		};

		template<>
		struct pop_back_impl< aux::vector_tag<i_> >
		{
			template< class Vector > 
			struct apply
			{
				typedef pp_cat(vector,pp_dec(i_))< pp_enum_params(pp_dec(i_), typename Vector::item) > type;
			};
		};

#endif // i_ > 0		

		template< class V >
		struct v_at<V,i_> {	typedef typename V::pp_cat(item,i_) type;	};

		
	}
}

#undef		i_

#endif // pp_is_iterating

