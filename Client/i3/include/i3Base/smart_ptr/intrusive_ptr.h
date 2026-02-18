#pragma once

#include "intrusive_ptr_fwd.h"
#include "intrusive_ptr_add_ref.h"
#include "intrusive_ptr_release.h"

#include "not_add_ref_tag.h"
#include "../itl/utility/safe_bool.h"


namespace i3
{



	template<class T>
	class intrusive_ptr
	{
	public:
		typedef intrusive_ptr						my_type;
		typedef safe_bool<my_type>					safe_bool_t;

		intrusive_ptr() : m_p(0) { }
		intrusive_ptr(T* p) : m_p(p) {  if (p) intrusive_ptr_add_ref(p); }
		intrusive_ptr(T* p, not_add_ref_tag) : m_p(p) {}
		intrusive_ptr(const intrusive_ptr& rhs) : m_p(rhs.m_p) {  if (m_p) intrusive_ptr_add_ref(m_p);  }
		template<class U> intrusive_ptr(const intrusive_ptr<U>& rhs, 
			typename i3::enable_if< i3::is_convertible<U*,T*>, int>::type = 0) : m_p( rhs.get()) {  if (m_p) intrusive_ptr_add_ref(m_p); }
		~intrusive_ptr() { if (m_p) intrusive_ptr_release(m_p); }		
		
		void				reset() { my_type().swap(*this); }
		void				reset(T* p) { my_type(p).swap(*this); }

		intrusive_ptr&		operator=(const intrusive_ptr& rhs) {  my_type(rhs).swap(*this); return *this; }
		template<class U>
		intrusive_ptr&		operator=(const intrusive_ptr<U>& rhs) { my_type(rhs).swap(*this);	return *this; }
		intrusive_ptr&		operator=(T* rhs) { my_type(rhs).swap(*this);	return *this; }

		T*					get() const {  return	m_p; }
		operator			T*() const			{	return m_p; }
		T&					operator*() const { return *m_p; }
		T*					operator->() const { return m_p; }

		void				swap(intrusive_ptr& rhs) {  i3::swap( m_p, rhs.m_p); }
		bool				operator!() const	{ return m_p == 0; }
	
		operator typename safe_bool_t::type() const { return safe_bool_t::call(m_p != 0); }

	private:
		T*					m_p;		// Æ÷ÀÎÅÍ..
	};
	
	
	template<class T, class U> inline 
	bool operator==(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b) {	return a.get() == b.get();	}

	template<class T, class U> inline 
	bool operator==(intrusive_ptr<T> const & a, U* b) {	return a.get() == b;	}

	template<class T, class U> inline 
	bool operator==(T* a, intrusive_ptr<U> const & b ) {	return a == b.get();	}

	template<class T, class U> inline 
	bool operator!=(intrusive_ptr<T> const & a, U* b) {	return !(a == b);  }

	template<class T, class U> inline 
	bool operator!=(intrusive_ptr<T> const & a, intrusive_ptr<U> const & b) {	return !(a == b);  }

	template<class T, class U> inline 
	bool operator!=(T* a, intrusive_ptr<U> const & b ) {	return !(a == b);  }

	template<class T, class U> inline 
	bool  operator<( intrusive_ptr<T> const & a, intrusive_ptr<U> const & b ) { return a.get() < b.get(); }
	
	template<class T, class U> inline 
	bool  operator<( intrusive_ptr<T> const & a, U* b ) { return a.get() < b; }

	template<class T, class U> inline 
	bool  operator<( T* a, intrusive_ptr<U> const & b ) { return a < b.get(); }

	template<class T, class U> inline 
	bool  operator>( intrusive_ptr<T> const & a, intrusive_ptr<U> const & b ) { return  b < a ; }
	
	template<class T, class U> inline 
	bool  operator>( intrusive_ptr<T> const & a, U* b ) { return  b < a ; }

	template<class T, class U> inline 
	bool  operator>( T* a, intrusive_ptr<U> const & b ) { return  b < a ; }

	
	template<class T, class U> inline 
	bool  operator<=( intrusive_ptr<T> const & a, intrusive_ptr<U> const & b ) { return !(b < a); }

	template<class T, class U> inline 
	bool  operator<=( intrusive_ptr<T> const & a, U* b ) { return !(b < a); }

	template<class T, class U> inline 
	bool  operator<=( T* a, intrusive_ptr<U> const & b ) { return !(b < a); }


	template<class T, class U> inline 
	bool  operator>=( intrusive_ptr<T> const & a, intrusive_ptr<U> const & b ) { return !(a < b); }

	template<class T, class U> inline 
	bool  operator>=( intrusive_ptr<T> const & a, U* b ) { return !(a < b); }

	template<class T, class U> inline 
	bool  operator>=( T* a, intrusive_ptr<U> const & b ) { return !(a < b); }

	template<class T> inline 
	void swap(intrusive_ptr<T>& lhs, intrusive_ptr<T>& rhs) { lhs.swap(rhs); }


	template<class T, class U> inline
	intrusive_ptr<T> static_pointer_cast(intrusive_ptr<U> const & p) {	return static_cast<T*>(p.get());	}

	template<class T, class U> inline
	intrusive_ptr<T> const_pointer_cast(intrusive_ptr<U> const & p) {	return const_cast<T*>(p.get());  }

	template<class T, class U> inline
	intrusive_ptr<T> dynamic_pointer_cast(intrusive_ptr<U> const & p) {	return dynamic_cast<T*>(p.get());  }

	template<class T> inline T* get_pointer(intrusive_ptr<T> const & p) {	return p.get(); 	}

	template< class T > struct hash;

	template<class T>
	struct hash< i3::intrusive_ptr<T> > : public i3::unary_function< i3::intrusive_ptr<T>, std::size_t>
	{
		std::size_t operator()( const i3::intrusive_ptr<T>& v) const {  return i3::hash<T*>()( v.get() );  }
	};

}
	/////
	