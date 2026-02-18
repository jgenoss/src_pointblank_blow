#pragma once

#include "../mpl_if.h"
#include "../mpl_int.h"

namespace i3
{
	namespace mpl
	{
		namespace aux
		{
			template< class T > struct integral_rank;

			template<> struct integral_rank<bool>           : int_<1> {};
			template<> struct integral_rank<signed char>    : int_<2> {};
			template<> struct integral_rank<char>           : int_<3> {};
			template<> struct integral_rank<unsigned char>  : int_<4> {};
			template<> struct integral_rank<wchar_t>        : int_<5> {};

			template<> struct integral_rank<short>          : int_<6> {};
			template<> struct integral_rank<unsigned short> : int_<7> {};
			template<> struct integral_rank<int>            : int_<8> {};
			template<> struct integral_rank<unsigned int>   : int_<9> {};
			template<> struct integral_rank<long>           : int_<10> {};
			template<> struct integral_rank<unsigned long>  : int_<11> {};

			template<> struct integral_rank<long long> : int_<12> {};
			template<> struct integral_rank<unsigned long long>: int_<13> {};


			template<class T1, class T2> 
			struct largest_int : if_c< ( integral_rank<T1>::value >= integral_rank<T2>::value ), T1, T2> {};		

		}
	}
}