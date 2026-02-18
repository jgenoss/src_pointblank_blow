#if !defined( _GLOBALVARIABLES_H__)
#define _GLOBALVARIABLES_H__

#include "LocaleValue.h"
#include "Config.h"
#include "ConfigEx.h"
#include "TempConfig.h"
#include "EnvSet.h"
#include "SkillSystemValueSet.h"

#include "GameFramework.h"
#include "Camera/GameCamera.h"
#include "GameCharaManager.h"
#include "GameContext.h"
#include "GameEventSender.h"
#include "GameEventReceiver.h"
#include "RandTable.h"
#include "TracerBulletManager.h"
#include "Camera/GameCameraManager.h"
#include "SplashScreenEx.h"
#include "GameStringTable.h"
#include "GameStringMap.h"
#include "EffectManager.h"
#include "PhysixInfo.h"
#include "ShopItemInfo.h"
#include "GameSoundManager.h"
#include "WeaponInfoDataBase.h"
#include "GameWeaponManager.h"
#include "EdgeTrailer.h"
#include "CharaInfoDataBase.h"
#include "GameInterface.h"
#include "ServerInfo.h"
#include "GameEvent.h"

class CClanMarkManager;

extern GameInterface *			g_pInterface;
extern CGameFramework	*		g_pFramework;
extern i3Viewer			*		g_pViewer;
extern CGameCamera		*		g_pCamera;
extern CGameCharaManager *		g_pCharaManager;
extern CGameWeaponManager *		g_pWeaponManager;
extern CTracerBulletManager *	g_pTracerBulletManager;
extern i3GuiRoot		*		g_pGUIRoot;
extern CEffectManager *			g_pEffectManager;
extern CGameSoundManager *		g_pSndMng;
extern CClanMarkManager *		g_pClanMarkManager;

extern CGameContext		*		g_pGameContext;
extern CRandTable		*		g_pRand;
extern CServerInfoContext *		g_pServerInfo;

extern PhysixInfoDataBase *		g_pPhysixInfoDataBase;
extern WeaponInfoDataBase	*	g_pWeaponInfoDataBase;
extern ShopItemInfoDataBase *	g_pShopItemInfoDataBase;

extern HWND					g_hWnd;
extern HINSTANCE			g_hInstance;
extern HIMC					g_hIMC;

extern CharaInfoDataBase	*	g_pCharaInfoDataBase;
extern CSplashScreenEx		*	g_pSplash;

extern INT_PTR					g_iBugTrapLogHandle;

extern CEdgeTrailer *			g_pEdgeTrail;

extern bool						g_bPause;
extern bool						g_bStep;

extern char						g_strVersion[256];
extern char						g_strLocale[256];
extern char						g_strVersionBuild[256];

extern bool						g_DidThisInactive;
extern bool						g_ShopState;
extern bool						g_FirstShop;
extern bool						g_FirstIGW;

extern STAGE_MODE				g_DinoStageType;

#if defined( LOCALE_INDONESIA)
namespace Warnet_Premium
{
	extern char g_szRegKey[MAX_INDONESIA_PCBANG_KEY];
};
#endif

//////////////////////////////////////////////////////////////////////////
//	디버깅용 싱글 모드만
extern CHARA_RES_ID	g_DebugChara[2];
extern CHARA_RES_ID	g_AiDebugChara[2];
extern UINT32		g_DebugWeaponSet[ 6 ][ 2 ];
extern i3::string	g_DebugStartingMap;
extern bool			g_DebugSpaceMode;

#if !defined (I3_NO_PROFILE)
#define LW_TRACE				if( g_pFramework != nullptr)g_pFramework->LogMessageToDbgWindow				
#endif

extern i3::vector_map<INT32, void*> g_mGameInfoCharaCon;
extern i3::vector_map<INT32, void*> g_mGameInfoBulletCon;

//	 현재 FPS 카메라 여부
inline bool		IsCamFPS( void)	{ return i3::same_of<CGameCamera_FPS*>(g_pCamera );  }	

//	 현재 Kill 카메라 여부
inline bool		IsCamKill( void) { return i3::same_of<CGameCamera_Kill*>(g_pCamera ); }

#define COLOR_CASHITEM_COUNT		256	//	변경 색상 샘플 갯수 

//	캐시 아이템 컬러 변경 색상표 (GUI와 인게임에서 사용해야 합니다. 클랜명, 닉네임 등등 씁니다.)
//extern const I3COLOR	g_ColorCashItem[COLOR_CASHITEM_COUNT];
//extern void Get_ColorCashItemString(char StringBuffer[], size_t StringBufferSize, int Index);

extern const I3COLOR *	GetColorCashItem(UINT8 idx);
extern const I3COLOR *	GetNickColor( const USER_INFO_INVITEM_DATA * pInfo);
extern const I3COLOR *	GetNickColor( const UINT8 ui8Color );
extern const I3COLOR *	GetClanNameColor( const USER_INFO_BASIC * pInfo);
extern const I3COLOR *	GetCrosshairColor( const USER_INFO_INVITEM_DATA * pData);
extern const I3COLOR *	GetChattingColor( const USER_INFO_INVITEM_DATA * pData);
extern const I3COLOR * GetColorbyIdx(const int type, const size_t Index);
extern void		SetNameUsingColorTable(i3::wstring& outDest, const i3::wliteral_range& wstrName, UINT8 color );

const char * GetDefaultLocaleFontName();
const i3TextFontSetInfo *	GetDefaultFontName();
void				SetDefaultFont(const i3TextFontSetInfo & FontSetInfo);

extern UINT32			GetCodePage(void);		//	Locale code page
extern void				ConvertLocalePath(char * pszOut, const char * pszBase, const char * pszLocalePath);
extern const char *		GetLocalePath();
extern UINT32			GetDefaultWeapon( WEAPON_SLOT_TYPE usage, WEAPON_CLASS_TYPE classType);
extern INT8				HexToInt(char chHex);								// [0-9a-fA-F]

extern bool				isNetThrowWeapon( WEAPON_CLASS_TYPE	classType, WEAPON::RES_ID number, WEAPON::FIRE_ORDER fireOrder, bool bWeaponExtension);
extern bool				isNetThrowWeapon( const GAMEINFO_CHARA_WEAPON * pInfo);

extern i3::string g_pStringFilePath;

extern const char* GetConfigLocale();
extern const INT32 GetLanguageType();
extern const UI_FRAMEWORK_VERSION GetUIVersionType();
extern void SetStringFilePath(INT32 languageType);

extern const std::string& __get_user_id();
extern void __setlogId( LPCSTR szFile ); 

extern void				SetDebugWeaponSet(void);

#if /*defined( LOCALE_KOREA) ||*/ defined( LOCALE_THAILAND) /*|| defined ( LOCALE_SINGMAL ) || defined (LOCALE_VIETNAM)*/
#define NICKNAME_CHAR_COUNT			(i3Language::GetCharMaxCount(GetCodePage(), NET_NICK_NAME_SIZE))
#else
#define NICKNAME_CHAR_COUNT			(NET_NICK_NAME_SIZE/2)
#endif

#define CHECK_UNICODE(wchar)		(wchar == 0xfeff)		//	유니코드 헤더 검사 (유니코드면 true)


#define PB_ASSERT(__C)\
{\
	GameEventSender::i()->SetEventOnce(EVENT_N_ABNORMAL_END, __FILE__, __LINE__); \
	Sleep(1000);\
	I3ASSERT(__C)\
}



#define SET_ROOM_BASIC_WEAPON_SHOTGUN( flag )		(( flag  & LOCK_BASIC ) | LOCK_SHOTGUN )

#if defined (LOCALE_KOREA)
	#define SET_ROOM_BASIC_WEAPON_SNIPER( flag )		(( flag  & LOCK_BASIC ) | LOCK_SNIPER )
#else 
	#define SET_ROOM_BASIC_WEAPON_SNIPER( flag )		(((( flag  & LOCK_BASIC ) | LOCK_SNIPER )&~LOCK_SECONDARY)&~LOCK_THROW)
#endif

#define SET_ROOM_BASIC_WEAPON_RPG7( flag )			(( flag  & LOCK_BASIC ) | LOCK_RPG7 )
#define SET_ROOM_BASIC_WEAPON_PRIMARY( flag )		(( flag  & LOCK_BASIC ))

#define SET_ROOM_BASIC_WEAPON_KNUCKLE( flag )		( flag | LOCK_KNUCKLE )

#define GET_ROOM_BASIC_WEAPON_SHOTGUN( flag )		( flag  & LOCK_SHOTGUN )
#define GET_ROOM_BASIC_WEAPON_SNIPER( flag )		( flag  & LOCK_SNIPER )
#define GET_ROOM_BASIC_WEAPON_RPG7( flag )			( flag  & LOCK_RPG7 )
#define GET_ROOM_BASIC_WEAPON_KNUCKLE( flag )			( flag  & LOCK_KNUCKLE )


// 관전기능
#define LOCK_OBSERVER_CHASE		0x01
#define LOCK_OBSERVER_FREE		0x02
#define LOCK_OBSERVER_ENEMY		0x04
#define LOCK_OBSERVER_ENEMY_HP	0x08
#define LOCK_OBSERVER_1PVONLY   0x10

#define		GOTO_EXIT_STAGE()	\
{\
	g_pFramework->GotoNextStage("Exit");\
	I3PRINTLOG(I3LOG_NOTICE,"Exit!");\
}


// GUIRoot가 입력을 받을 수 있을때만 작동 - 주로 메시지박스 출력시 입력 제한용
// ! praptor - 삭제 예정입니다. 사용하지 말아주세요.
#define GUI_STROKE(key)					(g_pGUIRoot && !g_pGUIRoot->getGuiDisableInput() && g_pFramework->getSysKeyStroke() & (key))


#endif
