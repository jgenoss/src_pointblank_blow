#pragma once

#include "mpl/mpl_if.h"
#include "mpl/mpl_identity.h"
#include "is_const.h"
#include "is_enum.h"
#include "is_volatile.h"
#include "remove_cv.h"

namespace i3
{
	namespace detail
	{
		template <class T> struct need_promotion : is_enum<T> {};
		template<> struct need_promotion<char              > : true_type {};
		template<> struct need_promotion<signed char       > : true_type {};
		template<> struct need_promotion<unsigned char     > : true_type {};
		template<> struct need_promotion<signed short int  > : true_type {};
		template<> struct need_promotion<unsigned short int> : true_type {};
		template<> struct need_promotion<long long>          : integral_constant<bool, (sizeof(long long) < sizeof(int))> {};
		template<> struct need_promotion<unsigned long long> : integral_constant<bool, (sizeof(unsigned long long) < sizeof(int))> {};
		
		template<> struct need_promotion<wchar_t> : true_type {};
		template<> struct need_promotion<bool> : true_type {};

		template<int Index, int IsConst, int IsVolatile> struct promote_from_index;

		template<> struct promote_from_index<1,0,0> { typedef int type; };           
		template<> struct promote_from_index<1,0,1> { typedef int volatile type; };  
		template<> struct promote_from_index<1,1,0> { typedef int const type; };     
		template<> struct promote_from_index<1,1,1> { typedef int const volatile type; };
		
		template<> struct promote_from_index<2,0,0> { typedef unsigned int type; };           
		template<> struct promote_from_index<2,0,1> { typedef unsigned int volatile type; };  
		template<> struct promote_from_index<2,1,0> { typedef unsigned int const type; };     
		template<> struct promote_from_index<2,1,1> { typedef unsigned int const volatile type; };
		
		template<> struct promote_from_index<3,0,0> { typedef long type; };           
		template<> struct promote_from_index<3,0,1> { typedef long volatile type; };  
		template<> struct promote_from_index<3,1,0> { typedef long const type; };     
		template<> struct promote_from_index<3,1,1> { typedef long const volatile type; };
		
		template<> struct promote_from_index<4,0,0> { typedef unsigned long type; };           
		template<> struct promote_from_index<4,0,1> { typedef unsigned long volatile type; };  
		template<> struct promote_from_index<4,1,0> { typedef unsigned long const type; };     
		template<> struct promote_from_index<4,1,1> { typedef unsigned long const volatile type; };
		
		template<> struct promote_from_index<5,0,0> { typedef long long type; };           
		template<> struct promote_from_index<5,0,1> { typedef long long volatile type; };  
		template<> struct promote_from_index<5,1,0> { typedef long long const type; };     
		template<> struct promote_from_index<5,1,1> { typedef long long const volatile type; };
		
		template<> struct promote_from_index<6,0,0> { typedef unsigned long long type; };           
		template<> struct promote_from_index<6,0,1> { typedef unsigned long long volatile type; };  
		template<> struct promote_from_index<6,1,0> { typedef unsigned long long const type; };     
		template<> struct promote_from_index<6,1,1> { typedef unsigned long long const volatile type; };
		
		char (&promoted_index_tester(int))[1];
		char (&promoted_index_tester(unsigned int))[2];
		char (&promoted_index_tester(long))[3];
		char (&promoted_index_tester(unsigned long))[4];
		char (&promoted_index_tester(long long))[5];
		char (&promoted_index_tester(unsigned long long))[6];
		
		template<class T>
		struct promoted_index
		{
			static T testee; // undefined
			static const int value = sizeof(promoted_index_tester(+testee));			// + //
		};
		
		template<class T>
		struct integral_promotion_impl
		{
			typedef typename promote_from_index< detail::promoted_index<T>::value, is_const<T>::value, is_volatile<T>::value >::type type;
		};
		
		template<class T>
		struct integral_promotion 
		  : mpl::eval_if< need_promotion<typename remove_cv<T>::type>, integral_promotion_impl<T>, mpl::identity<T> > {};

	}

	template<class T>
	struct integral_promotion { typedef typename detail::integral_promotion<T>::type type; };

}