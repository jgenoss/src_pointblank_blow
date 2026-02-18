#pragma once

#include "is_arithmetic.h"
#include "is_pointer.h"


namespace i3
{
	// make_parameter traits를 개선해야함(boost쪽 참고)
	namespace detail
	{
   		template <class T, bool is_small> struct ct_impl2			{	typedef const T param_type;	};
		template <class T> struct ct_impl2<T, false>				{   typedef const T& param_type; };

		template<class T, bool is_ptr, bool is_arith> struct ct_impl; 
		template<class T> struct ct_impl< T, false, false> { typedef const T&	param_type; };
		template<class T> struct ct_impl< T, false, true>  { typedef typename ct_impl2<T, sizeof(T) <= sizeof(void*)>::param_type param_type;};
		template<class T> struct ct_impl< T, true, false>  { typedef const T param_type; };
	}

	template <class T>
	struct call_traits
	{
		typedef T																						value_type;
		typedef T&																						reference;
		typedef const T&																				const_reference;
		typedef typename detail::ct_impl< T, i3::is_pointer<T>::value, i3::is_arithmetic<T>::value>::param_type	param_type;
	};
	
	template <class T>
	struct call_traits<T&>
	{
		typedef T&				value_type;
		typedef T&				reference;
		typedef const T&		const_reference;
		typedef T&				param_type;									
	};
	
	template <class T, std::size_t N>
	struct call_traits<T [N]>
	{
	private:
		typedef T array_type[N];
	public:
		// degrades array to pointer:
		typedef const T*			value_type;
		typedef array_type&			reference;
		typedef const array_type&	const_reference;
		typedef const T* const		param_type;
	};

	template <class T, std::size_t N>
	struct call_traits<const T [N]>
	{
	private:
		typedef const T array_type[N];
	public:
	   // degrades array to pointer:
	   typedef const T*				value_type;
	   typedef array_type&			reference;
	   typedef const array_type&	const_reference;
	   typedef const T* const		param_type;
	};
	
	namespace detail
	{
		template<class T> struct make_parameter_impl
		{
			typedef typename detail::ct_impl< T, i3::is_pointer<T>::value, i3::is_arithmetic<T>::value>::param_type	type;
		};

		template<class T> struct make_parameter_impl<T&>							{ typedef T&					type; };
		template <class T, std::size_t N> struct make_parameter_impl<T [N]>			{ typedef const T* const		type; };
		template <class T, std::size_t N> struct make_parameter_impl<const T [N]>	{ typedef const T* const		type; };
		
	}

	template <class T> struct make_parameter : detail::make_parameter_impl<T> {};


	// 아래는 구버전..
	// make_parameter
	/*
	template <class T> struct make_parameter 
	{
	private:
		template <class U> struct AddParameterType {	typedef const U & type;	};
		template <class U> struct AddParameterType<U &>	{	typedef U & type;	};
		template <> struct AddParameterType<void> {	typedef null_type type; };
	public:	
		typedef typename select< is_scalar<T>::value, T, typename AddParameterType<T>::type >::type		type;
	};
	*/

}
