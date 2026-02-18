#pragma once

#include "ft_arity_limit.h"

#include "../mpl/mpl_vector_n.h"
#include "../mpl/mpl_placeholders.h"


#include "ft_tag.h"
#include "ft_cc_tag.h"
#include "ft_class_transform.h"

namespace i3
{
	namespace ft
	{
		using mpl::placeholders::_;

		template<class T, class ClassTransform = add_reference<_> > 
		struct components;

		namespace detail {	template<class T, class L> struct components_impl;	}
		
		template<class T, class ClassTransform>
		struct components : detail::components_impl<T, ClassTransform>
		{
			typedef components<T, ClassTransform>	type;
		};
		
		// components가 기본클래스로 비트태그를 내장하는게 더 편할듯 싶다..
	
		struct components_mpl_sequence_tag;

		struct components_non_func : null_tag
		{
			typedef components_mpl_sequence_tag			tag;
			typedef mpl::integral_c<std::size_t, 0>		function_arity;
			typedef mpl::vector0<>						types;
		};
		
	

		namespace detail
		{
			template<class Components, class IfTag, class ThenTag, class Else = components_non_func>
			struct retagged_if : mpl::if_<	is_bits_tag_match< Components, IfTag>,
											changed_bits_tag<Components, IfTag, ThenTag>,
											Else >::type {};
			
			template<class T, class L>
			struct components_impl : retagged_if< components_impl<T*, L>, function_ptr_tag, function_tag> {};		// 함수포인터를 만족하면, 순함수형 태그로 전환..
			
			template<class T, class L>
			struct components_impl<T&, L> : retagged_if< components_impl<T*, L>, function_ptr_tag, function_ref_tag> {};	// 함수포인터를 만족시, 참조함수형으로 전환..

			template<class T, class L>
			struct components_impl<T*, L> : components_non_func {};			// 함수아닌 포인터의 경우는 함수아님으로 표기..


			template<class T, class L>										// 포인터 한정자의 경우, 한정자를 뗌...
			struct components_impl<T* const, L> : components_impl<T*, L> {};	
			template<class T, class L>
			struct components_impl<T* volatile, L> : components_impl<T*, L> {};	
			template<class T, class L>
			struct components_impl<T* const volatile, L> : components_impl<T*, L> {};	
			
			template<class T, class L>								// 값 한정자의 경우, 한정자를 뗌...
			struct components_impl<T const, L> : components_impl<T,L> {};
			template<class T, class L>
			struct components_impl<T volatile, L> : components_impl<T,L>{};
			template<class T, class L>
			struct components_impl<T const volatile, L> : components_impl<T,L> {};

			template<class T, class C> struct member_obj_ptr_result	{ typedef T & type; };
			template<class T, class C> struct member_obj_ptr_result<T, C const> { typedef T const & type; };
			template<class T, class C> struct member_obj_ptr_result<T, C volatile> 	{ typedef T volatile & type; };
			template<class T, class C> struct member_obj_ptr_result<T, C const volatile> { typedef T const volatile & type; };
			template<class T, class C> struct member_obj_ptr_result<T &, C>	{ typedef T & type; };
			template<class T, class C> struct member_obj_ptr_result<T &, C const> { typedef T & type; };
			template<class T, class C> struct member_obj_ptr_result<T &, C volatile> { typedef T & type; };
			template<class T, class C> struct member_obj_ptr_result<T &, C const volatile> { typedef T & type; };

			template<class T, class C, class L>
			struct member_obj_ptr_components : member_object_ptr_base_tag
			{
				typedef components_mpl_sequence_tag tag;
				typedef mpl::integral_c<std::size_t, 1>		function_arity;
				typedef mpl::vector2< typename member_obj_ptr_result<T,C>::type, typename class_transform<C,L>::type > types;
			};
			
			template<class T, class C, class L>
			struct components_impl< T C::*, L> : member_obj_ptr_components<T, C, L> {};			// 멤버변수 포인터....(클래스 자체를 1개의 인수로 취한다..)

		}			


	}
}



#include "detail/pp_ft_components_impl.h"
#include "detail/ft_components_as_mpl_sequence.h"
#include "detail/ft_retag_default_cc.h"
