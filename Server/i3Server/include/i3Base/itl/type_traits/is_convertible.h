#pragma once

#include "yes_no_type.h"
#include "is_array.h"
#include "add_reference.h"
#include "is_arithmetic.h"
#include "is_void.h"
#include "is_abstract.h"
#include "integral_constant.h"

#include "../mpl/mpl_logical.h"

#pragma warning(push)
#pragma warning(disable : 4244)			// 형변환 가능여부만 조사하는 것이므로, 형변환시 경고는 무시한다..

namespace i3
{
	
	namespace detail
	{

		template <class From, class To>
		struct is_convertible_basic_impl
		{
		private:
			static no_type 	Test(...);	static yes_type	Test(To);		static From		MakeFrom();
		public:
			typedef integral_constant<bool, ( sizeof(yes_type) == sizeof( Test( MakeFrom() ) ) )> type;
		};
		
		template<class From, class To>
		struct is_convertible_impl 
		{
			typedef typename i3::add_reference<From>::type	ref_type;
			static const bool value = mpl::and_< mpl::or_< is_convertible_basic_impl<ref_type, To>, is_void<To> >, mpl::not_< is_array<To> > >::value;
		};
				
		template<bool trivial1, bool trivial2, bool abstract_target>
		struct is_convertible_impl_select{ template<class From, class To> struct rebind { typedef is_convertible_impl<From, To> type; }; };
		
		template<>
		struct is_convertible_impl_select<true, true, false> { template<class From, class To> struct rebind { typedef true_type type; }; };
		
		template<>
		struct is_convertible_impl_select<false, false, true>{ template<class From, class To> struct rebind { typedef false_type type; }; };
		
		template<>
		struct is_convertible_impl_select<true, false, true>{ template<class From, class To> struct rebind { typedef false_type	type; }; };
		
		template<class From, class To>
		struct is_convertable_impl_dispatch
		{
			typedef is_convertible_impl_select< is_arithmetic<From>::value, is_arithmetic<To>::value, is_abstract<To>::value> selector;
			typedef typename selector::template rebind<From, To> binder;
			typedef typename binder::type						 type;
		};

	}
	
	template<class From, class To>
	struct is_convertible : integral_constant<bool, detail::is_convertable_impl_dispatch<From, To>::type::value> {};

//  아래는 void타입의 특화로 다소 많고 지저분함..

	template <>			struct is_convertible<void, void> : true_type {};
	template <>			struct is_convertible<void, void const> : true_type {};
	template <>			struct is_convertible<void, void volatile> : true_type {};
	template <>			struct is_convertible<void, void const volatile> : true_type {};

	template <>			struct is_convertible<void const, void> : true_type {};
	template <>			struct is_convertible<void const, void const> : true_type {};
	template <>			struct is_convertible<void const, void volatile> : true_type {};
	template <>			struct is_convertible<void const, void const volatile> : true_type {};
	
	template <>			struct is_convertible<void volatile, void> : true_type {};
	template <>			struct is_convertible<void volatile, void const> : true_type {};
	template <>			struct is_convertible<void volatile, void volatile> : true_type {};
	template <>			struct is_convertible<void volatile, void const volatile> : true_type {};

	template <>			struct is_convertible<void const volatile, void> : true_type {};
	template <>			struct is_convertible<void const volatile, void const> : true_type {};
	template <>			struct is_convertible<void const volatile, void volatile> : true_type {};
	template <>			struct is_convertible<void const volatile, void const volatile> : true_type {};

	template<class To>	struct is_convertible<void, To> : false_type {}; 
	template<class To>	struct is_convertible<void const, To> : false_type {}; 
	template<class To>	struct is_convertible<void volatile, To> : false_type {}; 
	template<class To>	struct is_convertible<void const volatile, To> : false_type {}; 

	template<class From> struct is_convertible<From, void> : true_type {};	// 엇...이것 true인것은 로키와 다름..(loki => false)
	template<class From> struct is_convertible<From, void const> : true_type {};	
	template<class From> struct is_convertible<From, void volatile> : true_type {};
	template<class From> struct is_convertible<From, void const volatile> : true_type {};
		
}

#pragma warning(pop)