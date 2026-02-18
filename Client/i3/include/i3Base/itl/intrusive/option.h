#pragma once

namespace i3
{

	namespace intrusive
	{
		struct hook_policy_tag;

		template<class C, class Hook, Hook C::* PtrToMem>
		struct smart_member_hook
		{
			typedef hook_policy_tag		policy_tag; 
			typedef typename Hook::tag	tag;

			typedef smart_member_hook<C, Hook, PtrToMem>	type;
		};

		template<class BaseHook>
		struct smart_base_hook
		{
			typedef hook_policy_tag		policy_tag;
			typedef BaseHook			type;
		};
		
		struct constant_time_size_policy_tag;

		template<bool Enable>
		struct constant_time_size 
		{
			typedef constant_time_size_policy_tag			policy_tag;
			typedef i3::integral_constant<bool, Enable>		type;
		};

		struct linear_policy_tag;

		template<bool Enable>
		struct linear
		{
			typedef linear_policy_tag		policy_tag;
			typedef i3::integral_constant<bool, Enable>	type;
		};

		struct cache_last_policy_tag;

		template<bool Enable>
		struct cache_last
		{
			typedef cache_last_policy_tag	policy_tag;
			typedef i3::integral_constant<bool, Enable> type;
		};
		
		
	}

}