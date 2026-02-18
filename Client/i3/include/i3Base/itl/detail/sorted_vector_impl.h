#pragma once

#include "../type_traits/call_traits.h"
#include "../pair.h"
#include "../ebo.h"
#include "../enable_if.h"
#include "../type_traits/is_convertible.h"

namespace i3
{
	namespace detail
	{
		template<class Val, class Pred>
		struct sorted_vector_compare : public Pred
		{
			typedef Val															value_type;
			typedef typename i3::make_parameter<const value_type>::type			value_param;
			sorted_vector_compare(){}
			sorted_vector_compare(const Pred& pr) : Pred(pr) {}
			bool  operator()( value_param lhs, value_param rhs) const { return Pred::operator()(lhs, rhs); }

			template<class Other> typename i3::disable_if< i3::is_convertible<Other, value_type> ,
			bool>::type  operator()(const Other& lhs, value_param rhs) const { return Pred::operator()(lhs, rhs);  }

			template<class Other> typename i3::disable_if< i3::is_convertible<Other, value_type> ,
			bool>::type  operator()(value_param lhs, const Other& rhs) const { return Pred::operator()(lhs, rhs);  }

		};

		template<class Key, class Val, class Pred>
		struct sorted_vector_compare< i3::pair< Key, Val>, Pred> : public Pred
		{
			typedef i3::pair< Key, Val>										value_type;
			typedef typename i3::make_parameter<const Key>::type			key_param;
			typedef typename i3::make_parameter<const value_type>::type		value_param;

			sorted_vector_compare(){}
			sorted_vector_compare(const Pred& pr) : Pred(pr) {}
			// 버전확장 (키/쌍비교함수추가..)
			bool  operator()( value_param lhs, value_param rhs) const { return Pred::operator()(lhs.first, rhs.first); }
			bool  operator()( value_param lhs, key_param rhs) const { return Pred::operator()(lhs.first, rhs);  }		
			
			template<class Other> typename i3::disable_if< mpl::or_<i3::is_convertible<Other, Key>, i3::is_convertible<Other, value_type> >,
			bool>::type  operator()( value_param lhs, const Other& rhs) const {  return Pred::operator()(lhs.first, rhs); }

			bool  operator()(key_param lhs, key_param rhs) const { return Pred::operator()(lhs, rhs);  }
			
			template<class Other> typename i3::disable_if< mpl::or_<i3::is_convertible<Other, Key>, i3::is_convertible<Other, value_type> >,
			bool>::type  operator()(const Other& lhs, key_param rhs) const { return Pred::operator()(lhs, rhs);  }

			template<class Other> typename i3::disable_if< mpl::or_<i3::is_convertible<Other, Key>, i3::is_convertible<Other, value_type> >,
			bool>::type  operator()(key_param lhs, const Other& rhs) const { return Pred::operator()(lhs, rhs);  }

			bool  operator()(key_param lhs, value_param rhs) const { return Pred::operator()(lhs, rhs.first); }
			
			template<class Other> typename i3::disable_if< mpl::or_<i3::is_convertible<Other, Key>, i3::is_convertible<Other, value_type> >,
			bool>::type  operator()(const Other& lhs, value_param rhs) const { return Pred::operator()(lhs, rhs.first); }
		};

	}
	
	template<class Val, class Pred>
	class sorted_vector_key : protected i3::enable_ebo< detail::sorted_vector_compare<Val, Pred> >
	{
	protected:
		typedef			sorted_vector_key<Val, Pred>	my_type;
		typedef			i3::enable_ebo< detail::sorted_vector_compare<Val, Pred> >		base_type;
		typedef			Val																value_type;
		typedef			Val																key_type;
		typedef			Pred															key_compare;
		typedef			Pred															value_compare;
		typedef			typename base_type::value_type									my_compare;

		my_type() {}
		my_type(const key_compare& pr) : base_type(pr) {}

		static __forceinline const value_type&	key(const value_type& val) { return val; }
	};

	template<class Key, class Val, class Pred>
	class sorted_vector_key< i3::pair< Key, Val>, Pred> : protected i3::enable_ebo< detail::sorted_vector_compare<i3::pair< Key, Val>, Pred> >
	{
	protected:
		typedef			sorted_vector_key<i3::pair< Key, Val>, Pred>	my_type;
		typedef			i3::enable_ebo< detail::sorted_vector_compare<i3::pair<Key, Val>, Pred> >		base_type;
		typedef			i3::pair< Key, Val>																value_type;
		typedef			Key																				key_type;
		typedef			Pred																			key_compare;
		typedef			typename base_type::value_type													my_compare;

		class value_compare	: public i3::binary_function<value_type, value_type, bool>
		{	
		public:
			value_compare(const key_compare& pr) : m_comp(pr) {}
			bool operator()(const value_type& lhs, const value_type& rhs) const {	return m_comp(lhs.first, rhs.first); }
		private:
			key_compare m_comp;	
		};

		my_type() {}
		my_type(const key_compare& pr) : base_type(pr) {}
		static __forceinline const key_type&	key(const value_type& val) { return val.first; }		
	};

	
	
}