#if !defined(pp_is_iterating)

#ifndef functor_base_h
#define functor_base_h

namespace i3
{
	namespace detail
	{
		template<class R>
		struct functor_root
		{
			typedef functor_root<R>		my_type;
			typedef functor_root<R>		root_type;

			virtual ~functor_root() {}
			virtual bool     operator==(const my_type&) const = 0;
			virtual const std::type_info&	target_type() const = 0;
			virtual const void*				target(const std::type_info& info) const = 0;
			virtual void					destroy_self() const = 0;	 
		};
		
		template<class R, class List>
		struct functor_base;

	}
}

#include "../../mpl/mpl_list_n.h"
#include "../../function_types/ft_arity_limit.h"
#include "../../preprocessor/repetition/pp_enum_trailing_params.h"

#define pp_iteration_params_1 (3, (0, ft_arity_limit, "functor_base.h") )
#include pp_iterate()

#endif // functor_base_h

#else // pp_is_iterating

#define i_			pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		template<class R pp_enum_trailing_params(i_, class P)>					// P는 인자화된것으로 가정..
		struct functor_base<R, pp_cat(mpl::list,i_)<pp_enum_params(i_, P)> > : public functor_root<R>
		{
			virtual functor_base* clone() const = 0;
			virtual R operator()( pp_enum_params(i_, P) ) const = 0;
		};
	}
}

#undef i_

#endif // pp_is_iterating

