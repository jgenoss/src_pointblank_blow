#pragma once

#include "mpl_find_if.h"

namespace i3
{
	
	namespace mpl
	{
		namespace aux
		{
			template<class IdxNT, int Index>
			struct find_of_ntype_pred_impl : bool_< (IdxNT::index == Index) > {};

			template<int Index>
			struct find_of_ntype_index_pred
			{
				template<class T> 
				struct apply : and_< is_idx_ntype<T>, find_of_ntype_pred_impl<T, Index> > {};
			};
		}

		template<class Seq, int Index>
		struct find_of_ntype_index 
		{
			typedef typename find_if<Seq, aux::find_of_ntype_index_pred<Index> >::type	result_iter1;
			typedef typename end<Seq>::type												end_iter;

			typedef typename eval_if<is_same<result_iter1, end_iter>, 
					find_of_ntype_index<Seq, default_index>, 
					identity<result_iter1> >::type											type;

		};
		
		template<class Seq>
		struct find_of_ntype_index<Seq, default_index> : find_if<Seq, aux::find_of_ntype_index_pred<default_index> > {};


	}
}