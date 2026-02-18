#pragma once

#pragma warning(push)
#pragma warning(disable: 4512) // assignment operator could not be generated

//
//  일부함수에서 무의미해보이는 int, long이 덧붙는 이유...오버로딩 모호성을 해결하거나, 의도하던 특정 오버로딩을 취하기 위해서 붙음..
//  (0이 넘어가면 우선적으로 int값으로 고려되고, 여의치 않다면 long쪽 오버로딩을 조회하게 됨)
//  따라서, 보다 일반적인 템플릿파라미터함수는 long을 통해 우선순위를 낮추고, 특정 템플릿파라미터 함수는 int타입으로 인해 선호되는 함수로
//  쉽게 형변환되는 것을 이용한다...
//
#include "../arg.h"
#include "../../utility/mem_fn.h"

namespace i3
{
	template<class T> class weak_ptr;

	namespace bi
	{
		struct unspecified {};
		template<class R, class F, class L> class bind_t;
		
		template<class T>
		class value 
		{
		public:
			value(const T& t) : m_t(t) {}
			T&			get() { return m_t; }
			const T&	get() const { return m_t; }
			bool		operator==(const value& rhs) const { return m_t == rhs.m_t; }
		private:
			T		m_t;
		};
		
		template<class R, class F> struct result_traits { typedef R type; };
		template<class F> struct result_traits<unspecified, F> { typedef typename F::result_type type; };
		template<class F> struct result_traits<unspecified, reference_wrapper<F> > { typedef typename F::result_type type; };
		
		template<class T>	bool ref_compare(const T& a, const T& b, long) { return a == b; }
		template<int I>		bool ref_compare(const arg<I>&, const arg<I>&, int) { return true; }
		template<int I>		bool ref_compare(arg<I>(*)(), arg<I>(*)(), int) { return true; }
		template<class T>	bool ref_compare(const reference_wrapper<T>& a, const reference_wrapper<T>& b, int) { return a.get_pointer() == b.get_pointer(); }
		template<class R, class F, class L> bool ref_compare(const bind_t<R,F,L>& a, const bind_t<R,F,L>& b, int) { return a.compare(b); }
		template<class T>	bool ref_compare(const value<weak_ptr<T> >& a, const value<weak_ptr<T> >& b, int){ return !(a.get()<b.get()) && !(b.get()<a.get()); }
		
		template<class T> class type {};

		template<class F> 
		struct unwrapper
		{
			static inline F& unwrap(F& f, long) { return f; }
			template<class F2> static inline F2& unwrap( reference_wrapper<F2> rf, int) { return rf.get(); }
			template<class R, class T> static inline mfi::mp<R, T> unwrap( R T::* pm, int) { return mfi::mp<R, T>(pm); }
		};
		
	}
}


#pragma warning(default: 4512) 
#pragma warning(pop)
