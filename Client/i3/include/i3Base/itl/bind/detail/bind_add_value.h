#pragma once

namespace i3
{
	namespace bi
	{
		template<class T, int I>	struct add_value_2 { typedef arg<I> type; };
		template<class T>			struct add_value_2<T, 0> { typedef bi::value<T> type; };
		
		template<class T>	struct add_value { typedef typename add_value_2<T, is_placeholder<T>::value>::type type; };
		template<class T>	struct add_value< value<T> > { typedef bi::value<T> type; };
		template<class T>	struct add_value< reference_wrapper<T> > { typedef reference_wrapper<T> type; };
		template<int I>		struct add_value< arg<I> > { typedef arg<I>	type; }; 
		template<int I>		struct add_value< arg<I> (*)() > { typedef arg<I>(*type)(); };
		template<class R, class F, class L> struct add_value<bind_t<R,F,L> > { typedef bind_t<R,F,L> type; };
	}
}
