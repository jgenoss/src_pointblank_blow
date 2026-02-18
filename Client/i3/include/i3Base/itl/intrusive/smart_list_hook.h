#pragma once

#include "detail/smart_generic_hook.h"
#include "detail/smart_list_node_traits.h"
#include "../../smart_ptr/shared_ptr_fwd.h"
#include "../../smart_ptr/intrusive_ptr_fwd.h"

namespace i3
{
	namespace intrusive
	{
		struct default_smart_list_base_tag;

		template<class SmartPtr, class Tag = mpl::na>
		struct make_smart_list_base_hook
		{
			typedef typename mpl::if_< is_same<Tag, mpl::na>, default_smart_list_base_tag, Tag>::type	tag;

			typedef smart_generic_hook<  list_algorithm< smart_list_node_traits<SmartPtr> >, tag>		type; 
		};
		
		template<class SmartPtr>
		struct make_smart_list_member_hook
		{
			typedef smart_generic_hook< list_algorithm< smart_list_node_traits<SmartPtr> >, member_tag>	type;
		};
		
		template<class T, class Tag>
		struct shared_list_base_hook : make_smart_list_base_hook< i3::shared_ptr<T>, Tag>::type {};
		
		template<class T, class Tag>
		struct intrusive_list_base_hook : make_smart_list_base_hook< i3::intrusive_ptr<T>, Tag>::type {};

		template<class T>
		struct shared_list_member_hook : make_smart_list_member_hook< i3::shared_ptr<T> >::type {};

		template<class T>
		struct intrusive_list_member_hook : make_smart_list_member_hook< i3::intrusive_ptr<T> >::type {};


		template<class T, class Tag>
		struct raw_list_base_hook : make_smart_list_base_hook< T*, Tag>::type {};

		template<class T>
		struct raw_list_member_hook : make_smart_list_member_hook< T* >::type {};


	}

}