#include "i3CommonType.h"

#if defined( I3_WIPI_GIGA ) && defined( I3_COMPILER_GCC)

// SKT의 GCC가 C++을 지원하지 않는 상황에서, 3.4.3 Version의 libstdc++.a를 추가로
// 사용할 수 있도록 하기 위한 Dummy Exception Handler 함수.
//
// 만약 SKT의 WIPI Library가 3.4.3 Version으로 제공된다면 없어질 수 있는 함수이다.
extern "C"
{
	typedef enum
	{
		aaa,
	} _Unwind_Reason_Code;

	extern _Unwind_Reason_Code _Unwind_SjLj_Resume(struct _Unwind_Exception *exc);

	_Unwind_Reason_Code _Unwind_SjLj_Resume_or_Rethrow (struct _Unwind_Exception *exc)
	{
		return _Unwind_SjLj_Resume(exc);
	}

	void end(void)
	{
		//
	}
}

#endif
