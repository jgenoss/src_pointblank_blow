#ifndef __PRECOMPILED_HEADER_H__
#define	__PRECOMPILED_HEADER_H__

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

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

#include <afxcontrolbars.h>

#if defined( I3_COMPILER_VC)
#pragma warning( disable : 4995 )
#pragma warning (disable : 4127)
#endif

// =================================================================================
// COMMON
// =================================================================================
//#define		USE_LUA_UI

// C Runtime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

// I3 Header Files
#include "i3Base.h"
#include "i3Base_UsefulStringFunctions.h"
#include "i3Math.h"

#if defined (USE_LUA_UI)
#include "i3Lua.h"
#endif

#include "i3Scene.h"
#include "i3SceneUtil.h"
#include "i3SoundH.h"
#include "i3Network.h"

#include "i3Framework.h"
#include "i3Gui.h"

#if !defined(I3_DEBUG) && defined(USE_EDITDLG)
#error "If you want edit dialog, you must be DEBUG build"
#endif

#if defined(USE_EDITDLG)
#include "../i3TDK/i3TDK.h"
#endif

#include <memory>

#if _UNICODE
typedef std::wstring	tstring;
#else
typedef std::string		tstring;
#endif

//Project Header Files
#include "../../../Common/Library/include/S2MO.h"

#include "../../../Common/CommonSource/QuestDef.h"
#include "../../../Common/CommonSource/CommonDef.h"
#include "../../../Common/CommonSource/Contents/Contents.h"
#include "ItemID/ItemMecro.h"
#include "CommonDef_MultiSlot.h"
#include "../Common/CommonDefForClient.h"
#include "../../../Common/CommonSource/SIA_Define.h"
#include "../../../Common/CommonSource/Medal/MedalDef.h"
#include "../../../Common/CommonStructure/ShareClient/ProtocolStruct.h"
#include "../../../Common/CommonStructure/ShareClient/ProtocolNewStruct.h"
#include "../../../Common/CommonStructure/ShareClient/ProtocolServerMsgStruct.h"
#include "../../../Common/CommonStructure/ShareClient/ProtocolBaseStruct.h"
#include "../../../Common/CommonStructure/ShareClient/ProtocolAuthStruct.h"
#include "../../../Common/CommonStructure/ShareClient/ProtocolLobbyStruct.h"
#include "../../../Common/CommonStructure/ShareClient/ProtocolRoomStruct.h"
#include "../../../Common/CommonStructure/ShareClient/ProtocolQuickJoinStruct.h"
#include "../../../Common/CommonStructure/ShareClient/ProtocolBattleStruct.h"
#include "../../../Common/CommonSource/NetworkP2PDef.h"
#include "../../../Common/CommonSource/ClanMatch/ClanMatchDefine.h"
#include "../../../Common/CommonSource/S2MO/S2MO.h"
#include "../../Themida/ThemidaSDK/Include/C/ThemidaSDK.h"

#include "GameDefine.h"
#include "ClientDef.h"
#include "NetCharaInfo.h"
#include "UICharaInfo.h"
#include "GlobalFunc.h"
#include "strsafe.h"

#include "StageDef.h"
#include "ShopDef.h"
#include "Shop.h"

#include "WeaponGlobal.h"


#if defined( I3_WINDOWS) && defined( I3_DLL)
#if defined( I3_TDK_BUILD)
#define I3_EXPORT_TDK		__declspec( dllexport)
#define I3_EXTERN_TDK		extern
#else
#define I3_EXPORT_TDK		__declspec( dllimport)
#define I3_EXTERN_TDK		__declspec( dllimport)
#endif
#else
#if !defined( I3_EXPORT_TDK)
#define	I3_EXPORT_TDK
#define I3_EXTERN_TDK		extern
#endif
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define	BUILD_RELEASE_QA_VERSION

#if defined(_DEBUG) || defined(BUILD_RELEASE_QA_VERSION)
#define PBCONSOLE_CREATE_FOR_DEBUG_RELEASEQA  extern i3ClassMeta* GetPBConsoleClientCtxMeta(); \
	i3UIManager::InitConsole( true, GetPBConsoleClientCtxMeta()); \
	i3UIManager::ShowConsole(false); \
	PBConsoleManager::CreateInstance();

#else
#define PBCONSOLE_CREATE_FOR_DEBUG_RELEASEQA
#endif

#endif // _PRECOMPILED_HEADER_H__