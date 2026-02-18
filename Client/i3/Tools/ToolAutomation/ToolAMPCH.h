
#ifndef _TOOLAUTOMATIONPCH_H_
#define _TOOLAUTOMATIONPCH_H_

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif						

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0700
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

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars
#include <afxsock.h>		// MFC 소켓 익스텐션

#include <assert.h>

#include "ToolAM.h"





//////////////////////////////////////////////////////////////////////////
// 항상 맨아래에 위치시킬것
//////////////////////////////////////////////////////////////////////////
#if defined(__I3_BASE_H) || defined(I3_DLL)
#error "이프로젝트안에 소스는 i3엔진라이브러리를 사용하지않습니다. 스태틱라이브러리로 제공되기때문입니다."
#error "이프로젝트안의 소스에서 i3엔진과 혼용사용시 i3클래스의 StaticMeta충돌로 정상작동하지않습니다.(DLL과 실행파일양쪽에 i3클래스 메타파일이생겨서)"
#endif


#endif