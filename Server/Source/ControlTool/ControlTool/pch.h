
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#pragma warning(disable:4091)

#include <SDKDDKVer.h>

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxcview.h>


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#define ALIGN1
#define ALIGN4

typedef float 							REAL32;
#define I3ASSERT(a)						(void)0
#define I3ASSERT_0						(void)0
#define SAFE_DELETE(p)					if(p){delete p;p=NULL;}
#define SAFE_DELETE_ARRAY(p)			if(p){delete[] p;p=NULL;}
#define I3WARN							(void)0
#define AUTH_TIME						60 * 1000 * 10	// 10분

#define SET_SERVER_FLAG( idx, type )	(( (idx & 0xFF) << 8 ) | type)
#define SET_SERVER_IDX( param )			( ( param >> 8 ) & 0xFF )
#define SET_SERVER_TYPE( param )		( 0xFF & param )

#define		I3_BOUNDCHK( a, b)					I3ASSERT( ((a) >= 0) && ((a) < (b)))

// 임시 TypeDefine 입니다
typedef double	REAL64;
typedef UINT16	REAL16;

#include <math.h>

#include "../../../../Common/Library/include/S2MO.h"
#include "CommonDef.h"
#include "SIA_Define.h"
#include "VersionCreate.h"
#include "Common/CommonDef_QuickJoin.h"
#include "CommonServerDef.h"
#include "CommonControlDef.h"

#include "HackCheckMenu.h"

#include "ErrorDlg.h"
#include <sys/stat.h> // _stati64()

#include <winsvc.h>		//SC_HANDLE을 위한 헤더 서비스컨트롤핸들

#pragma warning(disable:4996)


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


