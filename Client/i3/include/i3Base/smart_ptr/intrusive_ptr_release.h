#pragma once


namespace i3
{
	
//	template<class T> void	intrusive_ptr_release(T*);			// 이 구현을 비운다...(링크에러)
	
	template<class A1,class A2> __forceinline
	void intrusive_ptr_release(ref_count_template<A1, A2>* p) { p->release(); }

	template<class A1, class A2> __forceinline
	void intrusive_ptr_release(static_ref_count<A1, A2>* p) { p->release(); }
	
}

