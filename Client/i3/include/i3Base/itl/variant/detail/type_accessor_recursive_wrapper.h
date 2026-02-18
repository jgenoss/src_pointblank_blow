#pragma once


namespace i3
{
	namespace detail
	{
		// recursive_wrapper
		template<class T>
		struct type_accessor< recursive_wrapper<T> >
		{
			__forceinline static void construct(void* raw_data) { new (raw_data) recursive_wrapper<T>(); }
			__forceinline static void construct( void* raw_data, const T& val) { new (raw_data) recursive_wrapper<T>(val); }
			__forceinline static void copy( void* raw_data, const T& val) {  *( static_cast<recursive_wrapper<T>*>(raw_data) ) = val;	}

			__forceinline static void destroy( const void* raw_data) {  i3::destroy( static_cast<const recursive_wrapper<T>*>(raw_data) ); }
			__forceinline static const T&	get(const void* raw_data) { return static_cast<const recursive_wrapper<T>*>(raw_data)->get(); }
			__forceinline static T&			get(void* raw_data) { return static_cast<recursive_wrapper<T>*>(raw_data)->get(); }


			__forceinline static void swap(void* lhs, void* rhs) {   i3::swap( *static_cast<recursive_wrapper<T>*>(lhs), *static_cast<recursive_wrapper<T>*>(rhs) );  }
			__forceinline static void reset(void* raw_data) {  *static_cast<recursive_wrapper<T>*>(raw_data) = T();  }

			template<class To> __forceinline static 
			typename i3::enable_if<is_convertible<T, To>,
			bool>::type convert(void* raw_data) 
			{ 
				T tmp( static_cast<recursive_wrapper<T>*>(raw_data)->get());	// 임시객체 필요함..
				type_accessor< recursive_wrapper<T> >::destroy(raw_data);
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
		struct type_accessor< recursive_wrapper<const T> >
		{
			__forceinline static void construct(void* raw_data) { new (raw_data) recursive_wrapper<const T>(); }
			__forceinline static void construct( void* raw_data, const T& val) { new (raw_data) recursive_wrapper<const T>(val); }
			__forceinline static void copy( void* raw_data, const T& val)
			{  
				i3::destroy( static_cast< recursive_wrapper<const T>*>(raw_data) );	new (raw_data) recursive_wrapper<const T>(val); 
			}

			__forceinline static void destroy( const void* raw_data) {  i3::destroy( static_cast<const recursive_wrapper<const T>*>(raw_data) ); }

			__forceinline static const T&	get(const void* raw_data) { return static_cast<const recursive_wrapper<const T>*>(raw_data)->get(); }
			__forceinline static const T&	get(void* raw_data) { return static_cast<recursive_wrapper<const T>*>(raw_data)->get(); }

			__forceinline static void swap(void* lhs, void* rhs) {   i3::swap( *static_cast<recursive_wrapper<const T>*>(lhs), *static_cast<recursive_wrapper<const T>*>(rhs) );  }

			__forceinline static void reset(void* raw_data) 
			{  
				i3::destroy( static_cast< recursive_wrapper<const T>*>(raw_data) );	new (raw_data) recursive_wrapper<const T>(); 
			}

			template<class To> __forceinline static 
			typename i3::enable_if<is_convertible<const T, To>,
			bool>::type convert(void* raw_data) 
			{ 
				const T tmp( static_cast<recursive_wrapper<const T>*>(raw_data)->get());	// 임시객체 필요함..
				type_accessor< recursive_wrapper<const T> >::destroy(raw_data);
				type_accessor<To>::construct( raw_data, tmp);
				return true;
			}

			template<class To> __forceinline static 
			typename i3::disable_if<is_convertible<const T, To>,
			bool>::type convert(void* raw_data) 
			{
				return false;
			}

		};
				
	}
}
