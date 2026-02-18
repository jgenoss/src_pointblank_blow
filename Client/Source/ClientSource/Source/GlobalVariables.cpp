#include "pch.h"
#include "GlobalVariables.h"
#include "PBLog.h"

#include "i3Base/string/ext/extract_directoryname_filename.h"
#include "i3Base/string/ext/generic_string_cat.h"

GameInterface *			g_pInterface = nullptr;
CGameFramework	*		g_pFramework= nullptr;
i3Viewer		*		g_pViewer	= nullptr;
CGameCamera		*		g_pCamera	= nullptr;
//CGameCharaBase	*		g_pPlayer	= nullptr;
CGameCharaManager *		g_pCharaManager = nullptr;
CGameWeaponManager *	g_pWeaponManager = nullptr;
CTracerBulletManager *	g_pTracerBulletManager = nullptr;
i3GuiRoot		*		g_pGUIRoot = nullptr;

CRandTable		*		g_pRand			= nullptr;
CServerInfoContext *	g_pServerInfo	= nullptr;

WeaponInfoDataBase	*	g_pWeaponInfoDataBase = nullptr;
ShopItemInfoDataBase *  g_pShopItemInfoDataBase = nullptr;

HWND					g_hWnd;
HINSTANCE				g_hInstance;
HIMC					g_hIMC = nullptr;
PhysixInfoDataBase *	g_pPhysixInfoDataBase = nullptr;
CharaInfoDataBase	*	g_pCharaInfoDataBase = nullptr;
//CGameCameraManager *	g_pCameraManager	= nullptr;
CSplashScreenEx		*	g_pSplash = nullptr;

CGameContext	*		g_pGameContext	= nullptr;

CEffectManager	*		g_pEffectManager = nullptr;
CGameSoundManager *		g_pSndMng = nullptr;
CClanMarkManager *		g_pClanMarkManager = nullptr;

INT32					g_LocalHitCount;
INT32					g_NetHitCount;
REAL32					g_HitSyncRate;

INT_PTR					g_iBugTrapLogHandle = -1;

#if 0
CPBLog*					g_pLog = nullptr;
#endif

CEdgeTrailer *			g_pEdgeTrail = nullptr;

// 중국 판호버전 : 하동익
// 중국 버전정보용 임시 변수입니다.
const char				g_Version[10]="1.0.0.0";

bool					g_bPause = false;
bool					g_bStep = false;

char					g_strVersion[256];
char					g_strVersionBuild[256];

bool					g_DidThisInactive = false;
bool					g_ShopState = true;
bool					g_FirstShop = true;
bool					g_FirstIGW = true;

//////////////////////////////////////////////////////////////////////////
// LOCALE Test 코드 - 디버깅용으로 임시로 사용하세요. - Chaen
#if 0
	#undef DEV_BUILD
	#undef LOCALE_KOREA
	#undef LOCALE_THAILAND
	#undef LOCALE_INDONESIA
	#undef LOCALE_RUSSIA
	#undef LOCALE_TURKEY
	#undef LOCALE_BRAZIL
	#undef LOCALE_LATIN_AMERICA

	//#define DEV_BUILD
	//#define LOCALE_KOREA
	//#define LOCALE_THAILAND
	//#define LOCALE_INDONESIA
	//#define LOCALE_RUSSIA
	//#define LOCALE_TURKEY
	//#define LOCALE_BRAZIL
	#define LOCALE_LATIN_AMERICA
#endif

//////////////////////////////////////////////////////////////////////////
//	LOCALE SETTING
//	국가가 추가될시 여기도 추가해야 합니다. 임의로 수정하지 마세요...komet
#if defined(LOCALE_KOREA) && defined(DEV_BUILD)
static	UINT32				s_nCodePage			= I3_LANG_CODE_KOREAN;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_DEV;
static	INT32				s_nUIVersionType	= UFV_1_5;

#elif defined(LOCALE_KOREA)
static	UINT32				s_nCodePage			= I3_LANG_CODE_KOREAN;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_KOREA;
static	INT32				s_nUIVersionType = UFV_1_5;

#elif defined(LOCALE_THAILAND)
static	UINT32				s_nCodePage			= I3_LANG_CODE_THAI;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_THAILAND;
static	INT32				s_nUIVersionType = UFV_1_5;

#elif defined(LOCALE_INDONESIA)
static	UINT32				s_nCodePage			= I3_LANG_CODE_ENGLISH_US;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_INDONESIA;
static	INT32				s_nUIVersionType = UFV_1_5;
	namespace Warnet_Premium
	{
		char g_szRegKey[MAX_INDONESIA_PCBANG_KEY] = {0,};
	};

#elif defined(LOCALE_RUSSIA)		//	러시아 (동유럽어)
static	UINT32				s_nCodePage			= I3_LANG_CODE_RUSSIA;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_RUSSIA;
static	INT32				s_nUIVersionType = UFV_1_2;

#elif defined(LOCALE_TURKEY)
static	UINT32				s_nCodePage			= I3_LANG_CODE_TURKISH;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_TURKEY;
static	INT32				s_nUIVersionType = UFV_1_5;

#elif defined(LOCALE_BRAZIL)
static	UINT32				s_nCodePage			= I3_LANG_CODE_WESTEUROPE;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_BRAZIL;
static	INT32				s_nUIVersionType = UFV_1_1;

#elif defined(LOCALE_LATIN_AMERICA)
static	UINT32				s_nCodePage			= I3_LANG_CODE_WESTEUROPE;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_LATIN_AMERICA;
static	INT32				s_nUIVersionType = UFV_1_1;

#elif defined( LOCALE_NORTHAMERICA)
static	UINT32				s_nCodePage			= I3_LANG_CODE_ENGLISH_US;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_NORTHAMERICA;
static	INT32				s_nUIVersionType = UFV_1_1;

#elif defined( LOCALE_ITALY)
static	UINT32				s_nCodePage			= I3_LANG_CODE_WESTEUROPE;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_ITALY;
static	INT32				s_nUIVersionType = UFV_1_5;

#elif defined( LOCALE_ENGLISH)
static	UINT32				s_nCodePage			= I3_LANG_CODE_ENGLISH_US;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_ENGLISH;
static	INT32				s_nUIVersionType = UFV_1_5;

#elif defined( LOCALE_MIDDLE_EAST )
static	UINT32				s_nCodePage			= I3_LANG_CODE_ARABIC;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_MIDDLE_EAST;
static	INT32				s_nUIVersionType = UFV_1_5;

#elif defined( LOCALE_PHILIPPINES   )
static	UINT32				s_nCodePage			= I3_LANG_CODE_ENGLISH_US;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_PHILIPPINES;
static	INT32				s_nUIVersionType	= UFV_1_1;

#elif defined( LOCALE_TAIWAN )
static	UINT32				s_nCodePage			= I3_LANG_CODE_CHINESE_TRADITIONAL;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_TAIWAN;
static	INT32				s_nUIVersionType = UFV_1_5;

#elif defined( LOCALE_SINGMAL )
static	UINT32				s_nCodePage			= I3_LANG_CODE_ENGLISH_US;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_SINGMAL;
static	INT32				s_nUIVersionType = UFV_1_5;

#elif defined( LOCALE_VIETNAM )
static	UINT32				s_nCodePage			= I3_LANG_CODE_VIETNAMESE;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_VIETNAM;
static	INT32				s_nUIVersionType = UFV_1_5;

#elif defined( LOCALE_JAPAN)
static	UINT32				s_nCodePage			= I3_LANG_CODE_JAPANESE;
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_JAPAN;
static	INT32				s_nUIVersionType = UFV_1_5;

#else
static	UINT32				s_nCodePage			= I3_LANG_CODE_ENGLISH_US;		// 영어
static	INT32				s_nLanguageType		= RESOURCE_LANGUAGE_DEV;
static	INT32				s_nUIVersionType = UFV_1_5;

#endif


#if defined(LOCALE_KOREA)
	char	g_strLocale[256] = " (LOCALE:KOREA)";
#elif defined(LOCALE_THAILAND)
	char	g_strLocale[256] = " (LOCALE:THAI)";
#elif defined(LOCALE_INDONESIA)
	char	g_strLocale[256] = " (LOCALE:INDONESIA)";
#elif defined(LOCALE_RUSSIA)
	char	g_strLocale[256] = " (LOCALE:RUSSIA)";
#elif defined(LOCALE_TURKEY)
	char	g_strLocale[256] = " (LOCALE:TURKEY)";
#elif defined(LOCALE_BRAZIL)
	char	g_strLocale[256] = " (LOCALE:BRAZIL)";
#elif defined(LOCALE_LATIN_AMERICA)
	char	g_strLocale[256] = " (LOCALE:LATIN_AMERICA)";
#elif defined(LOCALE_NORTHAMERICA)
	char	g_strLocale[256] = " (LOCALE:NORTHAMERICA)";
#elif defined(LOCALE_ITALY)
	char	g_strLocale[256] = " (LOCALE:ITALY)";
#elif defined(LOCALE_ENGLISH)
	char	g_strLocale[256] = " (LOCALE:ENGLISH)";
#elif defined(LOCALE_MIDDLE_EAST)
	char	g_strLocale[256] = " (LOCALE:MiddleEast)";
#elif defined(LOCALE_PHILIPPINES  )
	char	g_strLocale[256] = " (LOCALE:Philippines)";
#elif defined(LOCALE_TAIWAN)
	char	g_strLocale[256] = " (LOCALE:TAIWAN)";
#elif defined(LOCALE_SINGMAL)
	char	g_strLocale[256] = " (LOCALE:SINGMAL)";
#elif defined(LOCALE_VIETNAM)
	char	g_strLocale[256] = " (LOCALE:VIETNAM)";
#elif defined(LOCALE_JAPAN)
	char	g_strLocale[256] = " (LOCALE:JAPAN)";
#else
	char	g_strLocale[256] = " (LOCALE:UNKNOWN)";
#endif


//////////////////////////////////////////////////////////////////////////
//	디버깅용 싱글 모드만 가능합니다.
CHARA_RES_ID g_DebugChara[2] =
{
	CHARA_RES_ID_ACID_POLE,
	CHARA_RES_ID_SWAT_FEMALE,			//	싱글 더미
};

CHARA_RES_ID g_AiDebugChara[2] =
{
	CHARA_RES_ID_SWAT_FEMALE,		//	싱글 주인공
	CHARA_RES_ID_ACID_POLE,			//	싱글 더미
};


//	디버깅시 싱글모드에서 기본적으로 구성된 무기 셋팅입니다.
T_ItemID	g_DebugPartsSet[ EQUIP::ePART_COUNT];
UINT32		g_DebugWeaponSet[WEAPON_SLOT_COUNT][2];
i3::string	g_DebugStartingMap;
bool		g_DebugSpaceMode;

i3::vector_map<INT32, void*> g_mGameInfoCharaCon;
i3::vector_map<INT32, void*> g_mGameInfoBulletCon;

//	캐시 아이템 컬러 변경 색상표 (GUI와 인게임에서 사용해야 합니다.)
//const I3COLOR g_ColorCashItem[COLOR_CASHITEM_COUNT] =
//{
//	I3COLOR( 255, 255, 255, 255 ),	//	흰색
//	I3COLOR( 121,  23,   0, 255 ),	//	빨강
//	I3COLOR( 227,  96,   0, 255 ),	//	주황
//	I3COLOR( 255, 211,   3, 255 ),	//	노랑
//	I3COLOR( 196, 213,   0, 255 ),	//	잔디
//	I3COLOR( 128, 217,   0, 255 ),	//	연두
//	I3COLOR(  32, 171,   0, 255 ),	//	초록
//	I3COLOR(   3, 181, 255, 255 ),	//	밝은 파랑
//	I3COLOR(   0,  96, 227, 255 ),	//	파랑
//	I3COLOR(   0,  35, 107, 255 ),	//	짙은 파랑
//	I3COLOR( 115,  37, 117, 255 ),	//	보라
//};

/*
void Get_ColorCashItemString(char StringBuffer[], size_t StringBufferSize, int Index)
{
	I3_BOUNDCHK( Index, COLOR_CASHITEM_COUNT);

	sprintf_s(StringBuffer, StringBufferSize, "%d,%d,%d,%d",
		g_ColorCashItem[ Index ].r,
		g_ColorCashItem[ Index ].g,
		g_ColorCashItem[ Index ].b,
		g_ColorCashItem[ Index ].a);
}
*/

void SetDebugWeaponSet()
{
	g_DebugWeaponSet[ 0 ][ 0 ] = WEAPON_CLASS_ASSAULT;
	g_DebugWeaponSet[ 0 ][ 1 ] = DEFAULT_WEAPON_ASSAULT;

	g_DebugWeaponSet[ 1 ][ 0 ] = WEAPON_CLASS_HANDGUN;
	g_DebugWeaponSet[ 1 ][ 1 ] = DEFAULT_WEAPON_HANDGUN;

	g_DebugWeaponSet[ 2 ][ 0 ] = WEAPON_CLASS_KNIFE;
	g_DebugWeaponSet[ 2 ][ 1 ] = DEFAULT_WEAPON_KNIFE;

	g_DebugWeaponSet[ 3 ][ 0 ] = WEAPON_CLASS_THROWING_GRENADE;
	g_DebugWeaponSet[ 3 ][ 1 ] = DEFAULT_WEAPON_THROWING1;

	g_DebugWeaponSet[ 4 ][ 0 ] = WEAPON_CLASS_THROWING_CONTAIN;
	g_DebugWeaponSet[ 4 ][ 1 ] = DEFAULT_WEAPON_THROWING2;

	g_DebugWeaponSet[ 5 ][ 0 ] = WEAPON_CLASS_MISSION;
	g_DebugWeaponSet[ 5 ][ 1 ] = DEFAULT_WEAPON_MISSON;
}

i3::string g_pStringFilePath;

const INT32 GetLanguageType()
{
	return s_nLanguageType;
}

const UI_FRAMEWORK_VERSION GetUIVersionType()
{
	return static_cast<UI_FRAMEWORK_VERSION>(s_nUIVersionType);
}

const char* GetConfigLocale()
{
	switch(s_nLanguageType)
	{
	case RESOURCE_LANGUAGE_DEV:
		return "/Dev/";

	case RESOURCE_LANGUAGE_KOREA:
		return "/korea/";

	case RESOURCE_LANGUAGE_THAILAND:
		return "/thai/";

	case RESOURCE_LANGUAGE_INDONESIA:
		return "/Indonesia/";

	case RESOURCE_LANGUAGE_RUSSIA:
		return "/russia/";

	case RESOURCE_LANGUAGE_TURKEY:
		return "/Turkey/";

	case RESOURCE_LANGUAGE_BRAZIL:
		return "/Brazil/";

	case RESOURCE_LANGUAGE_LATIN_AMERICA:
		return "/Latin_America/";

	case RESOURCE_LANGUAGE_NORTHAMERICA:
		return "/NorthAmerica/";

	case RESOURCE_LANGUAGE_ITALY:
		return "/Italy/";

	case RESOURCE_LANGUAGE_ENGLISH:
		return "/English/";

	case RESOURCE_LANGUAGE_MIDDLE_EAST:
		return "/MiddleEast/";

	case RESOURCE_LANGUAGE_PHILIPPINES:
		return "/Philippines/";

	case RESOURCE_LANGUAGE_TAIWAN:
		return "/Taiwan/";

	case RESOURCE_LANGUAGE_SINGMAL:
		return "/Singmal/";

	case RESOURCE_LANGUAGE_VIETNAM:
		return "/Vietnam/";

	case RESOURCE_LANGUAGE_JAPAN:
		return "/Japan/";

	default:
		return "/Dev/";
	}
}

void SetStringFilePath(INT32 languageType)
{
	switch(languageType)
	{
	case RESOURCE_LANGUAGE_DEV:
		i3::sprintf( g_pStringFilePath, "/Dev/");
		break;
	case RESOURCE_LANGUAGE_KOREA:
		i3::sprintf( g_pStringFilePath, "/korea/");
		break;
	case RESOURCE_LANGUAGE_THAILAND:
		i3::sprintf( g_pStringFilePath, "/thai/");
		break;
	case RESOURCE_LANGUAGE_INDONESIA:
		i3::sprintf( g_pStringFilePath, "/Indonesia/");
		break;
	case RESOURCE_LANGUAGE_RUSSIA:
		i3::sprintf( g_pStringFilePath, "/russia/");
		break;
	case RESOURCE_LANGUAGE_TURKEY:
		i3::sprintf( g_pStringFilePath, "/Turkey/");
		break;
	case RESOURCE_LANGUAGE_BRAZIL:
		i3::sprintf( g_pStringFilePath, "/Brazil/");
		break;
	case RESOURCE_LANGUAGE_LATIN_AMERICA:
		i3::sprintf( g_pStringFilePath, "/Latin_America/");
		break;
	case RESOURCE_LANGUAGE_NORTHAMERICA:
		i3::sprintf( g_pStringFilePath, "/NorthAmerica/");
		break;
	case RESOURCE_LANGUAGE_ITALY:
		i3::sprintf( g_pStringFilePath, "/Italy/");
		break;
	case RESOURCE_LANGUAGE_ENGLISH:
		i3::sprintf( g_pStringFilePath, "/English/");
		break;
	case RESOURCE_LANGUAGE_MIDDLE_EAST:
		i3::sprintf( g_pStringFilePath, "/MiddleEast/");
		break;
	case RESOURCE_LANGUAGE_PHILIPPINES:
		i3::sprintf( g_pStringFilePath, "/Philippines/");
		break;
	case RESOURCE_LANGUAGE_TAIWAN:
		i3::sprintf( g_pStringFilePath, "/Taiwan/");
		break;
	case RESOURCE_LANGUAGE_SINGMAL:
		i3::sprintf( g_pStringFilePath, "/Singmal/");
		break;
	case RESOURCE_LANGUAGE_VIETNAM:
		i3::sprintf( g_pStringFilePath, "/Vietnam/");
		break;
	case RESOURCE_LANGUAGE_JAPAN:
		i3::sprintf(g_pStringFilePath, "/Japan/");
		break;
	default:
		i3::sprintf( g_pStringFilePath, "/Dev/");
		break;
	}
}

const I3COLOR * GetColorCashItem(UINT8 idx)
{
	I3ASSERT_RETURN( idx < COLOR_CASHITEM_COUNT, nullptr);
	return &ColorNickPalette::i()->GetColor(idx);
}

const I3COLOR * GetNickColor( const USER_INFO_INVITEM_DATA * pInfo)
{	//	현재 닉네임의 컬러를 반환
//	if(  pInfo->_ui8NickColor < COLOR_CASHITEM_COUNT)
		return &ColorNickPalette::i()->GetColor(pInfo->_ui8NickColor);
//	return nullptr;
}

const I3COLOR * GetNickColor( const UINT8 ui8Color )
{	//	현재 닉네임의 컬러를 반환
//	if(  ui8Color < COLOR_CASHITEM_COUNT)
		return &ColorNickPalette::i()->GetColor(ui8Color);
//	return nullptr;
}

const I3COLOR * GetClanNameColor( const USER_INFO_BASIC * pInfo)
{	//	현재 클랜명의 컬러를 반환
//	if(pInfo->m_ui8ClanNamecolor < COLOR_CASHITEM_COUNT)
		return &ColorNickPalette::i()->GetColor(pInfo->m_ui8ClanNamecolor);
//	return nullptr;
}

const I3COLOR * GetCrosshairColor( const USER_INFO_INVITEM_DATA * pData)
{	//	현재 크로스헤어의 컬러를 반환
//	if( pData->_ui8CrossHairColor < COLOR_CASHITEM_COUNT)
		return &ColorNickPalette::i()->GetColor(pData->_ui8CrossHairColor);
//	return nullptr;
}

const I3COLOR * GetChattingColor( const USER_INFO_INVITEM_DATA * pData)
{	//	현재 채팅의 컬러를 반환
//	if( pData->_ui8ChattingColor < COLOR_CASHITEM_COUNT)
		return &ColorNickPalette::i()->GetColor(pData->_ui8ChattingColor);
//	return nullptr;
}

const I3COLOR * GetColorbyIdx(const int type, const size_t Index)
{
	I3ASSERT_RETURN( Index < COLOR_CASHITEM_COUNT, nullptr);
	return &ColorFireMuzzleFlash::i()->GetColor(type, Index);
}

const I3COLOR * GetCurFireEffectColor( const USER_INFO_INVITEM_DATA * pData)
{
	if( pData->_ui16MuzzleFlashColor < COLOR_CASHITEM_COUNT)
		return &ColorFireMuzzleFlash::i()->GetColor(pData->_ui16MuzzleFlashColor);

	return nullptr;
}

/*
const void SetNameUsingColorTable(i3GuiEditBox* pControl, const char* pszName, UINT8 color )
{
	if( color < COLOR_CASHITEM_COUNT)
	{
		pControl->SetTextEllipsis( pszName );
		pControl->SetTextColor( (COLOR*)&g_ColorCashItem[ color ] );
	}
}
*/
/*
const void SetNameUsingColorTable(char * pszDes, const size_t sizeDesMax, const TTCHAR* pszName, UINT8 color )
{
	switch( color )
	{
		case 0:		sprintf_s( pszDes, sizeDesMax, "{col:255,255,255,255}%s{/col}",		pszName );		break;
		case 1:		sprintf_s( pszDes, sizeDesMax, "{col:121,23,0,255}%s{/col}",		pszName );		break;
		case 2:		sprintf_s( pszDes, sizeDesMax, "{col:227,96,0,255}%s{/col}",		pszName );		break;
		case 3:		sprintf_s( pszDes, sizeDesMax, "{col:255,211,3,255}%s{/col}",		pszName );		break;
		case 4:		sprintf_s( pszDes, sizeDesMax, "{col:196,213,0,255}%s{/col}",		pszName );		break;
		case 5:		sprintf_s( pszDes, sizeDesMax, "{col:128,217,0,255}%s{/col}",		pszName );		break;
		case 6:		sprintf_s( pszDes, sizeDesMax, "{col:32,171,0,255}%s{/col}",		pszName );		break;
		case 7:		sprintf_s( pszDes, sizeDesMax, "{col:3,181,255,255}%s{/col}",		pszName );		break;
		case 8:		sprintf_s( pszDes, sizeDesMax, "{col:0,96,227,255}%s{/col}",		pszName );		break;
		case 9:		sprintf_s( pszDes, sizeDesMax, "{col:0,35,107,255}%s{/col}",		pszName );		break;
		case 10:	sprintf_s( pszDes, sizeDesMax, "{col:115,37,117,255}%s{/col}",		pszName );		break;
		default:	i3::safe_string_copy( pszDes, pszName, sizeDesMax );										break;
	}
}
*/

void SetNameUsingColorTable(i3::wstring& outDest, const i3::wliteral_range& wstrName, UINT8 color )
{
	switch( color )
	{
	case 0:		i3::sprintf( outDest, L"{col:255,255,255,255}%s{/col}",		wstrName );		break;
	case 1:		i3::sprintf( outDest, L"{col:121,23,0,255}%s{/col}",		wstrName );		break;
	case 2:		i3::sprintf( outDest, L"{col:227,96,0,255}%s{/col}",		wstrName );		break;
	case 3:		i3::sprintf( outDest, L"{col:255,211,3,255}%s{/col}",		wstrName );		break;
	case 4:		i3::sprintf( outDest, L"{col:196,213,0,255}%s{/col}",		wstrName );		break;
	case 5:		i3::sprintf( outDest, L"{col:128,217,0,255}%s{/col}",		wstrName );		break;
	case 6:		i3::sprintf( outDest, L"{col:32,171,0,255}%s{/col}",		wstrName );		break;
	case 7:		i3::sprintf( outDest, L"{col:3,181,255,255}%s{/col}",		wstrName );		break;
	case 8:		i3::sprintf( outDest, L"{col:0,96,227,255}%s{/col}",		wstrName );		break;
	case 9:		i3::sprintf( outDest, L"{col:0,35,107,255}%s{/col}",		wstrName );		break;
	case 10:	i3::sprintf( outDest, L"{col:115,37,117,255}%s{/col}",		wstrName );		break;
	default:	outDest = wstrName;										break;
	}
}



UINT32 GetCodePage(void)
{
#if 0
	return i3Language::GetCurrentCodePage();	//	OS code page
#else
	return s_nCodePage;							//	fixed code page
#endif
}

// 각 국가별 폰트 이름
const char * GetDefaultLocaleFontName()
{
#if defined(LOCALE_KOREA)
	return "돋음";				//	Korean (Unified Hangeul Code) 
#elif defined(LOCALE_THAILAND)
	return "Tahoma";			//	Thai (same as 28605, ISO 8859-15)
#elif defined(LOCALE_INDONESIA)
	return "Arial";				//	"Arial", "Times New Roman", "Courier New"
#elif defined(LOCALE_RUSSIA)
	return "Proxima Nova Rg";				//	Cyrillic
#elif defined(LOCALE_TURKEY)
	return "Calibri";
#elif defined(LOCALE_BRAZIL)
	return "Arial";				//	"Arial", "Times New Roman", "Courier New"
#elif defined(LOCALE_LATIN_AMERICA)
	return "Arial";
#elif defined(LOCALE_NORTHAMERICA)
	return "Arial";				//	"Arial", "Times New Roman", "Courier New"
#elif defined(LOCALE_ITALY)
	return "Arial";				//	"Arial", "Times New Roman", "Courier New"
#elif defined(LOCALE_ENGLISH)
	return "Arial";				//	"Arial", "Times New Roman", "Courier New"
#elif defined(LOCALE_MIDDLE_EAST)
	return "Arial";				//	"Arial", "Times New Roman", "Courier New"
#elif defined(LOCALE_PHILIPPINES  )
	return "Arial";				//	"Arial", "Times New Roman", "Courier New"
#elif defined(LOCALE_TAIWAN)
	if( GetCodePage() == I3_LANG_CODE_CHINESE_TRADITIONAL)
		return "MingLiU";
	else
		I3ASSERT_0;
#elif defined(LOCALE_VIETNAM)
	return "Arial";
#elif defined(LOCALE_JAPAN)
	return "Arial";
#else
	return "Arial";				//	"Arial", "Times New Roman", "Courier New"
#endif
}

static i3TextFontSetInfo gFontSetInfo;
const i3TextFontSetInfo *	GetDefaultFontName()
{
	return &gFontSetInfo;
}

void				SetDefaultFont(const i3TextFontSetInfo & FontSetInfo)
{
	gFontSetInfo = FontSetInfo;
}

const char *		GetLocalePath()
{
	return g_pStringFilePath.c_str();
}


void ConvertLocalePath(char * pszOut, const char * pszBase, const char * pszLocalePath)
{
	char szFileName[MAX_PATH] = "";

	i3::extract_directoryname_filename(pszBase, pszOut, szFileName);

	// LOCALE_* 에 따라 변경됩니다.
	i3::generic_string_cat( pszOut, pszLocalePath);
	i3::generic_string_cat( pszOut, szFileName);
}


#define BUFFER_SIZE 1024 //MAX_PATH


#ifdef USE_I3EXEC
static bool s_bLogStarted = false;
#else
extern bool s_bLogStarted = false;
#endif

static std::string g_UserID;
static std::string g_LastestStr;

void __setlogId( LPCSTR szFile )
{
	g_UserID = szFile;
}

const std::string& __get_user_id()
{
	return g_UserID;
}

UINT32	GetDefaultWeapon( WEAPON_SLOT_TYPE usage, WEAPON_CLASS_TYPE classType)
{
	switch( usage)
	{
	case WEAPON_SLOT_PRIMARY:		
		{
			switch( classType)
			{
			case WEAPON_CLASS_ASSAULT:		return DEFAULT_WEAPON_ASSAULT;
			case WEAPON_CLASS_SNIPER:		return DEFAULT_WEAPON_SNIPER;
			case WEAPON_CLASS_SMG:			return DEFAULT_WEAPON_SMG;
			case WEAPON_CLASS_SHOTGUN:		return DEFAULT_WEAPON_SHOTGUN;
			default:						return DEFAULT_WEAPON_ASSAULT;
			}
		}
		break;
	case WEAPON_SLOT_SECONDARY:		return DEFAULT_WEAPON_HANDGUN;
	case WEAPON_SLOT_MELEE:			return DEFAULT_WEAPON_KNIFE;
	case WEAPON_SLOT_THROWING1:		return DEFAULT_WEAPON_THROWING1;
	case WEAPON_SLOT_THROWING2:		return DEFAULT_WEAPON_THROWING2;
	}

	return 0;
}

bool isNetThrowWeapon( WEAPON_CLASS_TYPE classType, WEAPON::RES_ID number, WEAPON::FIRE_ORDER fireOrder, bool bWeaponExtension)
{
	switch(classType)
	{
	case WEAPON_CLASS_ASSAULT :
	case WEAPON_CLASS_SMG :
		// 네트워크로 발사되는 유탄 탄환 무기인지 체크
		//if( fireOrder == WEAPON::SECONDARY_FIRE)
		if( bWeaponExtension)
		{
			CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( classType, number, true);
			if( pWeaponInfo->availableExtType( WEAPON::EXT_GLAUNCHER))
			{
				return true;
			}
		}
		break;

	case WEAPON_CLASS_HANDGUN :
		{
			CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( classType, number, true);
			if( pWeaponInfo->availableExtType( WEAPON::EXT_THROW_SHELL ))
			{
				if( (fireOrder == WEAPON::PRIMARY_FIRE))
					return true;
			}
		}
		break;

	case WEAPON_CLASS_KNIFE :
		// 던지는 무기류 체크
		if( (bWeaponExtension == false) && (fireOrder == WEAPON::SECONDARY_FIRE) )
		{
			CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( classType, number, true);

			if( pWeaponInfo->IsThrowWeapon())
				return true;

			if( pWeaponInfo->availableExtType( WEAPON::EXT_THROW))
				return true;

			return false;
		}
		break;

	case WEAPON_CLASS_DUALKNIFE:
		// 던지는 무기류 체크
		if ( (fireOrder == WEAPON::SECONDARY_FIRE) )
		{
			CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(classType, number, true);

			if (pWeaponInfo->IsThrowWeapon())
				return true;

			if (pWeaponInfo->availableExtType(WEAPON::EXT_THROW))
				return true;

			return false;
		}
		break;

	case WEAPON_CLASS_BOMBTRIGGER :
		if( (fireOrder == WEAPON::SECONDARY_FIRE))
			return true;
		break;

	case WEAPON_CLASS_THROWING_GRENADE:	return true;
	case WEAPON_CLASS_THROWING_CONTAIN:	return true;
	case WEAPON_CLASS_THROWING_ATTACK:	return true;
	case WEAPON_CLASS_THROWING_HEAL:	return true;
	case WEAPON_CLASS_GRENADESHELL:		return true;
	case WEAPON_SUBCLASS_G_LAUNCHER :
	case WEAPON_CLASS_ROCKET_LAUNCHER:	return true;
	case WEAPON_SUBCLASS_BOW:
	case WEAPON_CLASS_BOW:				
	{
		if (bWeaponExtension)
		{
			CWeaponInfo * pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo(classType, number, true);
			if (pWeaponInfo->availableExtType(WEAPON::EXT_GLAUNCHER))
			{
				return true;
			}
		}
	}
	default:							return false;
	}

	return false;
}

extern bool isNetThrowWeapon( const GAMEINFO_CHARA_WEAPON * pInfo)
{
	if( pInfo == nullptr) return false;

	WEAPON_CLASS_TYPE classType = const_cast<GAMEINFO_CHARA_WEAPON*>(pInfo)->getWeaponClass();
	UINT32 u32Number = const_cast<GAMEINFO_CHARA_WEAPON*>(pInfo)->getWeaponNumber();

	return isNetThrowWeapon( classType, (WEAPON::RES_ID)u32Number, pInfo->getFireOrder(), pInfo->getWeaponExtension());
}

extern INT8 HexToInt(char chHex)
{
	if (chHex >= '0' && chHex <= '9')
	{
		return chHex - '0';
	}
	else if (chHex >= 'a' && chHex <= 'f')
	{
		return chHex - 'a' + 10;
	}
	else if (chHex >= 'A' && chHex <= 'F')
	{
		return chHex - 'A' + 10;
	}


	return 0;
}