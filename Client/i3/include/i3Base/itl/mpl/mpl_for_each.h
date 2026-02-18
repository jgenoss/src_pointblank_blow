#pragma once

#include "mpl_is_sequence.h"
#include "mpl_begin_end.h"
#include "mpl_apply.h"
#include "mpl_bool.h"
#include "mpl_next_prior.h"
#include "mpl_deref.h"
#include "mpl_identity.h"
#include "aux_/mpl_unwrap.h"

#include "../value_init.h"


namespace i3 
{ 
	namespace mpl 
	{
		namespace aux 
		{

			template< bool done = true >
			struct for_each_impl
			{
				template<class Iterator, class LastIterator, class TransformFunc, class F>
				static void execute(Iterator*, LastIterator*, TransformFunc*, F){}
			};

			template<>
			struct for_each_impl<false>
			{
				template<class Iterator, class LastIterator, class TransformFunc, class F>
				static void execute(Iterator*, LastIterator*, TransformFunc*, F f)
				{
					typedef typename deref<Iterator>::type item;
					typedef typename apply1<TransformFunc,item>::type arg;
			    
					// dwa 2002/9/10 -- make sure not to invoke undefined behavior when we pass arg.
					value_initialized<arg> x;
					aux::unwrap(f, 0)(i3::get(x));
			        
					typedef typename mpl::next<Iterator>::type iter;
					for_each_impl<i3::is_same<iter,LastIterator>::value>
						::execute( static_cast<iter*>(0), static_cast<LastIterator*>(0), static_cast<TransformFunc*>(0), f);
				}
			};
		} 
		
		template<class Seq, class TransformOp, class F> inline
		void for_each(F f, Seq* = 0, TransformOp* = 0)
		{
			compile_assert_pred(( is_sequence<Seq> ));

			typedef typename begin<Seq>::type first;
			typedef typename end<Seq>::type last;

			aux::for_each_impl< i3::is_same<first,last>::value >
			::execute(static_cast<first*>(0), static_cast<last*>(0), static_cast<TransformOp*>(0), f);
		}

		template<class Seq, class F> inline
		void for_each(F f, Seq* = 0) {	for_each<Seq, identity<> >(f); }

	}
}
