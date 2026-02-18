#pragma once

#include "scoped_array_fwd.h"
#include "../itl/checked_delete.h"

namespace i3
{
	template<class T>
	class scoped_array
	{
	public:
		typedef scoped_array<T>		this_type;
		typedef T					element_type;
		
		explicit scoped_array( T * p = 0 ) : m_p(p) {}
		~scoped_array() {	i3::checked_array_delete( m_p );	}

		void		reset() {	i3::checked_array_delete( m_p );	m_p = 0; }
		void		reset(T* p) {	if (p == m_p) return;	i3::checked_array_delete(m_p); m_p = p;	}

		T&			operator[](std::ptrdiff_t i) const {	return m_p[i];	}

		T *			get() const {	return m_p;	}
		operator	bool() const		{ return m_p != 0; }
		bool		operator!() const	{ return !m_p; }
	
		void		swap(scoped_array & b) {	T* tmp = b.m_p;	b.m_p = m_p;	m_p = tmp;  }
		
	private:
		scoped_array(const scoped_array&);
		scoped_array & operator=(const scoped_array&);
		void operator==( const scoped_array& ) const;
		void operator!=( const scoped_array& ) const;

		T*		m_p;
	};

	template<class T> inline 
	void swap(scoped_array<T>& a, scoped_array<T>& b) {	a.swap(b);	}

}
