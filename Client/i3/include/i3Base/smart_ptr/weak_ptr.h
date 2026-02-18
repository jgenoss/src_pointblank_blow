#pragma once

#include "shared_ptr.h"

namespace i3
{

	template<class T>
	class weak_ptr
	{
	public:
		typedef weak_ptr<T>		this_type;
		typedef T				element_type;
		static const bool		is_mt = type_enable_shared_mt<T>::value;

		weak_ptr() : px(0), pn() {}
		
		template<class U>
		weak_ptr(const weak_ptr<U>& rhs, typename enable_if< is_convertible<U*, T*>, int>::type = 0) : px(rhs.lock().get()), pn(rhs.pn) {}
		
		template<class U>
		weak_ptr(const shared_ptr<U>& rhs, typename enable_if< is_convertible<U*, T*>, int>::type = 0) : px(rhs.px), pn(rhs.pn) {}
		
		template<class U>
		weak_ptr& operator=(const weak_ptr<U>& rhs) {	px = rhs.lock().get();	pn = rhs.pn; return *this; 	}
		
		template<class U>
		weak_ptr& operator=(const shared_ptr<U>& rhs) { px = rhs.px; pn = rhs.pn; return *this; }
		
		shared_ptr<T>	lock() const { return shared_ptr<element_type>(*this); }
		long			use_count() const { return pn.use_count(); }
		bool			expired() const { return pn.use_count() == 0; }
		bool			empty() const  { return pn.empty(); }
		void			reset() { this_type().swap(*this); }
		void			swap(this_type& rhs) { i3::swap(px, rhs.px); pn.swap(rhs.pn); }
		
		void			internal_assign(T* px2, const detail::shared_count<is_mt>& pn2) { px = px2; pn = pn2; }
		template<class U>
		bool			internal_less(const weak_ptr<U>& rhs) const { return pn < rhs.pn; }
		
	private:
		T*								px;
		detail::weak_count<is_mt>		pn;

		template<class> friend class weak_ptr;
		template<class> friend class shared_ptr;
	};
	
	template<class T, class U> inline 
	bool operator<( const weak_ptr<T>& a, const weak_ptr<U>& b) { return a.internal_less(b); }

	template<class T> inline
	void swap(weak_ptr<T>& a, weak_ptr<T>& b) { a.swap(b); }

	
	
	
}