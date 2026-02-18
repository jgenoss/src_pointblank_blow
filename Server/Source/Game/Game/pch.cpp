// stdafx.cpp : 표준 포함 파일을 포함하는 소스 파일입니다.
// BAServer.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj는 미리 컴파일된 형식 정보를 포함합니다.

#include "pch.h"

#ifdef YJ_DEBUG_PROTOCOL	// serverDef.h - 조건부 컴파일 스위치 포함
#include "../../CommonServerSource/Debug_ProtocolUtil.cpp" // cpp 파일은 pch.cpp에 포함
#endif