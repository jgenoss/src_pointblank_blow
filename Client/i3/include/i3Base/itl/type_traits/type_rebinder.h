#pragma once

#include "yes_no_type.h"
#include "has_other.h"
#include "has_type.h"
#include "template_replace_at.h"

namespace i3
{
	namespace detail_type_rebinder
	{
		template<class T>
		struct low_priority_conversion { low_priority_conversion(const T&){} };
		
		template<class Ctn, class U>
		struct has_rebind
		{
			template<class X>
			static yes_type test(int, typename X::template rebind<U>*);
			template<class X>
			static no_type  test(low_priority_conversion<int>, void*);
			static const bool value = ( sizeof(test<Ctn>(0,0)) == sizeof(yes_type) );
		};
		
		template<class RebindT>
		struct get_rebound_if_has_other {	typedef typename RebindT::other type;  };
		template<class RebindT>
		struct get_rebound_if_has_type {    typedef typename RebindT::type  type;  };
	
		template<class Ctn, class U>
		struct get_rebound_if_has_rebind
		{
			typedef  typename Ctn::template rebind<U>		rebind_type;

			typedef  typename mpl::multi_eval_if<
				has_other<rebind_type>, get_rebound_if_has_other<rebind_type>,
				has_type<rebind_type>,	get_rebound_if_has_type<rebind_type>,
										i3::identity<rebind_type> >::type	type;
		};

		template<class Ctn, class U>
		struct get_rebound_if_template
		{
			typedef typename template_replace_at_c<Ctn, 0, U>::type		type;
		};
	}

	template<class Ctn, class U>
	struct type_rebinder
	{
		typedef typename mpl::eval_if< detail_type_rebinder::has_rebind<Ctn, U>,
							detail_type_rebinder::get_rebound_if_has_rebind<Ctn, U>,
							detail_type_rebinder::get_rebound_if_template<Ctn, U> >::type type;
	};
}

