#pragma once

#include "detail/shared_count.h"

namespace i3
{
	
	template<class T>
	class shared_array
	{
	public:
		typedef T							element_type;

		typedef checked_array_deleter<T>	deleter;
		typedef shared_array<T>				this_type;
		
		static const bool					is_mt = type_enable_shared_mt<T>::value;

		explicit shared_array(T* p = 0) : px(p), pn(p, deleter()) {}
		template<class D> 
		shared_array(T* p, D d) : px(p), pn(p, d) {}
		
		void	reset(T* p = 0) { this_type(p).swap(*this); }
		template<class D>
		void	reset(T* p, D d) { this_type(p, d).swap(*this); }
		
		T&		operator[](std::ptrdiff_t i) const { return px[i]; }
		T*		get() const { return px; }
		
		typedef T* this_type::*unspecified_bool_type;		// 멤버포인터의 타입치환일 뿐..
		operator	unspecified_bool_type() const { return (px == 0) ? 0 : &this_type::px; }		// 유효값이면 멤버포인터를......
		bool		operator!() const { return px == 0; }
		
		bool unique() const { return pn.unique(); }
		long use_count() const { return pn.use_count(); }
		void swap(shared_array<T>& other) { i3::swap(px, other.px);	pn.swap(other.pn);	}

	private:
		
		T*	px;
		detail::shared_count<is_mt> pn;
	};

	template<class T> inline 
	bool operator==(const shared_array<T>& a, const shared_array<T>& b) {	return a.get() == b.get();	}

	template<class T> inline 
	bool operator!=(const shared_array<T>& a, const shared_array<T>& b) {	return a.get() != b.get();	}

	template<class T> inline 
	bool operator<(const shared_array<T>& a, const shared_array<T>& b)  {	return a.get() < b.get();  }

	template<class T> inline
	void swap(shared_array<T>& a, shared_array<T>& b) {	a.swap(b); }

}