#pragma once

//
// 널객체스타일로 처리하는 것 이외에는 intrusive_ptr과 동일하게..처리.
// 근본원리에 잘 맞추려면 널값 입력이 들어오지 않도록 해야함..널값 대신에 null_object<T>()를 사용...
// 객체공유가 매우 빈번하고, 객체 대부분이 유효객체인 경우에 적합..
// (객체 공유 빈도가 적고, 널값이 많이 들어간다면 원래의 intrusive_ptr이 더 나을것이다.)
//

#include "null_ref_ptr.h"
#include "intrusive_ptr_add_ref.h"
#include "intrusive_ptr_release.h"
#include "not_add_ref_tag.h"
#include "../itl/compile_assert.h"
#include "../itl/utility/safe_bool.h"

namespace i3
{

	template<class T>
	struct	null_object 
	{
		operator	const	T*() const { return i3::null_ref_ptr<T>();  }
		operator			T*() { return i3::null_ref_ptr<T>();  }
		T*					operator->() const { return i3::null_ref_ptr<T>(); }
		T&					operator*() const { return *i3::null_ref_ptr<T>(); }
	};

	template<class T, class B >
	class null_object_intrusive_ptr
	{
	public:
		compile_assert_pred( (i3::is_base_of<B, T>) );

		typedef null_object_intrusive_ptr	my_type;
		typedef B							ref_base_type;
		typedef safe_bool<my_type>			safe_bool_t;

		null_object_intrusive_ptr() : m_p( null_object<B>()  ) { intrusive_ptr_add_ref(m_p); }
		null_object_intrusive_ptr(T* p) : m_p(p) { intrusive_ptr_add_ref(m_p); }
		null_object_intrusive_ptr(null_object<B> no) : m_p(no) { intrusive_ptr_add_ref(m_p); }
		null_object_intrusive_ptr(T* p, not_add_ref_tag) : m_p(p) {}

		null_object_intrusive_ptr(const null_object_intrusive_ptr& rhs) : m_p(rhs.m_p) { intrusive_ptr_add_ref(m_p); }
		template<class U> null_object_intrusive_ptr(const null_object_intrusive_ptr<U, B>& rhs, 
			typename i3::enable_if< i3::is_convertible<U*,T*>, int>::type = 0) : m_p(rhs.m_p ) { intrusive_ptr_add_ref(m_p); }
		
		~null_object_intrusive_ptr() { intrusive_ptr_release(m_p); }		

		void				reset() { my_type().swap(*this); }
		void				reset(T* p) { my_type(p).swap(*this); }
		void				reset(null_object<B> no) { my_type(no).swap(*this); }

		intrusive_ptr&		operator=(const null_object_intrusive_ptr& rhs) {  my_type(rhs).swap(*this); return *this; }
		template<class U>
		intrusive_ptr&		operator=(const null_object_intrusive_ptr<U,B>& rhs) { my_type(rhs).swap(*this);	return *this; }
		intrusive_ptr&		operator=(T* rhs) { my_type(rhs).swap(*this);	return *this; }
		intrusive_ptr&		operator=(null_object<B> rhs) { my_type(rhs).swap(*this);	return *this; }

		T*					get() const {  return	static_cast<T*>(m_p); }
		operator			T*() const			{	return get(); }
		T&					operator*() const { return *get(); }
		T*					operator->() const { return get(); }

		void				swap(intrusive_ptr& rhs) {  i3::swap( m_p, rhs.m_p); }

		bool				operator!() const	{ return m_p == null_object<B>(); }

		operator typename safe_bool_t::type() const { return safe_bool_t::call(m_p != null_object<B>()); }

	private:
		null_object_intrusive_ptr(int);
		intrusive_ptr&		operator=(int);
		void				reset(int);

		ref_base_type*		m_p;
		template<class,class>		friend class null_object_intrusive_ptr;
	};
	
	template<class T, class B> inline 
	bool operator==(null_object_intrusive_ptr<T,B> const & , int);
	template<class T, class B> inline 
	bool operator!=(int, null_object_intrusive_ptr<T,B> const & );

	template<class T, class B> inline 
	bool  operator<( null_object_intrusive_ptr<T,B> const & a, int );
	template<class T, class B> inline 
	bool  operator<( int, null_object_intrusive_ptr<T,B> const & );


	template<class T, class U, class B> inline 
	bool operator==(null_object_intrusive_ptr<T,B> const & a, null_object_intrusive_ptr<U,B> const & b) 
	{	return a.get() == b.get();	}

	template<class T, class U, class B> inline 
	bool operator==(null_object_intrusive_ptr<T,B> const & a, U* b) {	return a.get() == b;	}

	template<class T, class U, class B> inline 
	bool operator==(T* a, null_object_intrusive_ptr<U,B> const & b ) {	return a == b.get();	}

	template<class T, class U, class B> inline 
	bool operator!=(null_object_intrusive_ptr<T,B> const & a, U* b) {	return !(a == b);  }

	template<class T, class U, class B> inline 
	bool operator!=(null_object_intrusive_ptr<T,B> const & a, null_object_intrusive_ptr<U,B> const & b) {	return !(a == b);  }

	template<class T, class U, class B> inline 
	bool operator!=(T* a, null_object_intrusive_ptr<U,B> const & b ) {	return !(a == b);  }

	template<class T, class U, class B> inline 
	bool  operator<( null_object_intrusive_ptr<T,B> const & a, null_object_intrusive_ptr<U,B> const & b ) { return a.get() < b.get(); }

	template<class T, class U, class B> inline 
	bool  operator<( null_object_intrusive_ptr<T,B> const & a, U* b ) { return a.get() < b; }

	template<class T, class U, class B> inline 
	bool  operator<( T* a, null_object_intrusive_ptr<U,B> const & b ) { return a < b.get(); }

	template<class T, class U, class B> inline 
	bool  operator>( null_object_intrusive_ptr<T,B> const & a, null_object_intrusive_ptr<U,B> const & b ) { return  b < a ; }

	template<class T, class U, class B> inline 
	bool  operator>( null_object_intrusive_ptr<T,B> const & a, U* b ) { return  b < a ; }

	template<class T, class U, class B> inline 
	bool  operator>( T* a, null_object_intrusive_ptr<U,B> const & b ) { return  b < a ; }


	template<class T, class U, class B> inline 
	bool  operator<=( null_object_intrusive_ptr<T,B> const & a, null_object_intrusive_ptr<U,B> const & b ) { return !(b < a); }

	template<class T, class U, class B> inline 
	bool  operator<=( null_object_intrusive_ptr<T,B> const & a, U* b ) { return !(b < a); }

	template<class T, class U, class B> inline 
	bool  operator<=( T* a, null_object_intrusive_ptr<U,B> const & b ) { return !(b < a); }


	template<class T, class U, class B> inline 
	bool  operator>=( null_object_intrusive_ptr<T,B> const & a, null_object_intrusive_ptr<U,B> const & b ) { return !(a < b); }

	template<class T, class U, class B> inline 
	bool  operator>=( null_object_intrusive_ptr<T,B> const & a, U* b ) { return !(a < b); }

	template<class T, class U, class B> inline 
	bool  operator>=( T* a, null_object_intrusive_ptr<U,B> const & b ) { return !(a < b); }

	template<class T, class B> inline 
	void swap(null_object_intrusive_ptr<T,B>& lhs, null_object_intrusive_ptr<T,B>& rhs) { lhs.swap(rhs); }


	template<class T, class U, class B> inline
	null_object_intrusive_ptr<T,B> static_pointer_cast(null_object_intrusive_ptr<U,B> const & p) {	return static_cast<T*>(p.get());	}

	template<class T, class U, class B> inline
	null_object_intrusive_ptr<T,B> const_pointer_cast(null_object_intrusive_ptr<U,B> const & p) {	return const_cast<T*>(p.get());  }

	template<class T, class U, class B> inline
	null_object_intrusive_ptr<T,B> dynamic_pointer_cast(null_object_intrusive_ptr<U,B> const & p) {	return dynamic_cast<T*>(p.get());  }

	template<class T, class B> inline T* get_pointer(null_object_intrusive_ptr<T,B> const & p) {	return p.get(); 	}

	template< class T> struct hash;

	template<class T, class B>
	struct hash< i3::null_object_intrusive_ptr<T,B> > : public i3::unary_function< i3::null_object_intrusive_ptr<T,B>, std::size_t>
	{
		std::size_t operator()( const i3::null_object_intrusive_ptr<T,B>& v) const {  return i3::hash<T*>()( v.get() );  }
	};

	
	
}