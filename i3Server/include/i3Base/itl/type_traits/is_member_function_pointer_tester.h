
#if !defined(pp_is_iterating)

#ifndef is_member_function_pointer_tester_h
#define is_member_function_pointer_tester_h 

#include "../mpl/mpl_always.h"
#include "is_array.h"
#include "yes_no_type.h"
#include "integral_constant.h"

#include "../preprocessor/repetition/pp_enum_params.h"

namespace i3
{
	namespace detail
	{
		no_type __cdecl is_member_function_pointer_tester(...);

	}
}

#include "../preprocessor/pp_iterate.h"
#define pp_iteration_params_1 ( 3, (0, 25, "is_member_function_pointer_tester.h") )
#include pp_iterate()

namespace i3
{
	namespace detail
	{
		template<bool Is_Array>
		struct	is_member_function_pointer_select : mpl::always<false_type> {};
	
		template<>
		struct is_member_function_pointer_select<false>
		{
			template<class T>
			struct apply 
			{
				static T* make_t;
				typedef integral_constant<bool, (sizeof(yes_type) == sizeof(detail::is_member_function_pointer_tester(make_t) ) ) > type;
			};
		};
		
		template <class T>
		struct is_member_function_pointer_impl : 
		is_member_function_pointer_select< is_array<T>::value >::template apply<T>::type {};

		template <class T> 	struct is_member_function_pointer_impl<T&> : false_type{};
		template<> 			struct is_member_function_pointer_impl<void> : false_type {};
		template<>			struct is_member_function_pointer_impl<void const> : false_type {};
		template<>			struct is_member_function_pointer_impl<void volatile> : false_type {};
		template<>			struct is_member_function_pointer_impl<void const volatile> : false_type {};
	}	
}


#endif // is_member_function_pointer_tester_impl_h

#else // pp_is_iterating

#define i_		pp_frame_iteration(1)


namespace i3
{
	namespace detail
	{
			
		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (T::*const volatile*)(pp_enum_params(i_,T)));

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (T::*const volatile*)(pp_enum_params(i_,T)) const);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (T::*const volatile*)(pp_enum_params(i_,T)) volatile);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (T::*const volatile*)(pp_enum_params(i_,T)) const volatile);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (T::*const volatile*)(pp_enum_params(i_,T) ...));

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (T::*const volatile*)(pp_enum_params(i_,T) ...) const);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (T::*const volatile*)(pp_enum_params(i_,T) ...) volatile);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (T::*const volatile*)(pp_enum_params(i_,T) ...) const volatile);
	
		#ifdef _MSC_EXTENSIONS // Other calling conventions used by MS compatible compilers:
		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__stdcall T::*const volatile*)(pp_enum_params(i_,T)));

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__stdcall T::*const volatile*)(pp_enum_params(i_,T)) const);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__stdcall T::*const volatile*)(pp_enum_params(i_,T)) volatile);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__stdcall T::*const volatile*)(pp_enum_params(i_,T)) const volatile);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__stdcall T::*const volatile*)(pp_enum_params(i_,T) ...));

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__stdcall T::*const volatile*)(pp_enum_params(i_,T) ...) const);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__stdcall T::*const volatile*)(pp_enum_params(i_,T) ...) volatile);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__stdcall T::*const volatile*)(pp_enum_params(i_,T) ...) const volatile);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__fastcall T::*const volatile*)(pp_enum_params(i_,T)));

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__fastcall T::*const volatile*)(pp_enum_params(i_,T)) const);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__fastcall T::*const volatile*)(pp_enum_params(i_,T)) volatile);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__fastcall T::*const volatile*)(pp_enum_params(i_,T)) const volatile);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__fastcall T::*const volatile*)(pp_enum_params(i_,T) ...));

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__fastcall T::*const volatile*)(pp_enum_params(i_,T) ...) const);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__fastcall T::*const volatile*)(pp_enum_params(i_,T) ...) volatile);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__fastcall T::*const volatile*)(pp_enum_params(i_,T) ...) const volatile);
	
		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__cdecl T::*const volatile*)(pp_enum_params(i_,T)));

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__cdecl T::*const volatile*)(pp_enum_params(i_,T)) const);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__cdecl T::*const volatile*)(pp_enum_params(i_,T)) volatile);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__cdecl T::*const volatile*)(pp_enum_params(i_,T)) const volatile);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__cdecl T::*const volatile*)(pp_enum_params(i_,T) ...));

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__cdecl T::*const volatile*)(pp_enum_params(i_,T) ...) const);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__cdecl T::*const volatile*)(pp_enum_params(i_,T) ...) volatile);

		template <class R, class T pp_comma_if(i_) pp_enum_params(i_,class T) >
		yes_type is_member_function_pointer_tester(R (__cdecl T::*const volatile*)(pp_enum_params(i_,T) ...) const volatile);
		#endif

	}
}

#undef i_
#endif // pp_is_iterating
