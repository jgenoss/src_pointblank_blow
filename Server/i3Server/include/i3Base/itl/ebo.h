#pragma once

//
//  ebo관련..	1. 멤버변수가 없이, 클래스에 기본클래스로 넣는 버전...
//				2. 멤버변수가 있고 멤버변수와 연동하는 버전...
//				--> 2개의 성질이 서로 다르므로, 다른 클래스로 처리..
#include "mpl/mpl_if.h"
#include "type_traits/is_empty.h"

namespace i3
{
		
	namespace detail
	{
		template<class T>
		class ebo : private T			// private상속이 더 적합할것으로 보인다..
		{
		protected:
			ebo(){}
			ebo(const T& ) {}				// 아무것도..없음..
			ebo(const ebo& ) {}				// 아무것도 없음..
			void assign(const ebo& ) {}	//
			const T&	get() const	{ return *this; }
			T&			get()		{ return *this; }
		};	
		
		template<class T>
		class not_ebo
		{
		protected:
			not_ebo(){}
			not_ebo(const T& val) : m_data(val) {}		// 초기화요구..
			not_ebo(const not_ebo& rhs) : m_data(rhs.get()) {}  // 복사생성자구현..
			void assign(const not_ebo& rhs) {  m_data = rhs.get(); }	//
			const T&	get() const		{ return m_data; }
			T&			get()			{ return m_data; }
		private:
			T				m_data;			// 이 멤버에 직접 접근하지 않도록 유의..(멤버함수를 충분이 만들것!)
		};
		
		template<class T>
		struct ebo_selector
		{
			typedef typename mpl::if_< is_empty<T>, ebo<T>, not_ebo<T> >::type type;
		};


	}

	template<class T>
	class enable_ebo : private detail::ebo_selector<T>::type
	{
	public:
		typedef typename detail::ebo_selector<T>::type		base_type;
		typedef T											value_type;

		enable_ebo() {}
		enable_ebo(const T& val) : base_type(val) {}
		enable_ebo(const enable_ebo& rhs) : base_type(rhs) {}	// 복사생성자..
		enable_ebo&				operator=(const enable_ebo& rhs) {  base_type::assign(rhs);	return *this; }
		__forceinline const T&	get() const		{ return base_type::get(); }
		__forceinline T&		get()			{ return base_type::get(); }
	};
	
		
	
}