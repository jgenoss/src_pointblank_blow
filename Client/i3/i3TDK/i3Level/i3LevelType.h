#if !defined( __I3_LEVEL_TYPE_H)
#define __I3_LEVEL_TYPE_H


#pragma once


#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// 아래 지정된 플랫폼보다 우선하는 플랫폼을 대상으로 하는 경우 다음 정의를 수정하십시오.
// 다른 플랫폼에 사용되는 해당 값의 최신 정보는 MSDN을 참조하십시오.
#ifndef WINVER				// Windows 95 및 Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
#define WINVER 0x0501		// Windows 98과 Windows 2000 이후 버전에 맞도록 적합한 값으로 변경해 주십시오. (XP전용변경)2015.08.31.수빈.
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

#include <afx.h>
#include <afxwin.h>         // MFC core and standard components

#include "i3Base.h"
#include "i3Scene.h"
#include "i3Opt.h"
#include "i3Framework.h"

#include "i3LevelDefine.h"

enum I3_LEVEL_COMPARE
{
	I3_LEVEL_COMPARE_EQ	=	0,
	I3_LEVEL_COMPARE_NE,
	I3_LEVEL_COMPARE_GT,
	I3_LEVEL_COMPARE_GE,
	I3_LEVEL_COMPARE_LT,
	I3_LEVEL_COMPARE_LE
};

#define		CHECKIO( Rc)				\
	if( Rc == STREAM_ERR)\
	{\
		i3Log( __FUNCDNAME__, "Line:%d", __LINE__);\
		return STREAM_ERR;\
	}\
	Result += Rc;

#endif