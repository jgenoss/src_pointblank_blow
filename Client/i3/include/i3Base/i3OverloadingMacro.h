#pragma once

// 하나 만들긴 만들어야 해서 넣음...
// 출처는 스택오버플로우 
// http://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
// 첫 발견한 위의 링크는 MSVC 환경에서 작동하지 않음..

// http://stackoverflow.com/questions/26682812/argument-counting-macro-with-zero-arguments-for-visualstudio-2010/26685339#26685339
// 이 링크 버전은 잘 작동하기 때문에 이것을 수정해서 적용한다..

//
// 매크로의 이름은 통일하고 인수갯수 별로 구현에는 차이를 둘때 쓴다..
// preprocessor에서는 기능 제공이 없고, 다소 이질적이기 때문에 i3Base 상단에 구현해놓도록 한다.
//

// 사용법 :  I3ASSERT(...)를 사용하려고 할때 인수0개 ,1개, 2개짜리가 구현이 아예 다른 경우
// #define I3ASSERT(...) I3_OVERLOADING_MACRO(I3ASSERT, __VA_ARGS__) 로 정의를 하고
// #define I3ASSERT0()
// #define I3ASSERT1(a)   someMacro1(a)
// #define I3ASSERT2(a, b) someMacro2(a, b)  와 같이 구현하면 된다..
//
// 이 구현은 C++이 전혀 아니며, 컴파일러 제작사 구현 종속인 상태이다...(최소한 MS의 경우, 가변매크로 가능한 경우 전체 호환된다고 여겨짐)
//

#define __I3_MACRO_FIND_NARG_IMPL__(\
      _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
     _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
     _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
     _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
     _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
     _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
     _61,_62,_63,N,...) N

#define __I3_MACRO_EXPAND_IMPL__(x) x

#define __I3_MACRO_NARG_IMPL__(...) __I3_MACRO_EXPAND_IMPL__(__I3_MACRO_FIND_NARG_IMPL__(__VA_ARGS__, \
	 62,61,60,\
     59,58,57,56,55,54,53,52,51,50, \
     49,48,47,46,45,44,43,42,41,40, \
     39,38,37,36,35,34,33,32,31,30, \
     29,28,27,26,25,24,23,22,21,20, \
     19,18,17,16,15,14,13,12,11,10, \
     9,8,7,6,5,4,3,2,1,0 ) )

#define __I3_MACRO_CONSIDER_ZERO_ARG__(...) unused, __VA_ARGS__


#define	I3_MACRO_NARGS(...) __I3_MACRO_NARG_IMPL__(__I3_MACRO_CONSIDER_ZERO_ARG__(__VA_ARGS__))


#define __I3_MACRO_VFUNC__(name , n) name##n
#define __I3_MACRO_VFUNC2__(name, n) __I3_MACRO_VFUNC__(name, n)

#define I3_OVERLOADING_MACRO(func, ...) __I3_MACRO_VFUNC2__(func, I3_MACRO_NARGS(__VA_ARGS__)) (__VA_ARGS__)
