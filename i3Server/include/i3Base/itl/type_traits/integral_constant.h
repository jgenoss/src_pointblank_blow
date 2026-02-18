#pragma once

#include "../mpl/mpl_bool.h"
#include "../mpl/mpl_integral_c.h"

namespace i3
{
	
	template <class T, T val>
	struct integral_constant : public mpl::integral_c<T, val>
	{
		typedef integral_constant<T,val> type;
	};
	
	template<> struct integral_constant<bool,true> : public mpl::true_ 
	{
		typedef integral_constant<bool,true> type;
	};

	template<> struct integral_constant<bool,false> : public mpl::false_ 
	{
		typedef integral_constant<bool,false> type;
	};

	typedef integral_constant<bool,true> true_type;
	typedef integral_constant<bool,false> false_type;
		
	
}
