#pragma once

#include "../mpl/mpl_list.h"
#include "../mpl/mpl_find_if.h"
#include "../mpl/mpl_if.h"
#include "../mpl/mpl_identity.h"
#include "../mpl/mpl_deref.h"
#include "../mpl/mpl_begin_end.h"

#include "../mpl/mpl_front.h"
#include "../mpl/mpl_pop_front.h"
#include "../mpl/mpl_empty.h"
#include "../mpl/mpl_equal_to.h"
#include "../mpl/mpl_sizeof.h"

#include "alignment_of.h"
#include "is_same.h"
#include "is_pod.h"

#pragma warning(push)
#pragma warning(disable: 4121) // alignment is sensitive to packing



namespace i3
{
	namespace detail
	{
		struct align_helper
		{
			class alignment_dummy;
			typedef void (*function_ptr)();
			typedef int (alignment_dummy::*member_ptr);
			typedef int (alignment_dummy::*member_function_ptr)();
			
//			struct abstract_class { virtual void fun(){} };		// 가상함수존재시 union이 안되는데..loki variant는 이것에 문제가 있다..
			
			template <class T> struct has_one_T { T data; };

			typedef mpl::list<	char, short, int, long, 
								long long, float, double, long double, 
								void*, function_ptr, member_ptr, member_function_ptr,
								has_one_T<char>, has_one_T<short>, has_one_T<int>, has_one_T<long>, 
								has_one_T<long long>, has_one_T<float>,	has_one_T<double>, has_one_T<long double>, 
								has_one_T<void*>, has_one_T<function_ptr>, has_one_T<member_ptr>, has_one_T<member_function_ptr> > alignment_list;
		};
		
/*
		template<class TList, size_t target>
		struct lower_alignment_helper;

		template<size_t target>
		struct lower_alignment_helper<null_type, target> {	typedef char	type;	};
		
		template<class h, class t, size_t target>
		struct lower_alignment_helper< TList< h, t>, target>
		{
			typedef typename lower_alignment_helper<t, target>::type	tail_type;
			typedef typename select< (alignment_of<h>::value == target), h, tail_type>::type		type;
						
		};
*/
		using namespace mpl::placeholders;

		template<class Seq, size_t target>
		struct lower_alignment_helper 
		{
			typedef	typename mpl::find_if<Seq, mpl::equal_to< mpl::sizeof_<_>, mpl::size_t<target> > >::type found_iter;
			typedef typename mpl::eval_if<	is_same<found_iter, typename mpl::end<Seq>::type>, 
													mpl::identity<char>, mpl::deref<found_iter> >::type type;
		};

		template<class Seq, bool IsEmpty = false> 
		union alignment_union {};

		template<class Seq>
		union alignment_union<Seq, false>
		{
			typename mpl::front<Seq>::type		data;	
			typedef typename mpl::pop_front<Seq>::type	next_sequence;
			alignment_union<next_sequence, mpl::empty<next_sequence>::value> next_union;
		};

//		template<> union alignment_union<null_type> {};
//		template<class head, class tail>
//		union alignment_union< TList<head, tail> > {	head data;		alignment_union<tail> next; };
		

	}
	
	template<size_t target>
	union lower_alignment {	typename detail::lower_alignment_helper<detail::align_helper::alignment_list, target>::type	data;  };

	union max_align {  detail::alignment_union<detail::align_helper::alignment_list> data;   };
//	union max_align : detail::alignment_union<detail::align_helper::alignment_tlist>{};		// union은 상속을 가질수 없다..
		
	template<size_t type_align, size_t target_align>
	struct is_aligned : integral_constant< bool, ( (type_align >= target_align) & ( type_align % target_align == 0 ) )> {};
	
	template<size_t align>
	struct is_pod< lower_alignment<align> > : true_type {};
	
	namespace detail
	{
		template<size_t align>
		struct type_with_alignment_impl
		{
			typedef lower_alignment<align>		lower_alignment_type;
			typedef typename mpl::if_c< ( is_aligned< alignment_of<lower_alignment_type>::value, align>::value), 
										lower_alignment_type, max_align >::type type;
		};
	}

	template<size_t align> struct type_with_alignment : public detail::type_with_alignment_impl<align> {};
	
	namespace align
	{
		struct __declspec(align(8)) a8 { char m[8]; typedef a8 type; };
		struct __declspec(align(16)) a16 { char m[16]; 	typedef a16 type; };
		struct __declspec(align(32)) a32 { char m[32]; 	typedef a32 type; };
		struct __declspec(align(64)) a64 { char m[64]; 	typedef a64 type; };
		struct __declspec(align(128)) a128 { char m[128]; typedef a128 type; };
	}
	// MSVC에 관련된 특화가 좀더 있으니 부스트에서 확인하고 추가해야함...

	template<> struct type_with_alignment<8>  
	{ 
		typedef mpl::if_c< alignment_of<max_align>::value < 8, align::a8, detail::type_with_alignment_impl<8>::type >::type type; 
	};
	template<> struct type_with_alignment<16> 
	{ 
		typedef mpl::if_c< alignment_of<max_align>::value < 16, align::a16, detail::type_with_alignment_impl<16>::type >::type type;
	};
	template<> struct type_with_alignment<32> 
	{ 
		typedef mpl::if_c< alignment_of<max_align>::value < 32, align::a32, detail::type_with_alignment_impl<32>::type >::type type; 
	};

	template<> struct type_with_alignment<64> 
	{
		typedef mpl::if_c< alignment_of<max_align>::value < 64, align::a64, detail::type_with_alignment_impl<64>::type >::type type; 
	};

	template<> struct type_with_alignment<128> 
	{
		typedef mpl::if_c< alignment_of<max_align>::value < 128, align::a128, detail::type_with_alignment_impl<128>::type >::type type; 
	};
	
	template<> struct is_pod< align::a8 > : true_type {};
	template<> struct is_pod< align::a16 > : true_type {};
	template<> struct is_pod< align::a32 > : true_type {};
	template<> struct is_pod< align::a64 > : true_type {};
	template<> struct is_pod< align::a128 > : true_type {};
	


}

#pragma warning(pop)