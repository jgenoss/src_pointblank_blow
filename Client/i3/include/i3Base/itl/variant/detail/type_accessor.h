#pragma once


namespace i3
{
	namespace detail
	{

		// 부분 특화로 계속 민다...
#pragma warning(push)
#pragma warning( disable: 4244)

#pragma push_macro("new")
#undef new

		template<class T>
		struct type_accessor
		{
			__forceinline static void construct(void* raw_data) { new (raw_data) T(); }
			__forceinline static void construct( void* raw_data, const T& val) { new (raw_data) T(val); }
			__forceinline static void copy( void* raw_data, const T& val) {  *( static_cast<T*>(raw_data) ) = val;	}

			__forceinline static void destroy( const void* raw_data) {  i3::destroy( static_cast<const T*>(raw_data) ); }
			__forceinline static const T&	get(const void* raw_data) { return *static_cast<const T*>(raw_data); }
			__forceinline static T&			get(void* raw_data) { return *static_cast<T*>(raw_data); }
			

			__forceinline static void swap(void* lhs, void* rhs) {   i3::swap( *static_cast<T*>(lhs), *static_cast<T*>(rhs) );  }
			__forceinline static void reset(void* raw_data) {  *static_cast<T*>(raw_data) = T();  }

			template<class To> __forceinline static 
			typename i3::enable_if<is_convertible<T, To>,
			bool>::type convert(void* raw_data) 
			{ 
				T tmp(*static_cast<T*>(raw_data));			// 임시객체 필요함..
				type_accessor<T>::destroy(raw_data);
				type_accessor<To>::construct( raw_data, tmp); 
				return true;
			}

			template<class To> __forceinline static 
			typename i3::disable_if<is_convertible<T, To>,
			bool>::type convert(void* raw_data) 
			{ 
				return false;
			}

		};

		template<class T>
		struct type_accessor<const T>
		{
			__forceinline static void construct(void* raw_data) { new (raw_data) T(); }
			__forceinline static void construct( void* raw_data, const T& val) { new (raw_data) T(val); }	
			__forceinline static void copy( void* raw_data, const T& val){  i3::destroy( static_cast<const T*>(raw_data) );	new (raw_data) T(val); }

			__forceinline static void destroy( const void* raw_data) {  i3::destroy( static_cast<const T*>(raw_data) ); }
			
			__forceinline static const T&	get(const void* raw_data) { return *static_cast<const T*>(raw_data); }
			__forceinline static const T&	get(void* raw_data) { return *static_cast<const T*>(raw_data); }
			
			__forceinline static void swap(void* lhs, void* rhs) 
			{   
				const T tmp( *static_cast<const T*>(lhs) );
				type_accessor<const T>::copy(lhs, type_accessor<const T>::get(rhs));
				type_accessor<const T>::copy(rhs, tmp);
			}	

			__forceinline static void reset(void* raw_data) {  i3::destroy( static_cast<const T*>(raw_data) ); new (raw_data) T;  }

			template<class To> __forceinline static 
			typename i3::enable_if<is_convertible<const T, To>,
			bool>::type convert(void* raw_data) 
			{ 
				T tmp(*static_cast<const T*>(raw_data));			// 임시객체 필요함..
				type_accessor<const T>::destroy(raw_data);
				type_accessor<To>::construct( raw_data, tmp ); 
				return true;
			}

			template<class To> __forceinline static 
			typename i3::disable_if<is_convertible<const T, To>,
			bool>::type convert(void* raw_data) 
			{ 
				return false;
			}

		};

		template<class T>
		struct type_accessor<T&>		
		{
			__forceinline static void construct(void*)  {}			// 참조인경우 생성이 없다..
			__forceinline static void construct( void* raw_data, T& val) { new (raw_data) reference_wrapper<T>(val); }
			__forceinline static void copy( void* raw_data, T& val) { new (raw_data) reference_wrapper<T>(val);  }

			__forceinline static void destroy( const void* ) {}	// POD로 소멸자를 호출하지 말고 그냥 넘어감..


			__forceinline static T&	get(const void* raw_data) {  return  static_cast<const reference_wrapper<T>*>(raw_data)->get(); }
			__forceinline static T&	get(void* raw_data) { return  static_cast<reference_wrapper<T>*>(raw_data)->get(); }

			__forceinline static void swap(void* lhs, void* rhs) 
			{   
				i3::swap( *static_cast<reference_wrapper<T>*>(lhs), *static_cast<reference_wrapper<T>*>(rhs) );  
			}
			__forceinline static void reset(void* ) { }	// do nothing // 리셋이 되지 않음..

			template<class To> __forceinline static 
			typename i3::enable_if<is_convertible<T&, To>,
			bool>::type convert(void* raw_data) 
			{ 
				reference_wrapper<T> tmp(*static_cast<reference_wrapper<T>*>(raw_data)); 
				type_accessor<To>::construct(raw_data, tmp.get());		
				return true;
			}

			template<class To> __forceinline static 
			typename i3::disable_if<is_convertible<T&, To>,
			bool>::type convert(void* raw_data) 
			{ 
				return false;
			}

		};

		template<class T>
		struct type_accessor<const T&>
		{
			__forceinline static void construct(void*)  {}			// 참조인경우 생성이 없다..
			__forceinline static void construct( void* raw_data, const T& val) { new (raw_data) reference_wrapper<const T>(val); }	
			__forceinline static void copy( void* raw_data, const T& val) { new (raw_data) reference_wrapper<const T>(val);  }

			__forceinline static void destroy( const void* ) {}	// POD로 소멸자를 호출하지 말고 그냥 넘어감..

			__forceinline static const T&	get(const void* raw_data) {  return  static_cast<const reference_wrapper<const T>*>(raw_data)->get(); }
			__forceinline static const T&	get(void* raw_data) { return  static_cast<reference_wrapper<const T>*>(raw_data)->get(); }
	
			__forceinline static void swap(void* lhs, void* rhs) 
			{   
				i3::swap( *static_cast<reference_wrapper<const T>*>(lhs), *static_cast<reference_wrapper<const T>*>(rhs) );  
			}
			__forceinline static void reset(void* ) { }	// do nothing // 리셋이 되지 않음..

			template<class To> __forceinline static 
			typename i3::enable_if<is_convertible<const T&, To>,
			bool>::type convert(void* raw_data) 
			{
				reference_wrapper<const T> tmp(*static_cast<reference_wrapper<const T>*>(raw_data)); 
				type_accessor<To>::construct(raw_data, tmp.get());
				return true;
			}
			
			template<class To> __forceinline static 
			typename i3::disable_if<is_convertible<const T&, To>,
			bool>::type convert(void* raw_data) 
			{
				return false;
			}
			

		};
/////////////////////////////





#pragma pop_macro("new")
#pragma warning(pop)
	}
	
	
	
}