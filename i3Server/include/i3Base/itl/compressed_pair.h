#pragma once

//
//  boost의 compressed_pair와 같음....구현상 몇가지 임의적 부분만 직관적으로 수정할 것..
//
#include "type_traits/call_traits.h"
#include "type_traits/is_same.h"
#include "type_traits/is_empty.h"

namespace i3
{

	template <class T1, class T2> class compressed_pair;

	namespace detail
	{
		template<class T1, class T2>
		class compressed_pair_normal2
		{
		public:
			typedef T1			first_type;			typedef T2					second_type;
			typedef typename call_traits<first_type>::param_type				first_param_type;
			typedef typename call_traits<second_type>::param_type				second_param_type;
			typedef typename call_traits<first_type>::reference					first_reference;
			typedef typename call_traits<second_type>::reference				second_reference;
			typedef typename call_traits<first_type>::const_reference			first_const_reference;
			typedef typename call_traits<second_type>::const_reference			second_const_reference;
			
			typedef compressed_pair_normal2<T1, T2>								my_type;
			my_type() {}
			my_type(first_param_type f, second_param_type s) : m_first(f), m_second(s) {}
			my_type(first_param_type f) : m_first(f) {}	my_type(second_param_type s) : m_second(s) {}
			my_type(const my_type& rhs) : m_first(rhs.first()), m_second(rhs.second()) {}

			first_reference				first()			{ return m_first;}
			first_const_reference		first() const	{ return m_first; }
			second_reference			second()		{ return m_second; }
			second_const_reference		second() const	{ return m_second; }
			
			void swap( i3::compressed_pair<T1, T2>& rhs) {  i3::swap(m_first, rhs.first());	i3::swap(m_second, rhs.second());  }
			void assign( const my_type& rhs ) { m_first = rhs.first();  m_second = rhs.second(); }

		private:
			first_type		m_first;
			second_type		m_second;
		};
		
		template <class T1, class T2>
		class compressed_pair_empty_normal : protected i3::remove_cv<T1>::type
		{
		public:
			typedef T1			first_type;			typedef T2					second_type;
			typedef typename call_traits<first_type>::param_type				first_param_type;
			typedef typename call_traits<second_type>::param_type				second_param_type;
			typedef typename call_traits<first_type>::reference					first_reference;
			typedef typename call_traits<second_type>::reference				second_reference;
			typedef typename call_traits<first_type>::const_reference			first_const_reference;
			typedef typename call_traits<second_type>::const_reference			second_const_reference;

			typedef compressed_pair_empty_normal<T1, T2>						my_type;
			
			my_type() {}
			my_type(first_param_type f, second_param_type s) : first_type(f), m_second(s) {}
			my_type(first_param_type f) : first_type(f) {}	my_type(second_param_type s) : m_second(s) {}
			my_type(const my_type& rhs) : m_second(rhs.second()) {}

			first_reference				first()			{ return *this;}
			first_const_reference		first() const	{ return *this;}
			second_reference			second()		{ return m_second; }
			second_const_reference		second() const	{ return m_second; }

			void swap( i3::compressed_pair<T1, T2>& rhs) {  i3::swap(m_second, rhs.second());  }
			void assign( const my_type& rhs ) {  m_second = rhs.second(); }

		private:
			second_type		m_second;
		};
		
		template <class T1, class T2>
		class compressed_pair_normal_empty : protected i3::remove_cv<T2>::type
		{
		public:
			typedef T1			first_type;			typedef T2					second_type;
			typedef typename call_traits<first_type>::param_type				first_param_type;
			typedef typename call_traits<second_type>::param_type				second_param_type;
			typedef typename call_traits<first_type>::reference					first_reference;
			typedef typename call_traits<second_type>::reference				second_reference;
			typedef typename call_traits<first_type>::const_reference			first_const_reference;
			typedef typename call_traits<second_type>::const_reference			second_const_reference;

			typedef compressed_pair_normal_empty<T1, T2>						my_type;
			my_type() {}
			my_type(first_param_type f, second_param_type s) : second_type(s), m_first(f) {}
			my_type(first_param_type f) : m_first(f) {}	my_type(second_param_type s) : second_type(s) {}
			my_type(const my_type& rhs) : m_first(rhs.first()) {}

			first_reference				first()			{ return m_first;}
			first_const_reference		first() const	{ return m_first;}
			second_reference			second()		{ return *this; }
			second_const_reference		second() const	{ return *this; }

			void swap( i3::compressed_pair<T1, T2>& rhs) {  i3::swap(m_first, rhs.first());  }
			void assign( const my_type& rhs ) { m_first = rhs.first();  }

		private:
			first_type		m_first;
		};
		
		template<class T1, class T2>
		class compressed_pair_empty2 : protected i3::remove_cv<T1>::type, protected i3::remove_cv<T2>::type
		{
		public:
			typedef T1			first_type;			typedef T2					second_type;
			typedef typename call_traits<first_type>::param_type				first_param_type;
			typedef typename call_traits<second_type>::param_type				second_param_type;
			typedef typename call_traits<first_type>::reference					first_reference;
			typedef typename call_traits<second_type>::reference				second_reference;
			typedef typename call_traits<first_type>::const_reference			first_const_reference;
			typedef typename call_traits<second_type>::const_reference			second_const_reference;
			typedef compressed_pair_empty2<T1, T2>								my_type;

			my_type() {}
			my_type(first_param_type f, second_param_type s) : first_type(f), second_type(s) {}
			my_type(first_param_type f) : first_type(f) {}	my_type(second_param_type s) : second_type(s) {}
			my_type(const my_type& rhs) {}

			first_reference       first()			{return *this;}
			first_const_reference first() const		{return *this;}
			second_reference       second()			{return *this;}
			second_const_reference second() const	{return *this;}
			void swap(i3::compressed_pair<T1,T2>&) {}
			void assign( const my_type& rhs ) {}

		};

		template<class T1, class T2>
		class compressed_pair_same_empty2 : protected i3::remove_cv<T1>::type 
		{
		public:
			typedef T1		first_type;			typedef T2						second_type;
			typedef typename call_traits<first_type>::param_type				first_param_type;
			typedef typename call_traits<second_type>::param_type				second_param_type;
			typedef typename call_traits<first_type>::reference					first_reference;
			typedef typename call_traits<second_type>::reference				second_reference;
			typedef typename call_traits<first_type>::const_reference			first_const_reference;
			typedef typename call_traits<second_type>::const_reference			second_const_reference;
			typedef compressed_pair_same_empty2<T1, T2>								my_type;

			my_type() {}
			my_type(first_param_type f, second_param_type s) : first_type(f), m_second(s) {}
			my_type(first_param_type f) : first_type(f), m_second(f) {}	
			my_type(const my_type& rhs) {}

			first_reference       first()       {return *this;}
			first_const_reference first() const {return *this;}

			second_reference       second()       {return m_second;}
			second_const_reference second() const {return m_second;}

			void swap(i3::compressed_pair<T1,T2>&) {}
			void assign( const my_type& rhs ) {}
		private:
			T2		m_second;
		};

		template<class T1, class T2>
		class compressed_pair_same_normal2 : protected i3::remove_cv<T1>::type 
		{
		public:
			typedef T1		first_type;			typedef T2						second_type;
			typedef typename call_traits<first_type>::param_type				first_param_type;
			typedef typename call_traits<second_type>::param_type				second_param_type;
			typedef typename call_traits<first_type>::reference					first_reference;
			typedef typename call_traits<second_type>::reference				second_reference;
			typedef typename call_traits<first_type>::const_reference			first_const_reference;
			typedef typename call_traits<second_type>::const_reference			second_const_reference;
			typedef compressed_pair_same_normal2<T1, T2>						my_type;
			
			my_type() {}
			my_type(first_param_type f, second_param_type s) : m_first(f), m_second(s) {}
			my_type(first_param_type f) : m_first(f), m_second(f) {}	
			my_type(const my_type& rhs) : m_first(rhs.first()), m_second(rhs.second()) {}

			first_reference       first()       {return m_first;}
			first_const_reference first() const {return m_first;}
			second_reference       second()       {return m_second;}
			second_const_reference second() const {return m_second;}

			void swap(i3::compressed_pair<T1,T2>& rhs) {   i3::swap(m_first, rhs.first());	i3::swap(m_second, rhs.second()); }
			void assign( const my_type& rhs ) { m_first = rhs.first();	m_second = rhs.second(); }

		private:
			first_type	m_first;
			second_type	m_second;
		};


		template< bool is_same_type, bool is_T1_empty, bool is_T2_empty>
		struct compressed_pair_type_switch;

		template<>
		struct compressed_pair_type_switch<false, false, false>
		{
			template<class T1, class T2> struct rebind { typedef compressed_pair_normal2<T1, T2>	type;	};
		};
		
		template<>
		struct compressed_pair_type_switch<false, true, true>
		{
			template<class T1, class T2> struct rebind { typedef compressed_pair_empty2<T1, T2> type;  };
		};
		
		template<>
		struct compressed_pair_type_switch<false, true, false>
		{
			template<class T1, class T2> struct rebind { typedef compressed_pair_empty_normal<T1, T2> type; };
		};

		template<>
		struct compressed_pair_type_switch<false, false, true>
		{
			template<class T1, class T2> struct rebind { typedef compressed_pair_normal_empty<T1, T2> type; };
		};

		template<>
		struct compressed_pair_type_switch<true, true, true>
		{
			template<class T1, class T2> struct rebind { typedef compressed_pair_same_empty2<T1, T2> type; };
		};
			
		template<>
		struct compressed_pair_type_switch<true, false, false>
		{
			template<class T1, class T2> struct rebind { typedef compressed_pair_same_normal2<T1, T2> type; };
		};

		template<class T1, class T2>
		struct compressed_pair_selector
		{
			typedef typename compressed_pair_type_switch< 
				is_same< typename remove_cv<T1>::type, typename remove_cv<T2>::type>::value, 
				is_empty<T1>::value, is_empty<T2>::value >::template rebind<T1, T2>::type   type;
		};
	}

	template <class T1, class T2>
	class compressed_pair : private detail::compressed_pair_selector<T1, T2>::type
	{
	public:
		typedef typename detail::compressed_pair_selector<T1, T2>::type		base_type;
		typedef T1															first_type;
		typedef T2															second_type;
		typedef typename call_traits<first_type>::param_type				first_param_type;
		typedef typename call_traits<second_type>::param_type				second_param_type;
		typedef typename call_traits<first_type>::reference					first_reference;
		typedef typename call_traits<second_type>::reference				second_reference;
		typedef typename call_traits<first_type>::const_reference			first_const_reference;
		typedef typename call_traits<second_type>::const_reference			second_const_reference;
		
		compressed_pair() : base_type() {}

		template<class U1, class U2>
		compressed_pair(const compressed_pair<U1, U2>& rhs) : base_type(rhs) {}

		compressed_pair(first_param_type f, second_param_type s) : base_type(f, s) {}
		explicit compressed_pair(first_param_type f) : base_type(f) {}
		explicit compressed_pair(second_param_type s) : base_type(s) {}
		
		template<class U1, class U2>
		compressed_pair&		operator=(const compressed_pair<U1, U2>& rhs) { base_type::assign(rhs);	return *this;  }

		first_reference			first()       {return base_type::first();}
		first_const_reference	first() const {return base_type::first();}
		second_reference		second()       {return base_type::second();}
		second_const_reference	second() const {return base_type::second();}

		void swap(compressed_pair& rhs) { base_type::swap(rhs); } 
	};
	
	
	template <class T>
	class compressed_pair<T, T> : private detail::compressed_pair_selector<T, T>::type
	{
	public:
		typedef typename detail::compressed_pair_selector<T, T>::type	   base_type;
		typedef T                                                  first_type;
		typedef T                                                  second_type;
		typedef typename call_traits<first_type>::param_type       first_param_type;
		typedef typename call_traits<second_type>::param_type      second_param_type;
		typedef typename call_traits<first_type>::reference        first_reference;
		typedef typename call_traits<second_type>::reference       second_reference;
		typedef typename call_traits<first_type>::const_reference  first_const_reference;
		typedef typename call_traits<second_type>::const_reference second_const_reference;

		compressed_pair() : base_type() {}
		template<class U1, class U2>
		compressed_pair(const compressed_pair<U1, U2>& rhs) : base_type(rhs) {}

		compressed_pair(first_param_type f, second_param_type s) : base_type(f, s) {}
		explicit compressed_pair(first_param_type f) : base_type(f) {}
	
		template<class U1, class U2>
		compressed_pair&		operator=(const compressed_pair<U1, U2>& rhs) { base_type::assign(rhs);	return *this;  }

		first_reference       first()       {return base_type::first();}
		first_const_reference first() const {return base_type::first();}

		second_reference       second()       {return base_type::second();}
		second_const_reference second() const {return base_type::second();}

		void swap(compressed_pair& rhs) { base_type::swap(rhs); }
	};

	template <class T1, class T2>
	inline void swap(compressed_pair<T1, T2>& lhs, compressed_pair<T1, T2>& rhs) {	lhs.swap(rhs);	}

	
	
}	