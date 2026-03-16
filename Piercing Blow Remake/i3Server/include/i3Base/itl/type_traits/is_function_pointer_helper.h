
#if !defined(pp_is_iterating)

#ifndef is_function_pointer_helper_h
#define is_function_pointer_helper_h 

#include "integral_constant.h"
#include "is_reference.h"

namespace i3
{
	namespace detail
	{
		template <class R>
		struct is_function_pointer_helper {	static const bool value = false;  };
	}
}

#include "../preprocessor/pp_iterate.h"
#define pp_iteration_params_1 (3, (0, 25, "is_function_pointer_helper.h") )
#include pp_iterate()


namespace i3
{
	namespace detail
	{
		template<bool is_ref = true>
		struct is_function_chooser 
		{
			template<class T> struct apply : false_type {};
		};
		template <>
		struct is_function_chooser<false>
		{
			template<class T > 
			struct apply : integral_constant<bool, is_function_pointer_helper<T*>::value> {};
		};
		template <class T>
		struct is_function_impl : detail::is_function_chooser<is_reference<T>::value>::template apply<T> {};
	

	}
}



#endif // is_function_pointer_helper_h

#else // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		template <class R pp_comma_if(i_) pp_enum_params(i_,class T)>
		struct is_function_pointer_helper<R (*)(pp_enum_params(i_,T))> { static const bool value = true; };

		template <class R pp_comma_if(i_) pp_enum_params(i_,class T)>
		struct is_function_pointer_helper<R (*)(pp_enum_params(i_,T) ...)> { static const bool value = true; };
	}
}

#undef i_
#endif // pp_is_iterating
