#pragma once

#include "../../type_traits/remove_reference.h"
#include "../../type_traits/is_convertible.h"

namespace i3
{
	namespace detail
	{
		// 후증가 연산에 대한 input_iterator, output_iterator 예외 처리..
		template<class It>
		class postfix_increment_proxy
		{
			typedef typename iterator_value<It>::type		value_type;
		public:
			explicit postfix_increment_proxy(const It& i) : m_value(*i) {}
			value_type& operator*() const { return m_value; }
		private:
			mutable value_type m_value;
		};
		template<class It>
		class writable_postfix_increment_proxy
		{
			typedef typename iterator_value<It>::type		value_type;
		public:
			explicit writable_postfix_increment_proxy(const It& i) : m_value(*i), m_iter(i) {}
			const writable_postfix_increment_proxy& operator*() const { return *this; }

			operator value_type&() const { return m_value; }
			operator const It&() const { return m_iter; }

			template<class It2>
			const It2& operator=(const It2& i) const {  *m_iter = i; return i;  }		// return *this가 아니라 그대로 리턴..
			template<class It2>
			It2& operator=(It2& i) const {  *m_iter = i; return i;  }		// return *this가 아니라 그대로 리턴..
		private:
			mutable value_type m_value;
			It				   m_iter;
		};
		
		// 레퍼런스타입 원형이 Value타입 그대로인 경우...
		template<class Ref, class Val>
		struct is_non_proxy_reference : is_convertible< typename remove_reference<Ref>::type const volatile*, Val const volatile*> {};

		template<class It, class Val, class Ref, class Cat>
		struct postfix_increment_result : mpl::eval_if< mpl::and_<	is_convertible<Ref, const Val& >,
																	mpl::not_<is_convertible< Cat, i3::forward_iterator_tag> > >,
														mpl::if_<	is_non_proxy_reference<Ref, Val>,
																	postfix_increment_proxy<It>,
																	writable_postfix_increment_proxy<It> >,
														mpl::identity<It> > {};
		

		// 레퍼런스가 순수 레퍼런스타입이 아닌 경우의 예외처리.
		template<class Ref, class Ptr>
		struct operator_arrow_dispatch
		{
			struct proxy 
			{
				explicit	proxy(const Ref& r) : m_ref(r) {}
				Ref*		operator->() { return i3::addressof(m_ref); }
				operator	Ref*()		 { return i3::addressof(m_ref); }
				Ref			m_ref;
			};
			typedef proxy result_type;
			static result_type apply(const Ref& r) { return result_type(x); }		
		};
		
		template<class T, class Ptr>
		struct operator_arrow_dispatch<T&, Ptr> 
		{
			typedef Ptr result_type;
			static result_type apply(T& r) { return i3::addressof(r); }
		};
		
		// 배열첨자연산자의 예외적 처리..

		template<class It>
		class operator_brackets_proxy
		{
			typedef typename It::reference	reference;
			typedef typename It::value_type value_type;
		public:
			operator_brackets_proxy(const It& i) : m_iter(i) {}
			operator reference() const { return *m_iter; }
			operator_brackets_proxy& operator=(const value_type& val) { *m_iter = val; return *this; }
		private:
			It		m_iter;
		};
		
		template<class Val>
		struct use_operator_brackets_proxy : mpl::not_< mpl::and_< i3::is_pod<Val>, i3::is_const<Val> > > {};

		template<class It, class Val>
		struct operator_brackets_result
		{
			typedef typename mpl::if_<	use_operator_brackets_proxy<Val>, // const pod가 아니라면..
										operator_brackets_proxy<It>,
										Val>::type		type;
		};
		
		template<class It> inline
		operator_brackets_proxy<It> make_operator_brackets_result(const It& it, mpl::true_) { return operator_brackets_proxy<It>(it); }
		template<class It> inline
		typename It::value_type		make_operator_brackets_result(const It& it, mpl::false_){ return *it; }


		template<class It1, class It2>
		struct choose_difference_type : mpl::eval_if<is_convertible<It2, It1>, iterator_difference<It1>, iterator_difference<It2> > {};
		

		template<class A, class B>
		struct is_interoperable : mpl::or_< is_convertible<A, B>, is_convertible<B, A> > {};

		template<class A, class B, class Ret>
		struct enable_if_interoperable : enable_if< is_interoperable<A, B>, Ret> {};

	}

}