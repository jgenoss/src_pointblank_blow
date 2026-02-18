#pragma once

#include "shared_ptr.h"
#include "detail/shared_count.h"

//
//  레퍼런스 카운트 유지는 필요하지만 데이터 참조는 하지 않는 경우라면 4바이트를 절약하기 위해
//  shared_count를 따로 만드는게 좋을 것 같아서 따로 빼놓는다..
//

namespace i3
{
	template<class T>
	class shared_count;

	namespace detail
	{
		
		template< class X, class Y, class T > inline 
		void sp_enable_shared_from_this( const shared_count<X>* ppx, const Y* py, const enable_shared_from_this< T >* pe )
		{
			if( pe != 0 )
			{
				shared_ptr<X> ppx2(ppx, static_cast<X*>(const_cast<Y*>(py)));
				pe->internal_accept_owner( ppx2, const_cast< Y* >( py ) );
			}
		}

		template< class X, class Y, class T > inline 
		void sp_enable_shared_from_this( shared_count<X> * ppx, const Y* py, const enable_shared_from_this2<T>* pe )
		{
			if( pe != 0 ) 
			{
				shared_ptr<X> ppx2(ppx, static_cast<X*>(const_cast<Y*>(py)));
				pe->internal_accept_owner( ppx2, const_cast< Y* >( py ) );
			}
		}

	}


	template<class T>
	class shared_count
	{
	public:
		typedef T													element_type;
		typedef T													value_type;
		typedef T*													pointer;
		typedef typename detail::shared_ptr_add_reference<T>::type	reference;

		typedef shared_count<T>										this_type;

		static const bool	is_mt = type_enable_shared_mt<T>::value;

		shared_count() : pn() {}
		template<class U> explicit 
		shared_count(U* p) : pn(p) { detail::sp_enable_shared_from_this( this, p, p); }
		template<class U, class D>
		shared_count(U* p, D d) : pn(p, d) { detail::sp_enable_shared_from_this( this, p, p); }
		template<class U, class D, class A>
		shared_count(U* p, D d, A a) : pn(p, d, a) { detail::sp_enable_shared_from_this( this, p, p); }

		template<class U> explicit 
		shared_count(std::auto_ptr<U>& rhs)
		{ 
			T* px = rhs.get();
			pn = detail::shared_count<is_mt>(rhs);
			detail::sp_enable_shared_from_this( this, static_cast<U*>(px), static_cast<U*>(px) ); 
		}
		template<class Ap> explicit
		shared_count(Ap rhs, typename enable_if< is_std_auto_ptr<Ap>, int>::type = 0) 
		{ 
			T* px = rhs.get();
			pn = detail::shared_count<is_mt>(rhs);
			detail::sp_enable_shared_from_this( this, static_cast<typename Ap::element_type*>(px), static_cast<typename Ap::element_type*>(px) ); 
		}

		template<class U> explicit 
		shared_count(const weak_ptr<U>& rhs) : pn(rhs.pn) {}

		template<class U>	// enable_if로 대체..
		shared_count( const shared_ptr<U>& rhs, typename enable_if< is_convertible<U*, T*>, int>::type = 0 ) : pn(rhs.pn) {}

		template<class U>	// enable_if로 대체..
		shared_count( const shared_count<U>& rhs, typename enable_if< is_convertible<U*, T*>, int>::type = 0 ) : pn(rhs.pn) {}


		template<class U>	// 값은 따로 따오고, 레퍼런스는 기존 오브젝트에서 갖고옴..
		shared_count(const shared_ptr<U>& rhs, T* p) : pn(rhs.pn) {}


		shared_count& operator=(const shared_count& rhs) { this_type(rhs).swap(*this); return *this; }

		template<class U>
		shared_count& operator=(const shared_count<U>& rhs) { this_type(rhs).swap(*this); return *this; }

		template<class U>
		shared_count& operator=(const shared_ptr<U>& rhs) { this_type(rhs).swap(*this); return *this; }

		template<class U>
		shared_count& operator=(const std::auto_ptr<U>& rhs) { this_type(rhs).swap(*this); return *this; }

		template<class Ap> typename enable_if< is_std_auto_ptr<Ap>, 
		shared_count&>::type operator=(Ap rhs) { this_type(rhs).swap(*this); return *this; }

		void reset() { this_type().swap(*this); }
		template<class U>
		void reset(U* p) { this_type(p).swap(*this); }		// 같은 포인터리셋시 에러..
		template<class U, class D>
		void reset(U* p, D d) { this_type(p,d).swap(*this); }
		template<class U, class D, class A>
		void reset(U* p, D d, A a) { this_type(p,d,a).swap(*this); }
		template<class U>
		void reset(const shared_ptr<U>& rhs, T* p) { this_type(rhs, p).swap(*this); }

		bool		unique() const { return pn.unique(); }
		long		use_count() const { return pn.use_count(); }
		void		swap(shared_count& rhs) { pn.swap(rhs.pn); }

		template<class U> 
		bool		internal_less(const shared_count<U>& rhs) const { return pn < rhs.pn; }
		void*		internal_get_deleter( const std::type_info& ti) const { return pn.get_deleter(ti); }

	private:
		detail::shared_count<is_mt>	pn;
		template<class> friend class shared_ptr;
		template<class> friend class weak_ptr;
	};
	

	template<class T, class U> inline 
	bool operator<(const shared_count<T>& a, const shared_count<U>& b) { return a.internal_less(b); }

	template<class T> inline 
	void swap(shared_count<T> & a, shared_count<T> & b) { a.swap(b);	}

	template<class D, class T> inline
	D*	get_deleter(const shared_count<T>& p) { 	return static_cast<D*>(p.internal_get_deleter( typeid(D) ) ); }



}