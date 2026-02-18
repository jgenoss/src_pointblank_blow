#pragma once
/*
#include "iterator.h"
#include "type_traits/remove_pointer.h"
#include "type_traits/is_base_of.h"
#include "mpl/mpl_multi_if.h"
*/

#include "type_traits/is_memcpyable.h"
#include "type_traits/is_base_of.h"
#include "iterator.h"

namespace i3
{
	
	// POINTER ITERATOR TAGS
	struct nonscalar_ptr_iterator_tag { };	// pointer to unknown type
	struct scalar_ptr_iterator_tag { };		// pointer to scalar type
	

	namespace detail
	{
		
		template<class F, class T>
		struct is_linear_copyable_ptr_f_t
		{	
			typedef typename remove_pointer<F>::type			pointee_from;
			typedef typename remove_pointer<T>::type			pointee_to;
			typedef typename i3::is_memcpyable_from_to<pointee_from, pointee_to>::type type;
			static const bool value = type::value;
		};
				
		template<class T>
		struct extract_inner_type_detail {	typedef typename T::inner_type	type; };

		template<class T>
		struct extract_inner_type_if_iterator
		{
			typedef typename mpl::eval_if< is_base_of<iterator_base, T>, extract_inner_type_detail<T>, identity<T> >::type type;
		};

		
		template<class F, class T>
		struct is_linear_copyable_f_t_impl 
		{
			typedef typename extract_inner_type_if_iterator<F>::type	type_to_check_from;
			typedef typename extract_inner_type_if_iterator<T>::type	type_to_check_to;
			
			typedef typename mpl::if_<	mpl::and_< is_pointer<type_to_check_from>, is_pointer<type_to_check_to> >,
										is_linear_copyable_ptr_f_t<type_to_check_from, type_to_check_to>,
										false_type>::type type;
			static const bool value = type::value;
		};

	}

	template<class In, class Out = In>
	struct select_ptr_iterator_tag 
	{
		typedef typename mpl::if_< detail::is_linear_copyable_f_t_impl<In, Out>,
			scalar_ptr_iterator_tag,
			nonscalar_ptr_iterator_tag>::type	type;
	};


	/*
	namespace detail
	{
		
		template<class T, bool pointee_must_not_const>
		struct is_linear_copyable_impl;

		template<class T, bool pointee_must_not_const>
		struct is_linear_copyable_ptr
		{
			typedef typename remove_pointer<T>::type		pointee;
			typedef typename mpl::eval_if< is_const<pointee>, false_type, is_linear_copyable_ptr<T, false> >::type type;
		};
		
		template<class T>
		struct is_linear_copyable_ptr<T, false>
		{
			typedef typename remove_const<typename remove_pointer<T>::type>::type	pointee;
			typedef typename mpl::and_< has_trivial_assign<pointee>, has_trivial_copy<pointee> >::type and_result;
			typedef typename integral_constant<bool, and_result::value > type;
		};

		
		template<class T, bool pointee_must_not_const>
		struct is_linear_copyable_itr
		{
			typedef typename T::inner_type	inner_type;
			typedef typename mpl::eval_if<	is_same<inner_type, undefined_inner_type>, 
											false_type,
										is_linear_copyable_impl<inner_type, pointee_must_not_const> >::type		type;
		};
		
		template<class T, bool pointee_must_not_const>
		struct is_linear_copyable_impl : mpl::multi_eval_if<	is_pointer<T>, is_linear_copyable_ptr<T, pointee_must_not_const>,
													is_base_of<iterator_base, T>, is_linear_copyable_itr<T, pointee_must_not_const>,
													false_type >::type	
		{};
		
		
	}

	template<class In, class Out = In>
	struct select_ptr_iterator_tag;

	template<class In, class Out>
	struct select_ptr_iterator_tag 
	{
		typedef typename mpl::if_< mpl::and_<	detail::is_linear_copyable_impl<In, false>, 
										detail::is_linear_copyable_impl<Out, true> >,
									scalar_ptr_iterator_tag,
									nonscalar_ptr_iterator_tag>::type	type;
	};

	template<class In>
	struct select_ptr_iterator_tag<In, In> 
	{
		typedef typename mpl::if_<	detail::is_linear_copyable_impl<In, true>, 
							scalar_ptr_iterator_tag, 
							nonscalar_ptr_iterator_tag>::type		type;	
	};
	*/

/*
	namespace detail
	{
		// T1이 Input / T2가 Output쪽임..
		template<class Input, class Output, bool is_derived_of_iterator1, bool is_derived_of_iterator2>
		struct pointer_category_helper_iterator_check;
	
		template<class Input,	class Output>
		struct pointer_category_helper	
		{		
			enum{    
					is_derived_of_iterator1 = is_base_of<iterator_base, Input>::value,
					is_derived_of_iterator2 = is_base_of<iterator_base, Output>::value
				};
			
			typedef typename pointer_category_helper_iterator_check
				<Input, Output, is_derived_of_iterator1, is_derived_of_iterator2>::pointer_category 
				pointer_category;
		};
		
		template<class Input, class Output, bool is_derived_of_iterator1, bool is_derived_of_iterator2>
		struct pointer_category_helper_iterator_check
		{
			typedef typename Input::inner_type		inner_type_input;
			typedef typename Output::inner_type		inner_type_output;
			typedef typename pointer_category_helper<inner_type_input, inner_type_output>::pointer_category    pointer_category;	
		};

		template<class Input, class Output, bool is_derived_of_iterator1>
		struct pointer_category_helper_iterator_check<Input, Output, is_derived_of_iterator1, false>
		{
			typedef typename Input::inner_type		inner_type_input;
			typedef typename pointer_category_helper< inner_type_input, Output>::pointer_category pointer_category;
		};

		template<class Input, class Output, bool is_derived_of_iterator2>
		struct pointer_category_helper_iterator_check<Input, Output, false, is_derived_of_iterator2>
		{
			typedef typename Output::inner_type		inner_type_output;
			typedef typename pointer_category_helper< Input, inner_type_output>::pointer_category pointer_category;
		};
		
		template<class Input, class Output>
		struct pointer_category_helper_iterator_check<Input, Output, false, false>
		{
			typedef nonscalar_ptr_iterator_tag	pointer_category;
		};

		template<class T>
		struct pointer_category_helper<T*, T*>
		{
			enum { value =  ice_and<!is_const<T>::value, has_trivial_copy_constructor<T>::value, 
				has_trivial_assign<T>::value, has_trivial_destructor<T>::value>::value };
			typedef typename mpl::if_c<value, scalar_ptr_iterator_tag, nonscalar_ptr_iterator_tag>::type	pointer_category;
//			typedef nonscalar_ptr_iterator_tag pointer_category;
		};

		template<class T>
		struct pointer_category_helper<const T*, T*> : pointer_category_helper<T*, T*> {};
		
		
		
		template<class T1>	struct pointer_category_helper<T1, undefined_inner_type> {	typedef nonscalar_ptr_iterator_tag pointer_category; };
		template<class T2>	struct pointer_category_helper<undefined_inner_type, T2> {	typedef nonscalar_ptr_iterator_tag pointer_category; };
		template<>			struct pointer_category_helper<undefined_inner_type, undefined_inner_type> {	typedef nonscalar_ptr_iterator_tag pointer_category; };

		template<>	struct pointer_category_helper<bool *, bool *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const bool *, bool *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<char *, char *>	{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const char *, char *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<signed char *, signed char *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const signed char *, signed char *> {	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<unsigned char *, unsigned char *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const unsigned char *, unsigned char *>{	typedef scalar_ptr_iterator_tag pointer_category;	};

#ifdef _NATIVE_WCHAR_T_DEFINED
		template<>	struct pointer_category_helper<wchar_t *, wchar_t *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const wchar_t *, wchar_t *>	{	typedef scalar_ptr_iterator_tag pointer_category;	};
#endif

		template<>	struct pointer_category_helper<short *, short *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const short *, short *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<unsigned short *, unsigned short *>{	typedef scalar_ptr_iterator_tag pointer_category; };
		template<>	struct pointer_category_helper<const unsigned short *, unsigned short *>{	typedef scalar_ptr_iterator_tag pointer_category; };
		template<>	struct pointer_category_helper<int *, int *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const int *, int *>{	typedef scalar_ptr_iterator_tag pointer_category;  };
		template<>	struct pointer_category_helper<unsigned int *, unsigned int *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const unsigned int *, unsigned int *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<long *, long *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const long *, long *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<unsigned long *, unsigned long *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const unsigned long *, unsigned long *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<float *, float *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const float *, float *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<double *, double *>	{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const double *, double *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<long double *, long double *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const long double *, long double *>	{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<long long *, long long *>	{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const long long *, long long *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<unsigned long long *, unsigned long long *>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<>	struct pointer_category_helper<const unsigned long long *, unsigned long long *>{	typedef scalar_ptr_iterator_tag pointer_category; };
		template<class T>	struct pointer_category_helper<T **, T **>	{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<class T>	struct pointer_category_helper<T **, const T **>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<class T>	struct pointer_category_helper<T *const *, T **>{	typedef scalar_ptr_iterator_tag pointer_category;	};
		template<class T>	struct pointer_category_helper<T *const *, const T **>{	typedef scalar_ptr_iterator_tag pointer_category; };
	}
	
	template<class T1, class T2> inline 
	typename detail::pointer_category_helper<T1, T2>::pointer_category pointer_category(T1&, T2&)
	{
		typename detail::pointer_category_helper<T1, T2>::pointer_category category;
		return (category);
	}
	
	template<class T> inline
	typename detail::pointer_category_helper<T, T>::pointer_category pointer_category(T&)
	{
		typename detail::pointer_category_helper<T, T>::pointer_category category;
		return (category);
	}
*/	

	
}


