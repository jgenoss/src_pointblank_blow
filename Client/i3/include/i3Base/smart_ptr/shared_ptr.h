#pragma once

#include "detail/shared_count.h"

#include "detail/shared_ptr_add_reference.h"

#include "../itl/type_traits/is_std_auto_ptr.h"

#include "../itl/enable_if.h"
#include "../itl/type_traits/is_convertible.h"

#include "shared_ptr_fwd.h"

namespace i3
{
	//
	namespace detail
	{
		struct static_cast_tag {};
		struct const_cast_tag {};
		struct dynamic_cast_tag {};

		struct sp_counted_base_tag {};		// enable_if 대신에, 명확하게 태그를 인자로 써서 처리하는 부분 존재..(make_shared/allocate_shared)

		template< class X, class Y, class T > inline 
		void sp_enable_shared_from_this( const shared_ptr<X>* ppx, const Y* py, const enable_shared_from_this< T >* pe )
		{
			if( pe != 0 ) pe->internal_accept_owner( ppx, const_cast< Y* >( py ) );
		}

		template< class X, class Y, class T > inline 
		void sp_enable_shared_from_this( shared_ptr<X> * ppx, const Y* py, const enable_shared_from_this2<T>* pe )
		{
			if( pe != 0 ) pe->internal_accept_owner( ppx, const_cast< Y* >( py ) );
		}

		inline void sp_enable_shared_from_this(...) {}		// enable_shared종류가 아니면 스킵(swallow)..
	}


	template<class T>
	class shared_ptr
	{
	public:
		typedef T													element_type;
		typedef T													value_type;
		typedef T*													pointer;
		typedef typename detail::shared_ptr_add_reference<T>::type	reference;
		typedef shared_ptr<T>										this_type;

		static const bool	is_mt = type_enable_shared_mt<T>::value;

		shared_ptr() : px(0), pn() {}
		template<class U> explicit 
		shared_ptr(U* p) : px(p), pn(p) { detail::sp_enable_shared_from_this( this, p, p); }
		template<class U, class D>
		shared_ptr(U* p, D d) : px(p), pn(p, d) { detail::sp_enable_shared_from_this( this, p, p); }
		template<class U, class D, class A>
		shared_ptr(U* p, D d, A a) : px(p), pn(p, d, a) { detail::sp_enable_shared_from_this( this, p, p); }

		template<class U> explicit 
		shared_ptr(std::auto_ptr<U>& rhs): px(rhs.get()), pn(rhs) { detail::sp_enable_shared_from_this( this, static_cast<U*>(px), static_cast<U*>(px) ); }
		template<class Ap> explicit
		shared_ptr(Ap rhs, typename enable_if< is_std_auto_ptr<Ap>, int>::type = 0) : px(rhs.get()), pn(rhs) 
			{ detail::sp_enable_shared_from_this( this, static_cast<typename Ap::element_type*>(px), static_cast<typename Ap::element_type*>(px) ); }
		
		template<class U> explicit 
		shared_ptr(const weak_ptr<U>& rhs) : px(0), pn(rhs.pn) { if (!pn.empty()) px = rhs.px; }

		template<class U>	// enable_if로 대체..
		shared_ptr( const shared_ptr<U>& rhs, typename enable_if< is_convertible<U*, T*>, int>::type = 0 ) : px(rhs.px), pn(rhs.pn) {}
		
		template<class U>	// 값은 따로 따오고, 레퍼런스는 기존 오브젝트에서 갖고옴..
		shared_ptr(const shared_ptr<U>& rhs, T* p) : px(p), pn(rhs.pn) {}
		
		template<class U>
		shared_ptr(const shared_count<U>& rhs, T* p) : px(p), pn(rhs.pn) {}

		// 아래 생성자는 make_shared함수를 위해 설정됨...
		shared_ptr(detail::sp_counted_base_tag, typename detail::shared_count<is_mt>::sp_counted_base* cb, T* p) : px(p), pn(cb) { detail::sp_enable_shared_from_this(this, p, p); }

		template<class U>
		shared_ptr(const shared_ptr<U>& rhs, detail::static_cast_tag): px(static_cast<element_type*>(rhs.px)), pn(rhs.pn){}
		template<class U>
		shared_ptr(const shared_ptr<U>& rhs, detail::const_cast_tag): px(const_cast<element_type*>(rhs.px)), pn(rhs.pn){}
		template<class U>
		shared_ptr(const shared_ptr<U>& rhs, detail::dynamic_cast_tag): px(dynamic_cast<element_type*>(rhs.px)), pn(rhs.pn) {	if(px == 0) pn = detail::shared_count<is_mt>(); }
		

		shared_ptr& operator=(const shared_ptr& rhs) { this_type(rhs).swap(*this); return *this; }

		template<class U>
		shared_ptr& operator=(const shared_ptr<U>& rhs) { this_type(rhs).swap(*this); return *this; }
		
		template<class U>
		shared_ptr& operator=(const std::auto_ptr<U>& rhs) { this_type(rhs).swap(*this); return *this; }

		template<class Ap> typename enable_if< is_std_auto_ptr<Ap>, 
		shared_ptr&>::type operator=(Ap rhs) { this_type(rhs).swap(*this); return *this; }

		void reset() { this_type().swap(*this); }
		template<class U>
		void reset(U* p) { this_type(p).swap(*this); }		// 같은 포인터리셋시 에러..
		template<class U, class D>
		void reset(U* p, D d) { this_type(p,d).swap(*this); }
		template<class U, class D, class A>
		void reset(U* p, D d, A a) { this_type(p,d,a).swap(*this); }
		template<class U>
		void reset(const shared_ptr<U>& rhs, T* p) { this_type(rhs, p).swap(*this); }
		template<class U>
		void reset(const shared_count<U>& rhs, T* p) { this_type(rhs, p).swap(*this); }


		reference	operator*() const	{ return *px; }
		T*			operator->() const	{ return px; }
		T*				get() const		{ return px; }		// 이제 표준이므로, 암시적 변환은 하지 않음..
		
		typedef T* this_type::*unspecified_bool_type;		// 멤버포인터의 타입치환일 뿐..

		operator	unspecified_bool_type() const { return (px == 0) ? 0 : &this_type::px; }		// 유효값이면 멤버포인터를......
		bool		operator!() const { return px == 0; }
		
		bool		unique() const { return pn.unique(); }
		long		use_count() const { return pn.use_count(); }
		void		swap(shared_ptr& rhs) { i3::swap(px, rhs.px);  pn.swap(rhs.pn); }
		
		template<class U> 
		bool		internal_less(const shared_ptr<U>& rhs) const { return pn < rhs.pn; }
		void*		internal_get_deleter( const std::type_info& ti) const { return pn.get_deleter(ti); }
		bool		internal_equiv( const shared_ptr& rhs) const { return px == rhs.px && pn == rhs.pn; }

	private:
		T*							px;			// px의 intrusive특성이 없기 때문에 널값 허용에 제약이 제거...
		detail::shared_count<is_mt>	pn;
		
		template<class> friend class shared_ptr;
		template<class> friend class weak_ptr;
	};
	
	template<class T, class U> inline 
	bool operator==(const shared_ptr<T>& a, const shared_ptr<U>& b) { return a.get() == b.get(); }

	template<class T, class U> inline 
	bool operator!=(const shared_ptr<T>& a, const shared_ptr<U>& b) { return a.get() != b.get(); }
	
	template<class T, class U> inline 
	bool operator<(const shared_ptr<T>& a, const shared_ptr<U>& b) { return a.internal_less(b); }

	template<class T> inline 
	void swap(shared_ptr<T> & a, shared_ptr<T> & b) { a.swap(b);	}

	template<class T> inline
	T*	get_pointer(const shared_ptr<T>& p) { return p.get(); }


	template<class T, class U> inline
	shared_ptr<T> static_pointer_cast(const shared_ptr<U>& r) {	return shared_ptr<T>(r, detail::static_cast_tag());	}

	template<class T, class U> inline
	shared_ptr<T> const_pointer_cast(const shared_ptr<U>& r)  {	return shared_ptr<T>(r, detail::const_cast_tag());	}

	template<class T, class U> inline
	shared_ptr<T> dynamic_pointer_cast(const shared_ptr<U>& r) { return shared_ptr<T>(r, detail::dynamic_cast_tag());	}

	template<class D, class T> inline
	D*			  get_deleter(const shared_ptr<T>& p) { 	return static_cast<D*>(p.internal_get_deleter( typeid(D) ) ); }
	
	
	
}
