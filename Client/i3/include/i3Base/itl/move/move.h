#pragma once
//
#include "enable_if.h"
#include "utility/addressof.h"
#include "mpl/mpl_if.h"
#include "mpl/mpl_bool.h"
#include "mpl/mpl_and.h"
#include "mpl/mpl_not.h"
#include "mpl/mpl_identity.h"
#include "type_traits/is_class.h"
#include "type_traits/is_convertible.h"
#include "type_traits/has_trivial_destructor.h"
#include "type_traits/integral_constant.h"

//
#define I3_COPYABLE_MOVABLE(T) public: \
	T& operator=( T& o)		{ return operator=( static_cast<const i3::rv<T>&>(o) ); } \
	operator i3::rv<T>& ()	{ return static_cast<i3::rv<T>&>(*this);  }	\
	operator const i3::rv<T>& () const { return static_cast<const i3::rv<T>&>(*this);  }
//
//
#define I3_MOVABLE(T) private: \
	T(T&); T& operator=(T&); public: \
	operator i3::rv<T>& ()	{ return static_cast<i3::rv<T>&>(*this);  }	\
	operator const i3::rv<T>& () const { return static_cast<const i3::rv<T>&>(*this);  }
//
//
#define rv_ref(T)				i3::rv<T>&
#define const_assign_ref(T)		const i3::rv<T>&
#define fwd_ref(T)				const T&
//

namespace i3
{
	template<class T>
	class rv : public mpl::if_< is_class_or_union<T>, T, null_type>::type	// 클래스급은 상속처리됨..
	{																		// 클래스급은 rv& -> v&로 형변환이 여기서 처리됨..
	private:																// v& -> rv<v>&는 형변환 연산자가 v쪽에서 필요하다..
		rv(); ~rv(); rv( const rv& ); void operator=(const rv&);			// rv객체는 생성용으로 쓰이지 않고 존재하는 T객체를 캐스팅으로만 쓴다..
	};
	
	// 주의점 :
	// rv<T>  : rvalue -->  rvalue생성자/할당자로 함수처리됨..
	// const rv<T> : const lvalue / const rvalue --> 일반적인 복사생성/할당자로 함수처리됨..
	
	namespace detail
	{
		template<class T> struct is_rv : integral_constant<bool, false> {};
		template<class T> struct is_rv< rv<T> > : integral_constant<bool, true> {};
		template<class T> struct is_rv<const rv<T> > : integral_constant<bool, true> {};	//문법상, const value형은 non-const value형으로 특화되지 않음.
	}

	template<class T> struct has_move_emulation_enabled : is_convertible<T, rv<T>& > {};
	template<class T> struct has_move_emulation_enabled<T&> : integral_constant<bool, false> {};
	template<class T> struct has_move_emulation_enabled< rv<T> > : integral_constant<bool, false> {};

	template<class T> inline typename disable_if< i3::has_move_emulation_enabled<T>,
	T&>::type			move(T& x) { return x; }
	template<class T> inline typename enable_if< i3::has_move_emulation_enabled<T>,
	rv<T>&>::type		move(T& x) { return *static_cast<rv<T>*>( addressof(x) );  }		// reinterpet_cast처리와 유사한데, 좀더 적법...
	template<class T> inline typename enable_if< i3::has_move_emulation_enabled<T>, 
	rv<T>&>::type		move(rv<T>& x) { return x; }										// 이미 rv래핑되면 건드리지 않음..
	
	//
	//  move emulation함수의 역할 : rv<T>로 형변환 가능한 값객체들은 rv<T>로 강제 참조캐스팅되어 리턴된다...
	//  C++12에서의 본래의 역할 :  lvalue -> rvalue로 변경해주며, rvalue reference type으로 함수오버로딩이 가능해진다..

	template<class T> inline typename enable_if<detail::is_rv<T>,
	T&>::type			forward(const typename mpl::identity<T>::type& x) { return const_cast<T&>(x); }		// rv인 경우 상수성을 제거한다.
	template<class T> inline typename disable_if<detail::is_rv<T>, 
	const T&>::type		forward(const typename mpl::identity<T>::type& x) { return x; }		//rv가 아니면 변화가 없음..
	

	// mpl::identity<T>::type 가 쓰인 이유는 인수추론을 쓰지 않고 명시적으로 타입을 명기하도록 문법을 강제하기 때문임...(좋은 기법)

	//  foward emulation의 역할 : 2008VC는 rvalue 참조인수에 lvalue를 넣어주면 rvalue로 바뀐다.  
	//                            이부분을 해소하기 위해 rv_ref매크로타입 대신, foward_ref매크로 타입을 쓰고 난뒤에 forward함수를 호출한다...
	//  C++12의 본래 역할 : C++12는 lvalue를 rvalue 참조로 넘겨도 lvalue가 보존되며, forward를 통해 분기한다.  
	            
	// fwd_ref의 T는 모듈에서 쓰이는 대표 타입이 아닌, 래퍼 함수안에 들어온 인수의 타입으로 적용되어야한다..

	// forward함수가 쓰이는 이유 : 
	// 1) 참조매개변수 함수는 상수참조를 받지 못한다.  이때문에, 상수참조매개변수함수로 변경하게 되는데..
	//    만약, 참조매개변수와 상수참조매개변수의 오버로딩 함수가 각각 필요하다면..해당함수를 접근할 래퍼함수들도 모두 2개이상씩 필요하다.
	// 2) 이는 타이핑의 낭비이므로, C++12는 rvalue참조매개변수로 모두 통일해서 일원화시킨다... 
	// 3) 어쨋든, rvalue참조된 매개변수가 원래의 l or r오버로딩 함수를 잘 구별해서 찾아가야한다면, 이때 forward함수를 써서 오버로딩분기를 처리한다.

}