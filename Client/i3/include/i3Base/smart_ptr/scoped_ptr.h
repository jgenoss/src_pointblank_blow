#pragma once

#include "scoped_ptr_fwd.h"

#include <memory>
#include "../itl/checked_delete.h"

namespace i3
{
	// boost의 것에서 아주 조금만 수정... base클래스 둔 것은 삭제...

	template<class T> 
	class scoped_ptr // noncopyable
	{
	public:
		typedef scoped_ptr<T>		this_type;
		typedef T					element_type;

		explicit scoped_ptr( T * p = 0 ): m_p( p ) {}
		explicit scoped_ptr( std::auto_ptr<T> p ): m_p( p.release() ) {}
		~scoped_ptr() {	i3::checked_delete( m_p ); }
		void			reset() {	i3::checked_delete( m_p );	m_p = 0; }

		void reset(T* p)
		{
			if (p == m_p) return;
			i3::checked_delete(m_p); m_p = p;	
		}
		
//		scoped_ptr&	operator=(T* p)	{	reset(p);	return *this; }		// 리턴된 것을 다른것에 대입하지는 말것...

		operator		T*() const	{ return m_p; }
		T& operator*() const {	return *m_p; }
		T* operator->() const {  return m_p;  }
		T* get() const {  return m_p;  }

		operator		bool() const		{ return m_p != 0; }
		bool			operator!() const	{ return !m_p; }
			
		void swap(scoped_ptr & b) 
		{	
			T * tmp = b.m_p;	b.m_p = m_p;     m_p = tmp; 
		}
	    
	private:
		T * m_p;
		scoped_ptr(scoped_ptr const &);
		scoped_ptr & operator=(scoped_ptr const &);
		void operator==( scoped_ptr const& ) const;
		void operator!=( scoped_ptr const& ) const;
	};

	template<class T> inline 
	void swap(scoped_ptr<T> & a, scoped_ptr<T> & b) {	a.swap(b); }

	template<class T> inline 
	T* get_pointer(scoped_ptr<T> const & p) { return p.get(); }

	template<class T> inline 
	bool operator==(scoped_ptr<T> const & a, int ) {	return a.get() == 0; }

	template<class T> inline 
	bool operator==(int, scoped_ptr<T> const & a ) {	return a.get() == 0; }

	template<class T> inline 
	bool operator!=(scoped_ptr<T> const & a, int ) {	return a.get() != 0; }

	template<class T> inline 
	bool operator!=(int, scoped_ptr<T> const & a ) {	return a.get() != 0; }




}