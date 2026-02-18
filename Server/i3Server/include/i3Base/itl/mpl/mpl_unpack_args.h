#if !defined(pp_is_iterating)

#ifndef mpl_unpack_args_h
#define mpl_unpack_args_h

#include "mpl_apply.h"
#include "mpl_at.h"
#include "mpl_size.h"
#include "mpl_limit.h"

namespace i3
{
	namespace mpl
	{
		#define aux_unpack(unused, i, Args) , typename at_c<Args,i>::type
		
		namespace aux
		{
			template< int size, class F, class Args >
			struct unpack_args_impl;
		}
	}
}

#define pp_iteration_params_1 (3, (0, mpl_arity_limit, "mpl_unpack_args.h"))
#include pp_iterate()

namespace i3
{
	namespace mpl
	{
		template<class F>
		struct unpack_args
		{
			template< class Args > 
			struct apply : aux::unpack_args_impl< size<Args>::value,F,Args > {};
		};
	}
}

#define lambda_spec (1, unpack_args)
#include "aux_/mpl_lambda_spec.h"
#undef  aux_unpack

#endif

#else // pp_is_iterating

#define i_	pp_frame_iteration(1)

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< class F, class Args >
			struct unpack_args_impl<i_,F,Args> : pp_cat(apply,i_)< F pp_repeat(i_, aux_unpack, Args) >
			{
			};


		}
	}
}

#undef i_

#endif
