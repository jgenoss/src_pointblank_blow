#if !defined(pp_is_iterating)

#ifndef pscoped_ptr_h
#define pscoped_ptr_h

#include "pscoped_ptr_fwd.h"
#include "../itl/preprocessor/ext/pp_enum_params_1_cat_params.h"
#include "../itl/preprocessor/ext/pp_enum_params_1.h"

#define	make_pscoped_limit	10

namespace i3
{
	// pool에서 생성시키고, pool해제를 지원하도록 수정..
	
	namespace detail 
	{ 
		template<class T>
		struct pscoped_value_wrapper 
		{
			T* p;
			explicit pscoped_value_wrapper(T* p) : p(p) {}
		private:
			void	operator=(pscoped_value_wrapper&);
		};
	}

	template<class T> 
	struct enable_pool_dbg< pscoped_ptr<T> > : enable_pool_dbg<T> {};
		
	template<class T> 
	class pscoped_ptr // noncopyable
	{
	public:
		typedef pscoped_ptr<T>		this_type;
		typedef T					element_type;
		
		pscoped_ptr() : m_p(nullptr) {}
		template<class U>
		pscoped_ptr(detail::pscoped_value_wrapper<U> pvw) : m_p(pvw.p) {}
		~pscoped_ptr() { destroy_impl(); }
		void reset()  {	destroy_impl(); m_p = 0; }
		template<class U>
		void reset(detail::pscoped_value_wrapper<U> pvw) {	if (pvw.p == m_p) return;	destroy_impl();	m_p = pvw.p; }

		template<class U>
		pscoped_ptr&	operator=(detail::pscoped_value_wrapper<U> pvw) { reset(pvw); return *this; }	// 리턴된 것을 다른것에 대입하지는 말것...

		operator		T*() const	{ return m_p; }
		T& operator*() const {	return *m_p; }
		T* operator->() const {  return m_p;  }
		T* get() const {  return m_p;  }

		operator		bool() const		{ return m_p != 0; }
		bool			operator!() const	{ return !m_p; }

		void swap(pscoped_ptr & b) 
		{	
			T * tmp = b.m_p;	b.m_p = m_p;     m_p = tmp; 
		}

	private:
		void destroy_impl() 
		{
			if (m_p)
			{
				m_p->~T();		// 소멸자 호출후, 메모리 저장소를 제거..
				i3::static_pool_type_free(i3::identity<T>(), m_p );
			}
		}

		T * m_p;
		pscoped_ptr(pscoped_ptr const &);
		pscoped_ptr & operator=(pscoped_ptr const &);
		void operator==( pscoped_ptr const& ) const;
		void operator!=( pscoped_ptr const& ) const;
	};

	template<class T> inline 
	void swap(pscoped_ptr<T> & a, pscoped_ptr<T> & b) {	a.swap(b); }

	template<class T> inline 
	T* get_pointer(pscoped_ptr<T> const & p) { return p.get(); }

	template<class T> inline 
		bool operator==(pscoped_ptr<T> const & a, int ) {	return a.get() == 0; }

	template<class T> inline 
		bool operator==(int, pscoped_ptr<T> const & a ) {	return a.get() == 0; }

	template<class T> inline 
		bool operator!=(pscoped_ptr<T> const & a, int ) {	return a.get() != 0; }

	template<class T> inline 
		bool operator!=(int, pscoped_ptr<T> const & a ) {	return a.get() != 0; }
	

	
#pragma push_macro("new")	
#undef new

	template<class T>
	detail::pscoped_value_wrapper<T> make_pscoped()
	{
		void* raw = i3::static_pool_type_malloc(i3::identity<T>(), sizeof(T));
		T* pt = ::new (raw) T();
		return detail::pscoped_value_wrapper<T>(pt);
	}
		
#pragma pop_macro("new")		
	
}

#include "../itl/preprocessor/pp_iterate.h"

#define pp_iteration_params_1	(3, (1, make_pscoped_limit, "pscoped_ptr.h"))		// 1 ~ 10까지 특화 / 자체재귀..(양개구간임에 유의할것)
#include pp_iterate()

#endif	// pscoped_ptr.h

#else // pp_is_iterating

#define		i_			pp_frame_iteration(1)

namespace i3
{

#pragma push_macro("new")	
#undef new

	template<class T, pp_enum_params_1(i_, class P) >
	detail::pscoped_value_wrapper<T>	make_pscoped( pp_enum_params_1_cat_params(i_, const P,&, p) )
	{
		void* raw = i3::static_pool_type_malloc(i3::identity<T>(), sizeof(T));
		T* pt = ::new (raw) T( pp_enum_params_1(i_, p) );
		return detail::pscoped_value_wrapper<T>(pt);
	}

#pragma pop_macro("new")

}

#undef i_

#endif // pp_is_iterating
