
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원


#include "i3Base.h"
#include "i3Math.h"
#include "i3Gfx.h"
#include "i3Input.h"
#include "i3SoundH.h"
#include "i3Scene.h"
#include "i3SceneUtil.h"
#include "i3Framework.h"
#include "i3Opt.h"
#include "i3TDK.h"
#include "i3Lua.h"


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

template <typename T> 
class CSingleton
{
private:
	/** \brief Explicit private copy constructor. This is a forbidden operation.*/
	CSingleton(const CSingleton<T> &);
	/** \brief Private operator= . This is a forbidden operation. */
	CSingleton& operator=(const CSingleton<T> &);

protected:
	static T* ms_Singleton;

public:
	CSingleton( void )
	{
		assert( !ms_Singleton );
		ms_Singleton = static_cast< T* >( this );
	}

	~CSingleton( void )
	{  
		assert( ms_Singleton );  
		ms_Singleton = 0;  
	}

	static T& getSingleton( void )
	{	
		assert( ms_Singleton );  
		return ( *ms_Singleton ); 
	}

	static T* getSingletonPtr( void )
	{ 
		return ms_Singleton; 
	}
};


template <typename T> T* CSingleton<T>::ms_Singleton = 0; /// 이 코드는 헤더에 있어야만 됨.

//더큐먼트 팬
#define ID_WEAPON_PANE 5
#define ID_ITEMLIST_PANE 6
#define ID_EQUIP_PANE 7


#define ID_PROPERTY 1
#define ID_PROPERTY_BASEINFO 2
#define ID_PROPERTY_NEWINFO 3
#define ID_PROPERTY_BASECHILDINFO 100


enum TRACER_TYPE
{
	TRACER_TYPE_UNKNOWN		= -1,
	TRACER_TYPE_BULLET = 0,			//	일반 총알 궤적
	TRACER_TYPE_HAND_GRENADE,		//	수류탄 궤적
	TRACER_TYPE_SHELL_GRENADE,		//	유탄 궤적
	TRACER_TYPE_ARROW,				//	화살 궤적
	TRACER_TYPE_SMOKE,				//	연막탄 궤적
	TRACER_TYPE_FLASH,				//	섬관탄 궤적
	TRACER_TYPE_BULLET_SMOKE,		//	일반 총알 연기

	TRACER_TYPE_MAX
};