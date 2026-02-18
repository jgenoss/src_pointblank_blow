#pragma once


namespace i3
{
	namespace intrusive
	{
////////////////////
		template<class T, class Tag = mpl::na>
		struct shared_list_base_hook;
		template<class T, class Tag = mpl::na>
		struct intrusive_list_base_hook;
		template<class T, class Tag = mpl::na>	// raw pointer 용도..
		struct raw_list_base_hook;

////////////////////
		template<class T>
		struct shared_list_member_hook;
		template<class T>
		struct intrusive_list_member_hook;
		template<class T>						// raw pointer용도
		struct raw_list_member_hook;	
/////////////////////
		template<class BaseHook>
		struct smart_base_hook;

		template<class C, class Hook, Hook C::* PtrToMem>
		struct smart_member_hook;

		template<bool Enable>
		struct constant_time_size;

		template<class T, class O1 = mpl::na, class O2 = mpl::na>
		class shared_list;

		template<class T, class O1 = mpl::na, class O2 = mpl::na>
		class intrusive_list;
		
		template<class T, class O1 = mpl::na, class O2 = mpl::na>
		class raw_list;

	}

}
