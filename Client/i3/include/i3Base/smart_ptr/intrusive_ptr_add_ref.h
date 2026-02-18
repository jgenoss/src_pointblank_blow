#pragma once

//
//  디폴트 구현 아래에...
//

namespace i3
{
	template<class, class> class ref_count_template;
	template<class, class> class static_ref_count;

//	template<class T> void	intrusive_ptr_add_ref(T*);			// 이 구현을 비운다...(링크에러)
	
	template<class A1,class A2> __forceinline
	void intrusive_ptr_add_ref(ref_count_template<A1, A2>* p) { p->add_ref(); }

	template<class A1, class A2> __forceinline
	void intrusive_ptr_add_ref(static_ref_count<A1, A2>* p) { p->add_ref(); }
	
}

