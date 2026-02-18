#pragma once

namespace i3
{
	namespace intrusive
	{
		struct default_hook_tag {};
		
		struct default_smart_list_hook : default_hook_tag
		{
			template<class T>
			struct apply {  typedef typename T::default_smart_list_hook type; };
		};

		struct default_smart_slist_hook : default_hook_tag
		{
			template<class T>
			struct apply { typedef typename T::default_smart_slist_hook type; };
		};


	}

}
