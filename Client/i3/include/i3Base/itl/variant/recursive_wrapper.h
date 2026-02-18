#pragma once

#include "recursive_wrapper_fwd.h"

namespace i3
{
	template<class T>
	class recursive_wrapper
	{
	public:
		typedef T	type;

		recursive_wrapper() : m_p(new T) {}		// 동적 생성..(이런 부분을 개선하려면 코드변경이 있어야함)
		~recursive_wrapper() {  i3::checked_delete(m_p); }

		recursive_wrapper(const recursive_wrapper& rhs) : m_p( new T(rhs.get())) {}
		recursive_wrapper(const T& rhs) : m_p(new T(rhs)) {}
		
		recursive_wrapper& operator=(const recursive_wrapper& rhs) {  *m_p = rhs.get(); return *this; }
		recursive_wrapper& operator=(const T& rhs) {  *m_p = rhs; return *this; }
		
		void swap(recursive_wrapper& rhs) { T* tmp = rhs.m_p;  rhs.m_p = m_p;  m_p = tmp; }
		T&			get() { return *m_p; }
		const T&	get() const { return *m_p; }
		T*			get_pointer() { return m_p; }
		const T*	get_pointer() const { return m_p; }
	private:
		T*	m_p;
	};
	
	template<class T> inline
	void swap( recursive_wrapper<T>& lhs, recursive_wrapper<T>& rhs) {	lhs.swap(rhs);	}
		

}

#include "detail/type_accessor_recursive_wrapper.h"
