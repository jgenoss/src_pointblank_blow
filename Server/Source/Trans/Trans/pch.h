// pch.h : 잘 변경되지 않고 자주 사용하는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#pragma warning(disable:4091)

#include <SDKDDKVer.h>

#ifdef BUILDMODE_DLG

#define NO_WARN_MBCS_MFC_DEPRECATION			// 멀티바이트 관련 오류를 표시 하지 않기 위해 작성 합니다.

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

#elif BUILDMODE_SEV

#include <process.h>
#include <iostream>
#include <tchar.h>

#endif

#include <math.h>
#include <winsock2.h>
#include <mmsystem.h>

#include "i3Base.h"
#include "../../../../Common/Library/include/S2MO.h"
#include "../../../../Common/CommonSource/CommonDef.h"
#include "../../../../Common/CommonSource/SIA_Define.h"
#include "../../CommonServerSource/CommonServerDef.h"
#include "../../../../Common/CommonSource/QuestDef.h"
#include "../../../../Common/CommonSource/Medal/MedalDef.h"
#include "../../../../Common/CommonStructure/ShareClient/ProtocolStruct.h"
#include "../../CommonServerSource/Ado.h"
#include "../../CommonServerSource/ServerStatistics.h"
#include "../../../../Common/CommonSource/Gacha/Gacha.h"
#include "../../../../Common/CommonSource/Gacha/GachaPurchase.h"
#include "../../../../Common/CommonSource/Contents/Contents.h"
#include "../../../../Common/CommonSource/ClanMatch/ClanMatchSeason.h"
#include "../../../../Common/CommonSource/S2MO/S2MO.h"
#include "../../../../Common/CommonSource/S2MO/S2MODefine.h"
#include "../../CommonServerSource/MultiSlot/MultiSlot.h"
#include "../../CommonServerSource/UserInven.h"

#include "../../CommonServerSource/MedalServer/MedalServerDef.h"

#include "../../CommonStructure/ShareServer/ProtocolServerMedalStruct.h"

#include "../../CommonStructure/ShareServer/ProtocolServerStruct.h"

#include "ServerDef.h"
#include "ModuleServiceLogFile.h"
#include "Config.h"
#include "TransDefine.h"

#include "DataBase_Script.h"
