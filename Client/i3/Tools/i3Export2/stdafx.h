// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Windows 헤더에서 거의 사용되지 않는 내용을 제외시킵니다.
#endif

// 아래 지정된 플랫폼보다 우선하는 플랫폼을 대상으로 하는 경우 다음 정의를 수정하십시오.
// 다른 플랫폼에 사용되는 해당 값의 최신 정보는 MSDN을 참조하십시오.
#ifndef WINVER				// Windows 95 및 Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
#define WINVER 0x0502		// Windows 98과 Windows 2000 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#ifndef _WIN32_WINNT		// Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_WINNT 0x0502		// Windows 98과 Windows 2000 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_WINDOWS 0x0502 // Windows Me 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#ifndef _WIN32_IE			// IE 4.0 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_IE 0x0800	// IE 5.0 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS



//#include <afx.h>

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소
#include <afxext.h>         // MFC 익스텐션
#include <afxdisp.h>        // MFC 자동화 클래스

#include <afxdtctl.h>		// Internet Explorer 4 공용 컨트롤에 대한 MFC 지원

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows 공용 컨트롤에 대한 MFC 지원
#endif

//#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#include "i3Base.h"

#include "i3Math.h"
#include "i3Scene.h"
#include "i3SceneUtil.h"
#include "i3Opt.h"

#include <dshow.h>

#include "i3Gfx.h"

#if !defined( min)
#define	min( a, b)			( a < b) ? a : b
#endif

#if !defined( max)
#define	max( a, b)			( a > b) ? a : b
#endif

// MAX SDK Headers

#include "max.h"
#include "bmmlib.h"
#include "iparamb2.h"
#include "iparamm2.h"
#include "notify.h"
#include "stdmat.h"
#include "istdplug.h"
#include "modstack.h"
#include "CS/Phyexp.h"
#include "CS/Bipexp.h"
#include "iskin.h"
#include "hsv.h"

#include <shellapi.h>

#if defined( I3_WINDOWS) && defined( I3_DLL)
	#if defined( I3EXPORT2_EXPORTS)
		#define I3_EXPORT_EXP		__declspec( dllexport)
		#define I3_EXTERN_EXP		extern
	#else
		#define I3_EXPORT_EXP		__declspec( dllimport)
		#define I3_EXTERN_EXP		__declspec( dllimport)
	#endif
#else
	#if !defined( I3EXPORT2_EXPORTS)
		#define	I3_EXPORT_EXP
		#define I3_EXTERN_EXP		extern
	#endif
#endif

#define		CHECKIO( Rc)				\
	if( Rc == STREAM_ERR)\
	{\
		i3Log( __FUNCDNAME__, "Line:%d", __LINE__);\
		return STREAM_ERR;\
	}\
	Result += Rc;

// class id for the physique modifier
#define PHYSIQUE_CLASSID Class_ID(PHYSIQUE_CLASS_ID_A,PHYSIQUE_CLASS_ID_B)

#define		CHKP(str)			MessageBox( NULL, "Check", str, MB_OK)

extern HINSTANCE hInstance;

#include "i3OptiExpPlug.h"
