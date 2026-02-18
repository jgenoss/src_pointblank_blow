#pragma once

#pragma warning(push)
#pragma warning(disable : 4512)

namespace i3
{
	
	template<class T> class reference_wrapper
	{ 
	public:
		typedef			T						type;
		explicit		reference_wrapper(T& t): m_ref(&t) {}
		operator		T& () const { return *m_ref; }
		T&				get() const { return *m_ref; }
		T*				get_pointer() const { return m_ref; }
	private:
		T* m_ref;			// boost버전과 동일...복사생성과 복사대입연산을 위해서는 반드시 포인터를 가져야한다.
	};


	template<class T> inline const	reference_wrapper<T>			ref(T & t) { return reference_wrapper<T>(t); }
	template<class T> inline const	reference_wrapper<const T>		cref(const T& t) { return reference_wrapper<const T>(t); }
	template<class T> inline T*										get_pointer( const reference_wrapper<T>& r ) {   return r.get_pointer();  }

	
	
		
	// 다음은 traits
	template<class T> struct is_reference_wrapper : false_type {};
	template<class T> struct is_reference_wrapper< reference_wrapper<T> > : true_type {};
	template<class T> struct is_reference_wrapper< reference_wrapper<T> const > : true_type {};
	template<class T> struct is_reference_wrapper< reference_wrapper<T> volatile > : true_type {};
	template<class T> struct is_reference_wrapper< reference_wrapper<T> const volatile > : true_type {};
	// 다음은 unwrap 타입
	template<class T> struct unwrap_reference {   typedef T type; };
	template<class T> struct unwrap_reference< reference_wrapper<T> > {   typedef T type; };
	template<class T> struct unwrap_reference< reference_wrapper<T> const > {   typedef T type; };
	template<class T> struct unwrap_reference< reference_wrapper<T> volatile > {   typedef T type; };
	template<class T> struct unwrap_reference< reference_wrapper<T> const volatile > {   typedef T type; };

// 다음은 새로 추가해본 unwrap
	template<class T> __forceinline typename unwrap_reference<T>::type&		unwrap(T& t) { return t; }
	
}

#pragma warning(pop)