// stdafx.h : 잘 변경되지 않고 자주 사용하는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Windows 헤더에서 거의 사용되지 않는 내용을 제외시킵니다.
#endif



// 아래 지정된 플랫폼보다 우선하는 플랫폼을 대상으로 하는 경우 다음 정의를 수정하십시오.
// 다른 플랫폼에 사용되는 해당 값의 최신 정보는 MSDN을 참조하십시오.
#ifndef WINVER				// Windows 95 및 Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
#define WINVER 0x0600		// Windows 98과 Windows 2000 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
#endif

#ifndef _WIN32_WINNT		// Windows NT 4 이후 버전에서만 기능을 사용할 수 있습니다.
#define _WIN32_WINNT 0x0600		// Windows 98과 Windows 2000 이후 버전에 맞도록 적합한 값으로 변경해 주십시오.
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

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars

#include "i3Base.h"
#include "i3Scene.h"
#include "i3TDK.h"

enum PEF_FILE_TYPE
{
	PFT_SINGLE,
	PFT_MULTI
};

enum SIZE_BAR_TYPE
{
	SBT_NONE,
	SBT_HOR,
	SBT_VER,
};

enum 
{
	ID_MENU_ADDKEY = 100,
	ID_MENU_ADDKEY_MULTINATION,
	ID_MENU_DELETEKEY,
	ID_MENU_COPYKEY,
	ID_MENU_PASTEKEY,
	ID_MENU_RENAMEKEY,
	ID_MENU_FINDKEY,
	ID_MENU_DELETEDATA,
	ID_MENU_EDITDATA,
	ID_MENU_RENAMEDATA,
	ID_MENU_COPYDATA,
	ID_MENU_PASTEDATA,
	ID_MENU_COLLAPSENODE,
	ID_MENU_EXPANDNODE,
	ID_MENU_IMPORTKEY,
	ID_MENU_EXPORTKEY,

	ID_MENU_ADD_SELKEYS,
	ID_MENU_SHOW_SELKEYS,
	ID_MENU_REMOVEALL_SELKEYS,
	ID_MENU_IMPORT_SELKEYS,
	ID_MENU_EXPORT_SELKEYS,

	ID_MENU_DTYPE_INT32,
	ID_MENU_DTYPE_REAL32,
	ID_MENU_DTYPE_STRING,
	ID_MENU_DTYPE_VEC2D,
	ID_MENU_DTYPE_VEC3D,
	ID_MENU_DTYPE_VEC4D,
	ID_MENU_DTYPE_COLOR,
	ID_MENU_DTYPE_MATRIX,
	ID_MENU_DTYPE_WEAPON,

	ID_MENU_EXPORT_SQLCREATE,
	ID_MENU_EXPORT_SQLUPDATE,
	ID_MENU_SORT_KEY,
	ID_MENU_SORT_TIME,

	ID_MENU_DATA_COPY_LEFT,
	ID_MENU_DATA_COPY_RIGHT,
	ID_MENU_DATA_COPY_WHOLE,

	ID_MENU_DATA_COMPARE,
	ID_MENU_DATA_MATCH,
}; 

enum
{
	REG_TREECTRL = 101,
	REG_SUBTREECTRL = 102,

	REG_ITEM_LISTCTRL = 111,
	REG_WEAPON_LISTCTRL = 112,

	REG_IMAGE_VIEW = 121,

	REG_EDITCTRL = 104,
};

enum
{
	I3ICON_ANIM			= 0,
	I3ICON_ATTR			= 2,
	I3ICON_BONE			= 4,
	I3ICON_CAMERA		= 6,
	I3ICON_GEOMETRY		= 8,
	I3ICON_NODE			= 10,
	I3ICON_RESOURCE		= 12,
	I3ICON_SOUND		= 14,
	I3ICON_TEXTURE		= 16,
};

enum AnalysisType
{
	DAT_COMPARE = 0,
	DAT_MATCH,
	DAT_SELECT,
	DAT_MODIFY,
	DAT_PASTE,
	DAT_TYPE_MAX,
};


#include "i3TDKRegistryCtrl_Impl.h"
#include "i3RegistrySet_Impl.h"
#include <afxcview.h>

typedef std::vector< std::string > NationBank;
typedef std::vector< std::string >::iterator NationBankIt;



// ClassType 중에 wooden_arrow 값이 200 이다.
//참조 : Client Source - CommonDef.h 소스에 enum WEAPON_CLASS_TYPE 부분
#define UN_INCLUDE_MAX_INDEX	200

#define DO_NOT_USE_CLASSTYPE	100		//wooden_arrow가 100을 사용한다.


namespace i3
{
	namespace range_detail
	{
		inline
		const char*  range_begin(const CString& t) { return (const char*)t;	}
		inline 
		const char*  range_end(const CString& t ) { return ((const char*)t) + t.GetLength(); }

		inline
		const char*  range_begin(const _bstr_t& t) { return (const char*)t;	}
		inline 
		const char*  range_end(const _bstr_t& t ) { return ((const char*)t) + t.length(); }
		
	}

	template<>
	struct range_const_iterator<CString>		{ typedef const char*	type; };			// 
	
	template<>
	struct range_const_iterator<_bstr_t>		{ typedef const char*	type; };			// 
	
}
