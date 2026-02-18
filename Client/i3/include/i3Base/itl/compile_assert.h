#pragma once

// static_assert : 2010의 경우 키워드로 예약되어 사용하기 힘들것임..
// STATIC_ASSERT : boost쪽에서 채택된 표기...대문자표기를 되도록 피하기로 함..

//
// compile_assert / compile_assert_msg 
// 메세지표기의 경우 빈칸없이변수 혹은 클래스 형식으로 적어야한다..
//

namespace i3
{

	template<bool test> struct compile_assert_failure {};
	template<> struct compile_assert_failure<true>{};
	template<> struct compile_assert_failure<false>; 

//	template<bool test>
//	struct compile_assert_failure {	typedef char type_must_be_complete[ (test) ? 1 : -1]; };

		
	template<class Pred>
	compile_assert_failure<bool(Pred::value)> compile_assert_pred_failure_fun(void (*)(Pred) );

	template<class Pred>
	compile_assert_failure<!bool(Pred::value)> compile_assert_pred_not_failure_fun(void (*)(Pred) );


	template<bool test, class msg> struct compile_assert_msg_failure {};
	template<class msg> struct compile_assert_msg_failure<true, msg> {};
	template<class msg> struct compile_assert_msg_failure<false, msg>;

//	template<bool test, class msg> 
//	struct compile_assert_msg_failure { typedef char type_must_be_complete[ (test) ? 1 : -1]; };
	

	template<bool test, class msg, class T> struct compile_assert_msg_type_failure {};
	template<class msg, class T> struct compile_assert_msg_type_failure<true, msg, T> {};
	template<class msg, class T> struct compile_assert_msg_type_failure<false, msg, T>;

//	template<bool test, class msg, class T> 
//	struct compile_assert_msg_type_failure { typedef char type_must_be_complete[ (test) ? 1 : -1]; }; 

	template<class msg, class test, class Type >
	compile_assert_msg_type_failure<test::value, msg,Type> compile_assert_msg_type_failure_fun( void(*)(test), void (*)(Type) );
	

	template<size_t test>	struct compile_assert_test {};

#define compile_assert(expr)		typedef  ::i3::compile_assert_test<sizeof(::i3::compile_assert_failure<bool(expr)>) >				compile_assert_type
#define compile_assert_rel(x, rel, y) compile_assert( x rel y )

#define compile_assert_msg(expr, x) typedef  ::i3::compile_assert_test<sizeof(::i3::compile_assert_msg_failure<bool(expr), class x>) >	compile_assert_type

#define compile_assert_msg_type(expr, x, y) typedef ::i3::compile_assert_test<sizeof(::i3::compile_assert_msg_type_failure_fun<class x>((void (*)(mpl::bool_<bool(expr)>) )0, (void (*) y)0)) > compile_assert_type

#define compile_assert_pred(pred)	typedef  ::i3::compile_assert_test<sizeof(::i3::compile_assert_pred_failure_fun( (void (*) pred)0)) > compile_assert_type
#define compile_assert_pred_not(pred)	typedef  ::i3::compile_assert_test<sizeof(::i3::compile_assert_pred_not_failure_fun( (void (*) pred)0)) > compile_assert_type	


}
