#pragma once

#include "detail/tuple_ext_impl.h"



namespace i3
{
	
	template<int N, class T>
	struct is_tuple_index : integral_constant<bool, ( N < tuple_size<T>::value ) > {};
		
	template<class Tuple, class Functor> inline
	void for_each_tuple(Tuple& t, Functor& f) {	detail::for_each_tuple(t,f); }
	
	template<class Tuple, class Functor> inline
	void for_each_tuple(Tuple& t, const Functor& f)	{	detail::for_each_tuple(t,f); }

	template<class Tuple, class Functor> inline
	void for_each_tuple(const Tuple& t, Functor& f)	{	detail::for_each_tuple(t,f); }
	
	template<class Tuple, class Functor> inline
	void for_each_tuple(const Tuple& t, const Functor& f){	detail::for_each_tuple(t,f); }
	
	template<class MetaPred, class Tuple, class Functor> inline
	void for_each_tuple_if(Tuple& t, Functor& f) {	detail::for_each_tuple_if<MetaPred>(t, f);	}
	
	template<class MetaPred, class Tuple, class Functor> inline
	void for_each_tuple_if(Tuple& t, const Functor& f) {	detail::for_each_tuple_if<MetaPred>(t, f);	}

	template<class MetaPred, class Tuple, class Functor> inline
	void for_each_tuple_if(const Tuple& t, Functor& f) {	detail::for_each_tuple_if<MetaPred>(t, f);	}
	
	template<class MetaPred, class Tuple, class Functor> inline
	void for_each_tuple_if(const Tuple& t, const Functor& f) {	detail::for_each_tuple_if<MetaPred>(t, f);	}

}

