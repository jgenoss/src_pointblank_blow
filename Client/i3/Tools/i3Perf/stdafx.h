// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

	#ifndef VC_EXTRALEAN
	#define VC_EXTRALEAN		// Windows 헤더에서 거의 사용되지 않는 내용을 제외시킵니다.
	#endif

	#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.

	// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
	#define _AFX_ALL_WARNINGS

	#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소
	#include <afxext.h>         // MFC 익스텐션
	#include <afxdisp.h>        // MFC 자동화 클래스

	#include <afxdtctl.h>		// Internet Explorer 4 공용 컨트롤에 대한 MFC 지원
	#ifndef _AFX_NO_AFXCMN_SUPPORT
	#include <afxcmn.h>			// Windows 공용 컨트롤에 대한 MFC 지원
	#endif // _AFX_NO_AFXCMN_SUPPORT

#include "i3Base.h"
#include "i3Math.h"
#include "i3Gfx.h"
#include "i3Scene.h"
#include "i3Framework.h"