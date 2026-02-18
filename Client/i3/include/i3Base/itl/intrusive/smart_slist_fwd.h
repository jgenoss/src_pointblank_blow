#pragma once

namespace i3
{
	namespace intrusive
	{

		////////////////////
		template<class T, class Tag = mpl::na>
		struct shared_slist_base_hook;
		template<class T, class Tag = mpl::na>
		struct intrusive_slist_base_hook;
		template<class T, class Tag = mpl::na>	// raw pointer 용도..
		struct raw_slist_base_hook;

		////////////////////
		template<class T>
		struct shared_slist_member_hook;
		template<class T>
		struct intrusive_slist_member_hook;
		template<class T>						// raw pointer용도
		struct raw_slist_member_hook;	

		/////////////////////
		template<class BaseHook>
		struct smart_base_hook;

		template<class C, class Hook, Hook C::* PtrToMem>
		struct smart_member_hook;

		template<bool Enable>
		struct constant_time_size;
		
		template<bool Enable>
		struct linear;

		template<bool Enable>
		struct cache_last;

		template<class T, class O1 = mpl::na, class O2 = mpl::na, 
				class O3 = mpl::na, class O4 = mpl::na>
		class shared_slist;

		template<class T, class O1 = mpl::na, class O2 = mpl::na, 
				class O3 = mpl::na, class O4 = mpl::na>
		class intrusive_slist;
		
		template<class T, class O1 = mpl::na, class O2 = mpl::na, 
		class O3 = mpl::na, class O4 = mpl::na>
		class raw_slist;

	}
}