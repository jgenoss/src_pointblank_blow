
#if !defined(pp_is_iterating)

#ifndef is_member_function_pointer_helper_h
#define is_member_function_pointer_helper_h 

#include "remove_cv.h"
#include "integral_constant.h"

namespace i3
{
	namespace detail
	{
		template<class T>
		struct is_member_function_pointer_helper {	static const bool value = false; };
	}
}

#include "../preprocessor/pp_iterate.h"
#define pp_iteration_params_1 (3, (0, 25, "is_member_function_pointer_helper.h") )
#include pp_iterate()

namespace i3
{
	namespace detail
	{
		template<class T> 
		struct is_member_function_pointer_impl : integral_constant<bool, detail::is_member_function_pointer_helper<typename remove_cv<T>::type>::value> {};
	}
}


#endif // is_member_function_pointer_helper_h

#else // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
				
		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T)>
		struct is_member_function_pointer_helper<R (T::*)(pp_enum_params(i_,T)) > { static const bool value = true; };
		
		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T)>
		struct is_member_function_pointer_helper<R (T::*)(pp_enum_params(i_,T) ...) > { static const bool value = true; };
		

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T)>
		struct is_member_function_pointer_helper<R (T::*)(pp_enum_params(i_,T)) const > { static const bool value = true;};

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T)>
		struct is_member_function_pointer_helper<R (T::*)(pp_enum_params(i_,T)) volatile > { static const bool value = true; };

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T)>
		struct is_member_function_pointer_helper<R (T::*)(pp_enum_params(i_,T)) const volatile > { static const bool value = true; };


		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T)>
		struct is_member_function_pointer_helper<R (T::*)(pp_enum_params(i_,T) ...) const > { static const bool value = true; };

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T)>
		struct is_member_function_pointer_helper<R (T::*)(pp_enum_params(i_,T) ...) volatile > { static const bool value = true; };

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T)>
		struct is_member_function_pointer_helper<R (T::*)(pp_enum_params(i_,T) ...) const volatile > { static const bool value = true; };
	}
}

#undef i_
#endif // pp_is_iterating