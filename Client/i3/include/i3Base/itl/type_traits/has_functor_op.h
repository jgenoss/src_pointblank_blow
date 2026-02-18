#pragma once

// 원본 출처는 데이브아브라함스가 작성한 메일내용-> 원본은 러시아의 개발블로그...
// http://groups.google.com/group/comp.lang.c++.moderated/msg/e5fbc9305539f699

#pragma warning(disable : 4510 4610)

namespace i3
{
	namespace detail
	{
		template<class T>
		struct has_functor_op_impl
		{
			struct base_mixin { void operator()(){} };
			struct functor_wrap : public T, public base_mixin {};
			
			template<class U, U t> class helper;
			
			template<class U> 
			static no_type test(U*, helper<void (base_mixin::*)(), &U::operator()>* = 0);
			static yes_type test(...);
			static const bool value = (sizeof(yes_type) == sizeof(test(static_cast<functor_wrap*>(0))));
			typedef mpl::bool_<value>	type;
		};
		
	}

	template<class T>
	struct has_functor_op : mpl::and_<is_class_or_union<T>, detail::has_functor_op_impl<T> > {};
}


#pragma warning(default : 4510 4610)
