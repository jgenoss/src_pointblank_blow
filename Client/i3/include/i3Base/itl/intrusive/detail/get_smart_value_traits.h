#pragma once

#include "../../type_traits/pointer_traits.h"

namespace i3
{
	namespace intrusive
	{
		template<class T, class NodeTraits, class Tag>
		struct smart_bhtraits;
		template<class T, class Hook, class Elem, Hook Elem::* PtoM>
		struct smart_mhtraits;

		struct member_tag;

		template<class T, class Hook>
		struct get_smart_base_value_traits
		{
			typedef typename Hook::node_traits	node_traits;
			typedef typename Hook::tag			tag;
			typedef smart_bhtraits<T, node_traits, tag>  type;
		};
		
		template<class T, class ProtoMemberHook>
		struct get_smart_member_value_traits;
		
		// 부분특화 사용..
		template<class T, class C, class MemberHook, MemberHook C::* PtoM >
		struct get_smart_member_value_traits<T, smart_member_hook<C, MemberHook, PtoM> >
		{
			typedef smart_mhtraits<T, MemberHook, C, PtoM>	type;
		};


		template<class T, class Hook>
		struct get_smart_value_traits
		{
			typedef typename i3::pointer_traits<T>::element_type	element_type;

			typedef typename mpl::eval_if< is_convertible<Hook*, default_hook_tag*>,
											mpl::apply<Hook, element_type>, 
											mpl::identity<Hook> >::type hook_type;
			
			typedef typename	hook_type::tag		tag;

			typedef typename mpl::eval_if< is_same<tag, member_tag>,
											get_smart_member_value_traits<T, hook_type>,
											get_smart_base_value_traits<T, hook_type> >::type type;
		};

		
	}

}