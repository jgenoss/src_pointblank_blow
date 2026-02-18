#if !defined(pp_is_iterating)

#ifndef tuple_function_h
#define tuple_function_h


#include "../type_traits/add_const.h"
#include "../type_traits/object_generator_traits.h"

#include "../preprocessor/ext/pp_enum_params_with_a_cat.h"

namespace i3
{
	template<class T>
	struct tuple_size : integral_constant<std::size_t, 0> {};

	template<pp_enum_params( tuple_limit, class T)>
	struct tuple_size< tuple< pp_enum_params(tuple_limit, T) > >
		: integral_constant< std::size_t, tuple<pp_enum_params(tuple_limit,T)>::sequence_size> {}; 
	
	template<pp_enum_params( tuple_limit, class T)>
	struct tuple_size< tuple< pp_enum_params(tuple_limit, T) > const >
		: integral_constant< std::size_t, tuple<pp_enum_params(tuple_limit,T)>::sequence_size> {}; 
	
	template<class T1, class T2>
	struct tuple_size< i3::pair<T1, T2> > : integral_constant<std::size_t, 2> {};

	template<int N, class T>
	struct tuple_element; 

	template<int N, pp_enum_params( tuple_limit, class T)>
	struct tuple_element< N, tuple< pp_enum_params(tuple_limit, T) > > 
	{ 
		typedef typename tuple< pp_enum_params(tuple_limit, T) >::template element<N> element_type;
		typedef typename element_type::type		type;
	};

	template<int N, pp_enum_params( tuple_limit, class T)>
	struct tuple_element< N, tuple< pp_enum_params(tuple_limit, T) > const > 
	{ 
		typedef typename tuple< pp_enum_params(tuple_limit, T) >::template element<N> element_type;
		typedef	typename add_const<typename element_type::type>::type	type;
	};

	template <class T1, class T2> 
	struct tuple_element<0, i3::pair<T1, T2> > {   typedef typename i3::pair<T1, T2>::first_type type; 	};

	template <class T1, class T2> 
	struct tuple_element<1, i3::pair<T1, T2> > {   typedef typename i3::pair<T1, T2>::second_type type; };

	

	template<int N, pp_enum_params(tuple_limit, class T)> inline 
	typename make_reference_result< typename tuple<pp_enum_params(tuple_limit, T)>::template element<N>::type >::type 
	get(tuple<pp_enum_params(tuple_limit, T)>& t)
	{
		return t.get<N>();
	}
	
	template<int N, pp_enum_params(tuple_limit, class T)> inline 
	typename make_const_reference_result< typename tuple<pp_enum_params(tuple_limit, T)>::template element<N>::type >::type 
	get(const tuple<pp_enum_params(tuple_limit, T)>& t)
	{
		return t.get<N>();
	}
	
	template<class Data, pp_enum_params(tuple_limit, class T)> inline 
	typename make_reference_result<Data>::type
	get(tuple<pp_enum_params(tuple_limit, T)>& t)
	{
		return t.get<Data>();
	}

	template<class Data, pp_enum_params(tuple_limit, class T)> inline 
	typename make_const_reference_result<Data>::type
	get(const tuple<pp_enum_params(tuple_limit, T)>& t)
	{
		return t.get<Data>();
	}	
	
	namespace detail
	{
		// 템플릿 함수의 경우... 부분특화가 안되므로, 함수명오른쪽에 <>대신에, 정수래퍼를 활용한다..

		template<class T1, class T2> inline
		typename make_reference_result<T1>::type pair_get( i3::pair<T1, T2>& p, const true_type&)		// 0여부에 따라..
		{
			return p.first;
		}

		template<class T1, class T2> inline
		typename make_reference_result<T2>::type pair_get( i3::pair<T1, T2>& p, const false_type&)
		{
			return p.second;
		}

		template<class T1, class T2> inline
		typename make_const_reference_result<T1>::type pair_get(const i3::pair<T1, T2>& p, const true_type&)		// 0여부에 따라..
		{
			return p.first;
		}

		template<class T1, class T2> inline
		typename make_const_reference_result<T2>::type pair_get(const i3::pair<T1, T2>& p, const false_type&)	
		{
			return p.second;
		}
	}
	
	template<int N, class T1, class T2> inline
	typename make_reference_result< typename mpl::if_c<N ==0,T1,T2>::type >::type get(i3::pair<T1, T2>& p) 
	{
		return detail::pair_get(p, integral_constant<bool, (N == 0)>());
	}
	
	template<int N, class T1, class T2> inline
	typename make_const_reference_result< typename mpl::if_c<N ==0,T1,T2>::type >::type get(const i3::pair<T1, T2>& p) 
	{
		return detail::pair_get(p, integral_constant<bool, (N == 0)>());
	}
	
	const swallow_assign ignore = swallow_assign();
	
	template< pp_enum_params_with_a_default(tuple_limit, class T, mpl::na) >
	struct make_tuple_result
	{
		typedef tuple< pp_enum_params_with_a_cat(tuple_limit, typename object_generator_traits<T, >::type)> type;
	};
	
	inline tuple<> make_tuple() { return tuple<>(); }

	template<class T0> inline 
	typename make_tuple_result<T0>::type make_tuple(const T0& t0)
	{
		return typename make_tuple_result<T0>::type(t0);
	}
	
	template<class T0, class T1> inline
	typename make_tuple_result<T0, T1>::type make_tuple(const T0& t0, const T1& t1)
	{
		return typename make_tuple_result<T0, T1>::type(t0, t1);
	}
	
	inline tuple<> tie() { return tuple<>(); }

	template<class T0> inline 
	tuple<T0&> tie(T0& t0) {	return tuple<T0&>(t0);	}

	template<class T0, class T1> inline 
	tuple<T0&, T1&> tie(T0& t0, T1& t1) {	return tuple<T0&, T1&>(t0, t1);	}
	
	#define		pp_iteration_params_1 (3, (3, tuple_limit, "tuple_function.h"))		// T2 ~ T(limit-1) //
	#include	pp_iterate()
		
	
}

#endif // tuple_function_h

#else // pp_is_iterating

#define		i_		pp_frame_iteration(1)
	
	template<pp_enum_params(i_, class T)> inline
	typename make_tuple_result<pp_enum_params(i_, T)>::type make_tuple( pp_enum_params_cat_params(i_, const T, &, t) )
	{
		return typename make_tuple_result<pp_enum_params(i_, T)>::type(pp_enum_params(i_, t));
	}
	
	template<pp_enum_params(i_, class T)> inline
	tuple<pp_enum_params_with_a_cat(i_, T, &)> tie( pp_enum_params_cat_params(i_, T, &, t) )
	{
		return tuple<pp_enum_params_with_a_cat(i_, T, &)>(pp_enum_params(i_, t));
	}

#undef		i_

#endif // pp_is_iterating