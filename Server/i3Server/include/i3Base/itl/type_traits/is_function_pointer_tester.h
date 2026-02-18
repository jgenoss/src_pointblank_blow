
#if !defined(pp_is_iterating)

#ifndef is_function_pointer_tester_h
#define is_function_pointer_tester_h 

#include "yes_no_type.h"
#include "integral_constant.h"

namespace i3
{
	namespace detail
	{
		no_type __cdecl is_function_pointer_tester(...);
	}
}


#include "../preprocessor/pp_iterate.h"
#define pp_iteration_params_1 (3, (0, 25, "is_function_pointer_tester.h") )
#include pp_iterate()

namespace i3
{
	namespace detail
	{
		template <class T>
		struct is_function_impl
		{
			static T* t;
			typedef integral_constant<bool, ( sizeof(is_function_pointer_tester(t)) == sizeof(yes_type) ) > type;
		};
		
		template <class T>
		struct is_function_impl<T&> : false_type {};
	
	}
}

#endif // is_function_pointer_tester_h

#else // pp_is_iterating

#define i_		pp_frame_iteration(1)

namespace i3
{
	namespace detail
	{
		template <class R pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_function_pointer_tester(R (*)(pp_enum_params(i_,T)));

		template <class R pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_function_pointer_tester(R (*)(pp_enum_params(i_,T) ...));


		#ifdef _MSC_EXTENSIONS

		template <class R pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_function_pointer_tester(R (__stdcall*)(pp_enum_params(i_,T)));
		template <class R pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_function_pointer_tester(R (__stdcall*)(pp_enum_params(i_,T) ...));

		template <class R pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_function_pointer_tester(R (__fastcall*)(pp_enum_params(i_,T)));
		template <class R pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_function_pointer_tester(R (__fastcall*)(pp_enum_params(i_,T) ...));

		template <class R pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_function_pointer_tester(R (__cdecl*)(pp_enum_params(i_,T)));
		template <class R pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_function_pointer_tester(R (__cdecl*)(pp_enum_params(i_,T) ...));

		#endif

	}
}


#undef i_
#endif // pp_is_iterating
