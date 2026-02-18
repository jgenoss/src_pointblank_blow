#if !defined(pp_is_iterating)

#ifndef mpl_inherit_h
#define mpl_inherit_h


#include "mpl_empty_base.h"
#include "mpl_na.h"
#include "mpl_limit.h"

namespace i3
{
	namespace mpl
	{
		template<class T1 = na, class T2 = na> 
		struct inherit2 : T1, T2
		{
			typedef inherit2 type;
		};

		template< class T1 > 
		struct inherit2<T1,empty_base>
		{
			typedef T1 type;
		};

		template< class T2 > 
		struct inherit2<empty_base,T2>
		{
			typedef T2 type;
		};

		template<> 
		struct inherit2<empty_base,empty_base>
		{
			typedef empty_base type;
 		};
	}
}

#define na_spec (2, inherit2)
#include "aux_/mpl_na_spec.h"

#define pp_iteration_params_1 (3,(3, mpl_arity_limit, "mpl_inherit.h"))
#include pp_iterate()

namespace i3
{
	namespace mpl
	{
		template< pp_enum_params_1_with_a_default(mpl_arity_limit, class T, empty_base) >
		struct inherit : pp_cat(inherit, mpl_arity_limit)< pp_enum_params_1(mpl_arity_limit, T) >
		{
		};

		template<>
		struct inherit< pp_enum_datas(mpl_arity_limit, na) >
		{
			template< pp_enum_params_1_with_a_default(mpl_arity_limit, class T, empty_base) >
			struct apply : inherit< pp_enum_params_1(mpl_arity_limit, T) > {};
		};
	}
}

#define na_spec (mpl_arity_limit, inherit)
#include "aux_/mpl_na_spec_lambda.h"

#endif

#else // pp_is_iterating

#define n_ pp_frame_iteration(1)

namespace i3
{
	namespace mpl
	{
		template< pp_enum_params_1_with_a_default(n_, class T, na) >
		struct pp_cat(inherit,n_) : inherit2< typename pp_cat(inherit,pp_dec(n_))< pp_enum_params_1(pp_dec(n_), T)>::type, pp_cat(T, n_) >
		{
		};
	}
}

#define na_spec (n_, pp_cat(inherit,n_))
#include "aux_/mpl_na_spec.h"

#undef n_
#endif  // pp_is_iterating
