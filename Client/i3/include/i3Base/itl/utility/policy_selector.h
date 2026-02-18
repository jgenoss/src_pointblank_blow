#pragma once

#include "../type_traits/has_tag.h"
#include "../type_traits/is_base_and_derived.h"
#include "../mpl/mpl_find_if.h"
#include "../preprocessor/pp_enum_params.h"
#include "../mpl/mpl_vector.h"

namespace i3
{
	template<class T> struct has_policy_tag;
}

#define has_xxx		(has_policy_tag, policy_tag)
#include "../type_traits/has_xxx.h"


#define policy_selector_limit 10

namespace i3
{
	// 1. 디폴트 policy_collector가 필요
	// 2. policy_selector가 타입 이름을 통해 타입을 취득할수 있도록..
	//    인터페이스를 가져야됨...
	//
	namespace detail
	{
		template<class TargetTag, class T>
		struct has_target_tag_impl : is_same< typename T::tag, TargetTag> {};
		
		template<class TargetTag, class T>
		struct has_target_policy_tag_impl : is_same< typename T::policy_tag, TargetTag> {};

		template<class Seq, class TargetTag, class DefaultPolicy, 
				template<class> class has_template, 
				template<class, class> class has_target_template>
		struct find_with_tag_impl
		{
			typedef mpl::and_< has_template<mpl::_1>, has_target_template<TargetTag, mpl::_1>  >   Pred;
			typedef typename mpl::find_if<Seq, Pred>::type   result_iterator;

			typedef typename mpl::eval_if<	is_same<result_iterator, typename mpl::end<Seq>::type >, 
											mpl::identity<DefaultPolicy>, 
											mpl::deref<result_iterator> >::type type;
		};
		
		template<class Seq, class TargetBase, class DefaultPolicy>
		struct find_with_base_impl
		{
			typedef typename mpl::find_if<Seq, i3::is_base_and_derived<TargetBase, mpl::_1> >::type  result_iterator;
			
			typedef typename mpl::eval_if<	is_same<result_iterator, typename mpl::end<Seq>::type >, 
				mpl::identity<DefaultPolicy>, 
				mpl::deref<result_iterator> >::type type;
			
		};

	}


	template<pp_enum_params_with_a_default(policy_selector_limit, class Policy, mpl::na)>
	struct policy_selector
	{
		typedef typename mpl::vector<pp_enum_params(policy_selector_limit, Policy)>::type  sequence_type;

		template<class TargetTag, class DefaultPolicy>
		struct find_with_tag
		{
			typedef typename detail::find_with_tag_impl<sequence_type, TargetTag, DefaultPolicy, 
				has_tag, detail::has_target_tag_impl>::type   type;
		};

		template<class TargetTag, class DefaultPolicy>
		struct find_with_policy_tag
		{
			typedef typename detail::find_with_tag_impl<sequence_type, TargetTag, DefaultPolicy, 
				has_policy_tag, detail::has_target_policy_tag_impl>::type   type;
		};
		
		template<class TargetBase, class DefaultPolicy>
		struct find_with_base
		{
			typedef typename detail::find_with_base_impl<sequence_type, TargetBase, DefaultPolicy>::type type;
		};
	};
	
	
	
	
	
	
}