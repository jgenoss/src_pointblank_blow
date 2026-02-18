// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#if defined( I3_WINDOWS)

	#ifndef VC_EXTRALEAN
	#define VC_EXTRALEAN		// Windows 헤더에서 거의 사용되지 않는 내용을 제외시킵니다.
	#endif

	// 아래 지정된 플랫폼보다 우선하는 플랫폼을 대상으로 하는 경우 다음 정의를 수정하십시오.
	// 다른 플랫폼에 사용되는 해당 값의 최신 정보는 MSDN을 참조하십시오.
	#ifndef WINVER				// Windows 95 및 Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
	#define WINVER 0x0501		// Windows 98과 Windows 2000 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
	#endif

	#ifndef _WIN32_WINNT		// Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
	#define _WIN32_WINNT 0x0501		// Windows 98과 Windows 2000 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
	#endif						

	#ifndef _WIN32_WINDOWS		// Windows 98 이후 버전에서만 기능을 사용할 수 있습니다.
	#define _WIN32_WINDOWS 0x0410 // Windows Me 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
	#endif

	#ifndef _WIN32_IE			// IE 4.0 이후 버전에서만 기능을 사용할 수 있습니다.
	#define _WIN32_IE 0x0700	// IE 5.0 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
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

	#include <afxcontrolbars.h>

#endif	//#ifdef I3_WINDOWS

#include "i3Base.h"
#include "i3Math.h"
#include "i3Gfx.h"
#include "i3Scene.h"
#include "i3Framework.h"

#include "../../../Common/Library/include/S2MO.h"
#include "../../../Common/CommonSource/CommonDef.h"
#include "../Common/CommonDefForClient.h"

#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

extern void __startlog(LPCTSTR processName);
extern LPCSTR __getLogName();

//Dll Injection 방어 코드
//#define USE_HACK_DETECTED	//Dll Injection Detected (오작동 함)
//#define USE_DLL_DETECTED	//Load 된 Dll을 이용한 Dectected