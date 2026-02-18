#if !defined( __I3_FRAMEWORK_PCH_H)
#define __I3_FRAMEWORK_PCH_H
#if defined( I3_WINDOWS) && defined( I3_DEBUG)
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
	#define _WIN32_WINDOWS 0x0510 // Windows Me 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
	#endif

	#ifndef _WIN32_IE			// IE 4.0 이후 버전에서만 기능을 사용할 수 있습니다.
	#define _WIN32_IE 0x0600	// IE 5.0 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
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
#endif

#include "i3Base.h"
#include "i3Math.h"
#include "i3Gfx.h"
#include "i3Scene.h"
#include "i3SceneUtil.h"
#include "i3Lua.h"

#if (defined( I3_WINDOWS) || defined( I3_WINCE)) && defined( I3_DLL)
	#if defined( I3_FRAMEWORK_BUILD)
		#define I3_EXPORT_FRAMEWORK		__declspec( dllexport)
		#define I3_EXTERN_FRAMEWORK		extern
	#else
		#define I3_EXPORT_FRAMEWORK		__declspec( dllimport)
		#define I3_EXTERN_FRAMEWORK		__declspec( dllimport)
	#endif
#else
	#if !defined( I3_EXPORT_FRAMEWORK)
		#define	I3_EXPORT_FRAMEWORK
		#define I3_EXTERN_FRAMEWORK		extern
	#endif
#endif

typedef		wchar_t		CHAR16, *STR16;

#endif
