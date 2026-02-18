#pragma once


//  detail¿ª ∂¿..

namespace i3
{
	namespace ft
	{
		
		
		template<class T> struct to_class_cv_tag 										{ typedef non_cv type; };
		template<class T> struct to_class_cv_tag<T &>									{ typedef non_cv type; };
		template<class T> struct to_class_cv_tag<T *>									{ typedef non_cv type;  };
		template<class T> struct to_class_cv_tag<T * const>								{ typedef non_cv type;  };
		template<class T> struct to_class_cv_tag<T * volatile>							{ typedef non_cv type;  };
		template<class T> struct to_class_cv_tag<T * const volatile>					{ typedef non_cv type;  };

		template<class T> struct to_class_cv_tag<T const>								{ typedef const_non_volatile type;  };
		template<class T> struct to_class_cv_tag<T const &>								{ typedef const_non_volatile type;  };
		template<class T> struct to_class_cv_tag<T const *>								{ typedef const_non_volatile type;  };
		template<class T> struct to_class_cv_tag<T const * const>						{ typedef const_non_volatile type;  };
		template<class T> struct to_class_cv_tag<T const * volatile>					{ typedef const_non_volatile type;  };
		template<typename T> struct to_class_cv_tag<T const * const volatile>			{ typedef const_non_volatile type;  };

		template<class T> struct to_class_cv_tag<T volatile>							{ typedef volatile_non_const type;  };
		template<class T> struct to_class_cv_tag<T volatile &>							{ typedef volatile_non_const type;  };
		template<class T> struct to_class_cv_tag<T volatile *>							{ typedef volatile_non_const type;  };
		template<class T> struct to_class_cv_tag<T volatile * const>					{ typedef volatile_non_const type;  };
		template<class T> struct to_class_cv_tag<T volatile * volatile>					{ typedef volatile_non_const type;  };
		template<class T> struct to_class_cv_tag<T volatile * const volatile>			{ typedef volatile_non_const type;  };

		template<class T> struct to_class_cv_tag<T const volatile>						{ typedef cv_qualified type;  };
		template<class T> struct to_class_cv_tag<T const volatile &>					{ typedef cv_qualified type;  };
		template<class T> struct to_class_cv_tag<T const volatile *>					{ typedef cv_qualified type;  };
		template<class T> struct to_class_cv_tag<T const volatile * const>				{ typedef cv_qualified type;  };
		template<class T> struct to_class_cv_tag<T const volatile * volatile>			{ typedef cv_qualified type;  };
		template<class T> struct to_class_cv_tag<T const volatile * const volatile>		{ typedef cv_qualified type;  };

//////////////////


		template<class T> struct to_class_type											{  typedef T type; };
		template<class T> struct to_class_type<T &>										{  typedef T type; };
		template<class T> struct to_class_type<T *>										{  typedef T type; };
		template<class T> struct to_class_type<T * const>								{  typedef T type; };
		template<class T> struct to_class_type<T * volatile>							{  typedef T type; };
		template<class T> struct to_class_type<T * const volatile>						{  typedef T type; };

		template<class T> struct to_class_type<T const>									{  typedef T type; };
		template<class T> struct to_class_type<T const &>								{  typedef T type; };
		template<class T> struct to_class_type<T const *>								{  typedef T type; };
		template<class T> struct to_class_type<T const * const>							{  typedef T type; };
		template<class T> struct to_class_type<T const * volatile>						{  typedef T type; };
		template<class T> struct to_class_type<T const * const volatile>				{  typedef T type; };

		template<class T> struct to_class_type<T volatile>								{  typedef T type; };
		template<class T> struct to_class_type<T volatile &>							{  typedef T type; };
		template<class T> struct to_class_type<T volatile *>							{  typedef T type; };
		template<class T> struct to_class_type<T volatile * const>						{  typedef T type; };
		template<class T> struct to_class_type<T volatile * volatile>					{  typedef T type; };
		template<class T> struct to_class_type<T volatile * const volatile>				{  typedef T type; };

		template<class T> struct to_class_type<T const volatile>						{  typedef T type; };
		template<class T> struct to_class_type<T const volatile &>						{  typedef T type; };
		template<class T> struct to_class_type<T const volatile *>						{  typedef T type; };
		template<class T> struct to_class_type<T const volatile * const>				{  typedef T type; };
		template<class T> struct to_class_type<T const volatile * volatile>				{  typedef T type; };
		template<class T> struct to_class_type<T const volatile * const volatile>		{  typedef T type; };
	
		
		
	}
}