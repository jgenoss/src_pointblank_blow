#pragma once


//#   define BOOST_MPL_AUX_MAP0_OVERLOAD(R, f, X, T) \
//  static R BOOST_PP_CAT(BOOST_MPL_AUX_OVERLOAD_,f)(X const&, T) \

//#   define BOOST_MPL_AUX_MAP_OVERLOAD(R, f, X, T) \
//    BOOST_MPL_AUX_MAP0_OVERLOAD(R, f, X, T); \
//    using Base::BOOST_PP_CAT(BOOST_MPL_AUX_OVERLOAD_,f) \


//#   define BOOST_MPL_AUX_SET0_OVERLOAD(R, f, X, T) \
//    static R BOOST_PP_CAT(BOOST_MPL_AUX_OVERLOAD_,f)(X const&, T) \


//#   define BOOST_MPL_AUX_SET_OVERLOAD(R, f, X, T) \
//    BOOST_MPL_AUX_SET0_OVERLOAD(R, f, X, T); \
//    using Base::BOOST_PP_CAT(BOOST_MPL_AUX_OVERLOAD_,f) \



//#   define BOOST_MPL_AUX_OVERLOAD_VALUE_BY_KEY  value_by_key_
//#   define BOOST_MPL_AUX_OVERLOAD_ITEM_BY_ORDER item_by_order_
//#   define BOOST_MPL_AUX_OVERLOAD_ORDER_BY_KEY  order_by_key_
//#   define BOOST_MPL_AUX_OVERLOAD_IS_MASKED     is_masked_

//#define BOOST_MPL_AUX_OVERLOAD_CALL_VALUE_BY_KEY(T, x)   T::value_by_key_( aux::ptr_to_ref(static_cast<T*>(0)), x )
//#define BOOST_MPL_AUX_OVERLOAD_CALL_ITEM_BY_ORDER(T, x)  T::item_by_order_( aux::ptr_to_ref(static_cast<T*>(0)), x )
//#define BOOST_MPL_AUX_OVERLOAD_CALL_ORDER_BY_KEY(T, x)   T::order_by_key_( aux::ptr_to_ref(static_cast<T*>(0)), x )
//#define BOOST_MPL_AUX_OVERLOAD_CALL_IS_MASKED(T, x)      T::is_masked_( aux::ptr_to_ref(static_cast<T*>(0)), x )


namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template<class T> static const T& ptr_to_ref(T*);
		}
	}
}
