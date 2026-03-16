#pragma once

#include "is_class.h"
#include "remove_cv.h"

namespace i3
{
	namespace detail
	{
		
		template <class T>
		struct is_polymorphic_imp1
		{	
			typedef typename remove_cv<T>::type ncvT;
			struct d1 : public ncvT
			{
				d1();		~d1()throw();
				char padding[256];
			private:
				d1(const d1&);
				d1& operator=(const d1&);
			};

			struct d2 : public ncvT
			{
				d2();		virtual ~d2()throw();
				char padding[256];
			private:
				d2(const d2&);
				d2& operator=(const d2&);
			};
			static const bool value = (sizeof(d2) == sizeof(d1));
		};

		template <class T>
		struct is_polymorphic_imp2
		{
			static const bool value = false;
		};

		template <bool IsClass>
		struct is_polymorphic_selector {   template <class T> struct rebind {	  typedef is_polymorphic_imp1<T> type;   }; };

		template <> struct is_polymorphic_selector<false> {   template <class T> struct rebind {  typedef is_polymorphic_imp2<T> type;  }; };
		
		template<class T>
		struct is_polymorphic_imp 
		{
			typedef is_polymorphic_selector<is_class<T>::value> selector;
			typedef typename selector::template rebind<T>		binder;
			typedef typename binder::type						imp_type;
			static const bool value = imp_type::value;
		};
	}
	
	template<class T> struct is_polymorphic : integral_constant<bool, detail::is_polymorphic_imp<T>::value> {};

}
