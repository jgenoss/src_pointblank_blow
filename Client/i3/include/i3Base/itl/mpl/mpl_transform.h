#pragma once

#include "mpl_fold.h"
#include "mpl_reverse_fold.h"
#include "mpl_pair_view.h"
#include "mpl_is_sequence.h"
#include "mpl_lambda.h"
#include "mpl_bind.h"
#include "mpl_or.h"
#include "mpl_not.h"

//////
#include "mpl_back_inserter.h"
#include "mpl_front_inserter.h"
#include "mpl_push_back.h"
#include "mpl_push_front.h"
#include "mpl_clear.h"
#include "mpl_if.h"
#include "mpl_na.h"
///////


namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< class Seq, class Op, class In>
			struct transform1_impl : fold< Seq, typename In::state, 
												bind2< typename lambda< typename In::operation >::type, _1, bind1< typename lambda<Op>::type, _2> > > {};

			template< class Seq, class Op, class In>
			struct reverse_transform1_impl : reverse_fold<Seq,	typename In::state, 
												bind2< typename lambda< typename In::operation >::type, _1, bind1< typename lambda<Op>::type, _2> > > {};

			template< class Seq1, class Seq2, class Op, class In>
			struct transform2_impl : fold< pair_view<Seq1,Seq2>, typename In::state, 
				bind2< typename lambda< typename In::operation >::type, _1, bind2<typename lambda<Op>::type, bind1<first<>,_2>, bind1<second<>,_2> > > > {};

			template< class Seq1, class Seq2, class Op, class In>
			struct reverse_transform2_impl : reverse_fold< pair_view<Seq1,Seq2>, typename In::state, 
				bind2< typename lambda< typename In::operation >::type, _1, bind2<typename lambda<Op>::type, bind1<first<>,_2>, bind1<second<>,_2> > > > {};
		}

	// inserter algorithm && reverse algorithm..

		template<class P1 = na, class P2 = na, class P3 = na>		// 1,2,3,na
		struct transform1 : aux::transform1_impl<P1, P2, P3> {};
		
		template<class P1, class P2>							// 하나 줄여 특화..
		struct transform1<P1, P2, na> : if_<	has_push_back<typename clear<P1>::type>,			// clear는 P1 only
											aux::transform1_impl<P1, P2, back_inserter<typename clear<P1>::type> >,		// 하나 줄여 특화..
											aux::reverse_transform1_impl<P1, P2, front_inserter<typename clear<P1>::type> > >::type {};	// 하나줄여 특화..

		template<class P1 = na, class P2 = na, class P3 = na>
		struct reverse_transform1 : aux::reverse_transform1_impl<P1, P2, P3> {};
		
		template<class P1, class P2>
		struct reverse_transform1<P1, P2, na> : if_<	has_push_back<P1>,
													aux::reverse_transform1_impl<P1, P2, back_inserter< typename clear<P1>::type> >,		// 하나 줄여 특화..
													aux::transform1_impl<P1, P2, front_inserter<typename clear<P1>::type> > >::type {};	// 하나 줄여 특화..
		
	// inserter algorithm && reverse algorithm..

		template<class P1 = na, class P2 = na, class P3 = na, class P4 = na>		// 1,2,3,na
		struct transform2 : aux::transform2_impl<P1, P2, P3, P4> {};
		
		template<class P1, class P2, class P3>							// 하나 줄여 특화..
		struct transform2<P1, P2, P3, na> : if_<	has_push_back<typename clear<P1>::type>,			// clear는 P1 only
											aux::transform2_impl<P1, P2, P3, back_inserter<typename clear<P1>::type> >,		// 하나 줄여 특화..
											aux::reverse_transform2_impl<P1, P2, P3, front_inserter<typename clear<P1>::type> > >::type {};	// 하나줄여 특화..

		template<class P1 = na, class P2 = na, class P3 = na, class P4 = na>
		struct reverse_transform2 : aux::reverse_transform2_impl<P1, P2, P3, P4> {};
		
		template<class P1, class P2, class P3>
		struct reverse_transform2<P1, P2, P3, na> : if_<	has_push_back<P1>,
													aux::reverse_transform2_impl<P1, P2, P3, back_inserter< typename clear<P1>::type> >,		// 하나 줄여 특화..
													aux::transform2_impl<P1, P2, P3, front_inserter<typename clear<P1>::type> > >::type {};	// 하나 줄여 특화..
				

		template<class Seq1 = na, class Seq2OrOperation = na, class OperationOrInserter = na, class Inserter = na>                                                                   \
		struct transform                                                             
		{                                                                       
			typedef typename eval_if<                                           
				  or_<                                                          
					  is_na<OperationOrInserter>                                
					, is_lambda_expression< Seq2OrOperation >                   
					, not_< is_sequence<Seq2OrOperation> >                      
					>                                                           
				, transform1<Seq1,Seq2OrOperation,OperationOrInserter>             
				, transform2<Seq1,Seq2OrOperation,OperationOrInserter,Inserter>    
				>::type type;                                                   
		};          
		
		template<class Seq1 = na, class Seq2OrOperation = na, class OperationOrInserter = na, class Inserter = na>                                                                   \
		struct reverse_transform                                                             
		{                                                                       
			typedef typename eval_if<                                           
				  or_<                                                          
					  is_na<OperationOrInserter>                                
					, is_lambda_expression< Seq2OrOperation >                   
					, not_< is_sequence<Seq2OrOperation> >                      
					>                                                           
				, reverse_transform1<Seq1,Seq2OrOperation,OperationOrInserter>             
				, reverse_transform2<Seq1,Seq2OrOperation,OperationOrInserter,Inserter>    
				>::type type;                                                   
		};    
		
	}
}


#define na_spec (3, transform1)
#include "aux_/mpl_na_spec.h"

#define na_spec (3, reverse_transform1)
#include "aux_/mpl_na_spec.h"

#define na_spec (4, transform2)
#include "aux_/mpl_na_spec.h"

#define na_spec (4, reverse_transform2)
#include "aux_/mpl_na_spec.h"

#define na_spec (4, transform)
#include "aux_/mpl_na_spec.h"

#define na_spec (4, reverse_transform)
#include "aux_/mpl_na_spec.h"
