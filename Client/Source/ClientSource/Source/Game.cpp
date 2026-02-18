#include "pch.h"
#include "Game.h"
#include "GameSupportSet.h"
#include "InGameQACommandSet.h"
#include "GameFramework.h"
#include "AllStage.h"
#include "GameChatBox.h"
#include "MessageBoxManager.h"
#include "GameWorld.h"
#include <mmsystem.h>
#include "Weapon.h"
#include "GlobalVariables.h"
#include "Crosshair.h"
#include "SecurityHeader.h"
#include "WinXPSP2FireWall.h"
#include "V3FireWall.h"
#include "PBLog.h"
#include "ClanMark.h"
#include "CommunityContext.h"
#include "ServerInfo.h"
#include "GameMaster.h"
#include "WeaponLRU_Manager.h"
#include "Weapon/WeaponItemID.h"
#include "TwitchTVContext.h"

#include "LoadingStateDlg.h"
#include "i3Base/itl/container_util.h"
#include "i3Base/string/stack_string.h"
#include "i3Base/string/algorithm/to_upper.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/utf16_to_mb.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"
#include "psapi.h"

#include "Designation/DesigUtil.h"
#include "MissionCard/MCardUtil.h"

#include "ClientStageInfoUtil.h"

#define ENABLE_TEXTURE_CHANGE_I3S


i3::vector_map<i3::string, UINT32> g_mapItemIndex;
i3::vector_map<i3::string, UINT32> g_mapUITemplateIndex_Chara;
i3::vector_map<i3::string, UINT32> g_mapUITemplateIndex_Item;
i3::vector_map<i3::string, UINT32> g_mapUITemplateIndex_Parts;

#if defined (I3_DEBUG) && !defined( USE_I3EXEC)
#pragma comment(lib, "../../../../Common/Library/lib/S2MOd_32bit.lib")
#else
#pragma comment(lib, "../../../../Common/Library/lib/S2MO_32bit.lib")
#endif

void InitForCheckHacking();
void InitForCheckGravity();

#ifdef _CONSOLELOG
#if defined (I3_DEBUG)
#pragma comment(lib, "ConsoleLogger_DEBUG.lib")
#else
#pragma comment(lib, "ConsoleLogger.lib")
#endif
#endif

CCharaGlobal2*	g_pGlobalInfo = nullptr;	// global chara info
CCharaGlobal2*	g_pGlobalDinoInfo = nullptr;	// global dino info
i3::vector<CCharaInfo2*>	g_CharaInfoList;		// Character info



UINT8			g_uShiftGlobal2 = 0;
UINT8 *			g_pGlobalCharaShift = nullptr;

//Error 
static bool	s_bExitGame = false;

#ifdef BUG_TRAP
	#include "BugTrap.h"
	#pragma comment(lib, "../lib/BugTrap.lib") // Link to ANSI DLL
	// #pragma comment(lib, "BugTrapU.lib") // Link to Unicode DLL

	static void CALLBACK MyErrHandler(INT_PTR nErrHandlerParam)
	{
#ifdef _XIGNCODE_
		ZCWAVE_SysExit();
		ZCWAVE_SysCleanup();
#endif
	}

	static void SetupExceptionHandler()
	{
		BT_InstallSehFilter();
		BT_SetAppName(_T(GAME_APP_NAME_A));
		// BT_SetSupportEMail(_T("wishmaster@zepetto.com"));

		UINT32 BT_Flag = 0;
		BT_Flag = BTF_DETAILEDMODE | BTF_ATTACHREPORT | BTF_LISTPROCESSES |  BTF_SHOWADVANCEDUI;
	#if defined (BUILD_RELEASE_QA_VERSION)	// revision 54466
		BT_Flag = BT_Flag | BTF_SHOWADVANCEDUI;
	#endif

		BT_SetFlags(BT_Flag);
		BT_SetReportFormat(BTRF_TEXT);

		BT_SetSupportServer(_T("182.162.168.94"), 9999);
		//BT_SetSupportServer(_T("203.116.49.17"), 9999);			// 구 가레나 버그트랩 서버
		//BT_SetSupportServer(_T("http://www.zepetto.com"), 9999);	// 구 Zepetto 버그트랩 서버

		BT_SetDumpType(MiniDumpNormal | MiniDumpWithDataSegs | MiniDumpWithHandleData | MiniDumpFilterMemory);
		BT_SetPreErrHandler( MyErrHandler, 0);

		TCHAR	ExePath[1024];
		GetModuleFileName(nullptr, ExePath, 1024);
		PathRemoveFileSpec(ExePath);
		TCHAR	FullPath[1024];
#if defined(NC_BUILD) || defined(USE_I3EXEC)
		PathCombine(FullPath, ExePath, BC_LOG);
#else
		PathCombine(FullPath, ExePath, __getLogName());
#endif
		BT_AddLogFile(FullPath);

		// 버그 트랩 레포트에 PreCommon.pbc 추가
		TCHAR	PreFullPath[1024];
		PathCombine(PreFullPath, ExePath, LRU_FILE);
		BT_AddLogFile(PreFullPath);

	//MiniDumpFilterModulePaths = 0x0080, 
  //MiniDumpWithProcessThreadData = 0x0100, 
  //MiniDumpWithPrivateReadWriteMemory = 0x0200

		// BT_SetSupportURL(_T("http://www.zepetto.com"));

		/*g_iBugTrapLogHandle = BT_OpenLogFile(nullptr, BTLF_TEXT);
		BT_SetLogSizeInBytes(g_iBugTrapLogHandle, 10000);
		BT_SetLogFlags(g_iBugTrapLogHandle, BTLF_SHOWTIMESTAMP);
		BT_SetLogEchoMode(g_iBugTrapLogHandle, BTLE_STDERR | BTLE_DBGOUT);*/

		/*PCTSTR pszLogFileName = BT_GetLogFileName(g_iBugTrapLogHandle);
		BT_AddLogFile(pszLogFileName);*/
	}
#endif


void LoadItemList()
{
	i3RegKey * pRootKey = Config::_GetRegRoot("Weapon/ItemList");
	INT32 iMaxIndex = 0;

	for(INT32 i=0; i<pRootKey->getChildCount(); i++)
	{
		i3RegKey* pRegKey = (i3RegKey*)pRootKey->getChild(i);
		iMaxIndex = 0;


		for(INT32 j=0; j<pRegKey->GetDataCount(); j++)
		{
			i3RegData* pData = pRegKey->GetData(j);
			if(pData->getDataType() == I3REG_TYPE_INT32) 
			{
				INT32 iTemp = 0;
				pData->GetValue(&iTemp);

				if(iTemp > iMaxIndex) {
					iMaxIndex = iTemp;
				}
				
				i3::pair<i3::vector_map<i3::string, UINT32>::iterator, bool> pr = g_mapItemIndex.insert( i3::vector_map<i3::string, UINT32>::value_type(pData->GetName(), iTemp ) );
				
				if(pr.second == false) {
					//I3ASSERT_0;
					I3PRINTLOG(I3LOG_FATAL,  "Duplicate Item %s, %d", pData->GetName(), iTemp );
				}
			}
		}

	
		{
			i3::stack_string name_upper;
			i3::to_upper_copy(name_upper, pRegKey->GetName());
			pRegKey->SetName(name_upper);
		}

		char szTemp[256];
		_snprintf(szTemp, 256, "%s_MAX", pRegKey->GetName());
		g_mapItemIndex.insert( i3::vector_map<i3::string, UINT32>::value_type(szTemp, (UINT32)iMaxIndex+1 ) );

	}

	WEAPON::Initialize();

}

UINT32 GetItemIndex( LPCSTR szName)
{
	i3::vector_map<i3::string, UINT32>::iterator It = g_mapItemIndex.find(szName);

	if( It == g_mapItemIndex.end() ) {
		return 0;
	}

	return (UINT32) It->second;
}

UINT32 GetItemCountThrowItem(void)
{
	return (WEAPON::getItemCount( WEAPON_CLASS_THROWING_CONTAIN) - 1) + (WEAPON::getItemCount( WEAPON_CLASS_THROWING_ATTACK) - 1) + WEAPON::getItemCount(WEAPON_CLASS_THROWING_HEAL);
}


///----------------
void LoadUITemplateIndex(const char* path, i3::vector_map<i3::string, UINT32>& vm)
{
	i3RegKey* rk = Config::_GetRegRoot(path);
	for (INT32 i=0; i<rk->GetDataCount(); i++)
	{
		i3RegData* data = rk->GetData(i);
		if (data->getDataType() == I3REG_TYPE_INT32)
		{
			INT32 type = -1;
			data->GetValue(&type);

			i3::pair<i3::vector_map<i3::string, UINT32>::iterator, bool> pr = 
				vm.insert(i3::vector_map<i3::string, UINT32>::value_type(data->GetName(), type));

			if (pr.second == false)
				I3PRINTLOG(I3LOG_FATAL,  "Duplicate Item %s, %d", data->GetName(), type);
		}
	}

	i3::stack_string name_upper;
	i3::to_upper_copy(name_upper, rk->GetName());
	rk->SetName(name_upper.c_str());

	char szTemp[256];
	_snprintf(szTemp, 256, "%s_MAX", rk->GetName());
	i3::vector_map<i3::string, UINT32>::iterator iter = std::max_element(vm.begin(), vm.end());

	vm.insert(i3::vector_map<i3::string, UINT32>::value_type(szTemp, iter->second+1));
}

UINT32 GetUITemplateIndex_Chara(LPCSTR szName)
{
	i3::vector_map<i3::string, UINT32>::iterator It = g_mapUITemplateIndex_Chara.find(szName);

	if( It == g_mapUITemplateIndex_Chara.end() ) {
		return 0;
	}

	return (UINT32) It->second;
}

UINT32 GetUITemplateIndex_Item(LPCSTR szName)
{
	i3::vector_map<i3::string, UINT32>::iterator It = g_mapUITemplateIndex_Item.find(szName);

	if( It == g_mapUITemplateIndex_Item.end() ) {
		return 0;
	}

	return (UINT32) It->second;
}

UINT32 GetUITemplateIndex_Parts(LPCSTR szName)
{
	i3::vector_map<i3::string, UINT32>::iterator It = g_mapUITemplateIndex_Parts.find(szName);

	if( It == g_mapUITemplateIndex_Parts.end() ) {
		return 0;
	}

	return (UINT32) It->second;
}
///----------------
void SetAICharacterWeapon()
{
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, GetItemIndex("ASSAULT_SG550_DOTSIGHT")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, GetItemIndex("ASSAULT_AK47_DUALMAG")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, GetItemIndex("ASSAULT_M4A1_SCOPE2")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, GetItemIndex("ASSAULT_K2")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, GetItemIndex("ASSAULT_F2000_SCOPE2")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, GetItemIndex("ASSAULT_AK47_GOLD")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, GetItemIndex("ASSAULT_M4A1_CAMO")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, GetItemIndex("ASSAULT_K201_GL")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, GetItemIndex("ASSAULT_G36C_SCOPE2")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, GetItemIndex("ASSAULT_SG550_SE")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_ASSAULT, GetItemIndex("ASSAULT_AK_SOPMOD")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, GetItemIndex("SMG_MP5K_SILENCER")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, GetItemIndex("SMG_SPECTRE_STEADYAIM")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, GetItemIndex("SMG_K1_ZOOM")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, GetItemIndex("SMG_MP7_DOTSIGHT")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, GetItemIndex("SMG_K1")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, GetItemIndex("SMG_MP5K_SERIF")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, GetItemIndex("SMG_UMP45_DUALMAG")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, GetItemIndex("SMG_SPECTRE_SE")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, GetItemIndex("SMG_P90MC_DOTSIGHT")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, GetItemIndex("SMG_P90_DOTSIGHT")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SMG, GetItemIndex("SMG_KRISSSUPERV")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SNIPER, GetItemIndex("SNIPER_DRAGUNOV")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SNIPER, GetItemIndex("SNIPER_PSG1")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SNIPER, GetItemIndex("SNIPER_SSG69")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SNIPER, GetItemIndex("SNIPER_SSG69_CAMO")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SNIPER, GetItemIndex("SNIPER_L115A1")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SNIPER, GetItemIndex("SNIPER_DRAGUNOV_GOLD")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SNIPER, GetItemIndex("SNIPER_PSG1_S")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SHOTGUN, GetItemIndex("SHOTGUN_870MCS")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SHOTGUN, GetItemIndex("SHOTGUN_SPAS15")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_SHOTGUN, GetItemIndex("SHOTGUN_870MCS_W")));
	g_vecAI_UsablePrimaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_PRIMARY, WEAPON_CLASS_MG,		GetItemIndex("MG_MK46")));

	g_vecAI_UsableSecondaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_SECONDARY, WEAPON_CLASS_HANDGUN, GetItemIndex("HANDGUN_DESERTEAGLE")));
	g_vecAI_UsableSecondaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_SECONDARY, WEAPON_CLASS_HANDGUN, GetItemIndex("HANDGUN_MK23_SILENCER")));
	g_vecAI_UsableSecondaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_SECONDARY, WEAPON_CLASS_HANDGUN, GetItemIndex("HANDGUN_K5")));
	g_vecAI_UsableSecondaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_SECONDARY, WEAPON_CLASS_HANDGUN, GetItemIndex("HANDGUN_DESERTEAGLE_REF")));
	g_vecAI_UsableSecondaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_SECONDARY, WEAPON_CLASS_CIC, GetItemIndex("CIC_P99")));
	g_vecAI_UsableSecondaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_SECONDARY, WEAPON_CLASS_DUALHANDGUN, GetItemIndex("DUALHANDGUN_MK23_K5")));
	g_vecAI_UsableSecondaryWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_SECONDARY, WEAPON_CLASS_DUALHANDGUN, GetItemIndex("DUALHANDGUN_DESERTEAGLE")));

	g_vecAI_UsableMeleeWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MELEE, WEAPON_CLASS_KNIFE, GetItemIndex("KNIFE_M7")));
	g_vecAI_UsableMeleeWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MELEE, WEAPON_CLASS_KNIFE, GetItemIndex("KNIFE_KUKRII")));
	g_vecAI_UsableMeleeWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MELEE, WEAPON_CLASS_KNIFE, GetItemIndex("KNIFE_MINIAXE")));
	g_vecAI_UsableMeleeWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MELEE, WEAPON_CLASS_DUALKNIFE, GetItemIndex("DUALKNIFE_M9")));
	g_vecAI_UsableMeleeWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_MELEE, WEAPON_CLASS_KNIFE, GetItemIndex("KNIFE_FANG_BLADE")));

	g_vecAI_UsableThrowingWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING1, WEAPON_CLASS_THROWING_GRENADE, GetItemIndex("THROWING_K400")));

	g_vecAI_UsableItemWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING2, WEAPON_CLASS_THROWING_ATTACK, GetItemIndex("THROWING_WP_SMOKE")));
	g_vecAI_UsableItemWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING2, WEAPON_CLASS_THROWING_CONTAIN, GetItemIndex("THROWING_SMOKE")));
	g_vecAI_UsableItemWeaponFlag.push_back(MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING2, WEAPON_CLASS_THROWING_CONTAIN, GetItemIndex("THROWING_FLASHBANG")));
}

#ifdef ENABLE_TEXTURE_CHANGE_I3S
bool LoadTextureChangei3s();
#endif


bool CreateGlobal()
{
	I3PRINTLOG(I3LOG_NOTICE, "CrateGlobal() Enter");

	if ( !LocaleValue::Init() ) 
	{
		I3PRINTLOG(I3LOG_NOTICE, "LocaleValue::Init() false");
		return false;
	}

	I3PRINTLOG(I3LOG_NOTICE, "Environment setting load >>>");
	g_pEnvSet = CEnvSet::new_object();
	I3ASSERT( g_pEnvSet != nullptr );
	I3PRINTLOG(I3LOG_NOTICE, "Environment setting load <<<");

	//	문자열 테이블 파일을 읽어들입니다.
	I3PRINTLOG(I3LOG_NOTICE, "* 1");
	
	//PointPBV.3 핵 검사 위한 암호화
	InitForCheckHacking();

	//Gravity 핵 검사 위한 함호화
	InitForCheckGravity();

	if( ! CGameStringMap::Get()->CreateForPointBlank() )
	{
		I3PRINTLOG(I3LOG_NOTICE, "* Could not load string map.");
		return false;
	}
	ApplyClientStringTable();


#ifdef BUG_TRAP
	SetupExceptionHandler();
	BT_SetTerminate();
#endif

	MakeVersion();

	I3PRINTLOG(I3LOG_DEFALUT,  "Configuration init Start");

#ifdef ENABLE_TEXTURE_CHANGE_I3S
	if ( !LoadTextureChangei3s() )
	{
		I3PRINTLOG(I3LOG_NOTICE, "LoadTextureChangei3s() false");
		return false;
	}
#endif


	// 여기서 32MB를 로드한다??
	if ( !Config::Init() ) 
	{
		I3PRINTLOG(I3LOG_NOTICE, "Config::Init() false");
		return false;
	}
	I3PRINTLOG(I3LOG_DEFALUT,  "Configuration init Finish");



	/*if ( ! ExceptedStageConfig::Init() )
		return false;*/

	bool	Rv = false;

#if 0
	g_pLog = CPBLog::new_object();

	g_pLog->SetFileName("PBLog.txt");
#endif

	// 순서 지켜야 함.
	LoadItemList();
	LoadUITemplateIndex("Character/UITemplate", g_mapUITemplateIndex_Chara);
	LoadUITemplateIndex("Goods/UITemplate", g_mapUITemplateIndex_Item);

	if( i3::contain_string( ::GetCommandLine(), "OLD_EQUIPMENT") >= 0 )
		LoadUITemplateIndex("Equipment_Backup/UITemplate", g_mapUITemplateIndex_Parts);
	else
		LoadUITemplateIndex("Equipment/UITemplate", g_mapUITemplateIndex_Parts);
	
	SetAICharacterWeapon();
	SetDebugWeaponSet();
	// 여기까지 (순서 지켜야 함.)

	g_pWeaponInfoDataBase = WeaponInfoDataBase::new_object();
	if( g_pWeaponInfoDataBase->Create() == false)
	{
		I3PRINTLOG(I3LOG_NOTICE, "* Could not create weapon database.");
		return false;
	}

	I3PRINTLOG(I3LOG_NOTICE, "Configuration load >>>");
	g_pConfig = CConfig::new_object();
	I3ASSERT( g_pConfig != nullptr );
	I3PRINTLOG(I3LOG_NOTICE, "Configuration load <<<");

	I3PRINTLOG(I3LOG_NOTICE, "Configuration Ex load >>>");
	g_pConfigEx = CConfigEx::new_object();
	I3ASSERT( g_pConfigEx != nullptr );
	Rv = g_pConfigEx->Load();
	I3ASSERT( true == Rv );
	I3PRINTLOG(I3LOG_NOTICE, "Configuration Ex load <<<");

	I3PRINTLOG(I3LOG_NOTICE, "Temp Config load >>>");
	g_pTempConfig = CTempConfig::new_object();
	I3ASSERT( g_pTempConfig != nullptr );
	I3PRINTLOG(I3LOG_NOTICE, "Temp Config load <<<");
	
	g_pSkillSystemValueSet = SkillSystemValueSet::new_object();
	I3ASSERT( g_pSkillSystemValueSet != nullptr );
	I3PRINTLOG(I3LOG_NOTICE, "SkillSystem setting load <<<");

	//Open Windows FireWall
	{
		I3PRINTLOG(I3LOG_NOTICE, "Open Firewall >>>");
		WinXPSP2FireWall fw;

		fw.Initialize();

		wchar_t szApplication[MAX_PATH];

		GetCurrentDirectoryW( MAX_PATH, szApplication );

		wcscat( szApplication, L"\\");
		wcscat( szApplication, GAME_TITLE_NAME_W);
		wcscat( szApplication, L".exe");
		fw.AddApplication( szApplication, GAME_TITLE_NAME_W);
		fw.Uninitialize();		

		I3PRINTLOG(I3LOG_NOTICE, "Open Firewall <<<");
	}

	I3PRINTLOG(I3LOG_NOTICE, "i3Net Initialize >>>");
	if( !i3Net::Init())
	{
		I3PRINTLOG(I3LOG_NOTICE, "* i3Net::Init() failed.");
		return false; 
	}
	I3PRINTLOG(I3LOG_NOTICE, "i3Net Initialize <<<");

//#ifdef NC_BUILD
	//V3외 방화벽 처리하기 
	// 전 국가가 Dedi 서버로 변경 됨에 따라 더이상 필요 없는 루틴입니다. 만 혹시 모르니.. 20131205 김현우
	//{
	//	I3PRINTLOG(I3LOG_NOTICE, "V3 Firewall openning >>>");
	//	//패킷을 받을때까지 기다린다. 
	//	CV3FireWall V3FireWall; 
	//	I3PRINTLOG(I3LOG_NOTICE, "V3 Firewall Pre-Create.");
	//	V3FireWall.Create(PRIORITY_NORMAL);
	//	I3PRINTLOG(I3LOG_NOTICE, "V3 Firewall Create complete.");
	//	INT32 Count = 0; 

	//	I3PRINTLOG(I3LOG_NOTICE, "V3 Firewall First Send");
	//	V3FireWall.Send(); 
	//	I3PRINTLOG(I3LOG_NOTICE, "V3 Firewall First Send complete");
	//	while( V3FireWall.m_State == V3_FIREWALL_WAIT)
	//	{
	//		Sleep(1000); //1 초 기다린다. 			
	//		I3PRINTLOG(I3LOG_NOTICE, "V3 Firewall %d Send", Count);
	//		if(Count > 10)//10초
	//		{
	//			I3PRINTLOG(I3LOG_NOTICE, "V3 Firewall PreTerminate");
	//			V3FireWall.m_State = V3_FIREWALL_FAIL; 
	//			V3FireWall.Terminate(); 
	//			break; 
	//		}
	//		V3FireWall.Send(); 
	//		Count++; 
	//	}

	//	if(V3FireWall.m_State != V3_FIREWALL_OK)
	//	{	//메시지 보여주고 게임 종료 
	//		//방화벽을 꺼주세요.
	//		I3PRINTLOG(I3LOG_NOTICE, "V3 Firewall Warning MessageBox");
	//		::MessageBoxW(nullptr, GAME_STRING("STR_TBL_GLOBALDEF_GAME_ERROR_TEXT1"),	// 네크워크를 사용하지 못하고 있습니다.\r\n사용하시는 방화벽에서 포인트블랭크를 허용해 주세요.
	//					GAME_STRING("STR_TBL_GLOBALDEF_GAME_ERROR_TEXT2"),			// 알림
	//					MB_OK);
	//		return false;
	//	}

	//	I3PRINTLOG(I3LOG_NOTICE, "V3 Firewall openning <<<");
	//}
//#endif

	I3PRINTLOG(I3LOG_NOTICE, "User Font creation >>>");
	CreateUserFont();
	I3PRINTLOG(I3LOG_NOTICE, "User Font creation <<<");

	CStageInfoUtil::CreateCSIContextMapData();
	
	I3PRINTLOG(I3LOG_NOTICE, "CrateGlobal() Leave");
	
	return true;
}

bool CreateGlobal2()
{
	I3PRINTLOG(I3LOG_NOTICE, "CrateGlobal2() Enter");	

	

	I3PRINTLOG(I3LOG_NOTICE, "* 2");
	g_pRand	= CRandTable::new_object();
	I3ASSERT( g_pRand != nullptr );

	I3PRINTLOG(I3LOG_NOTICE, "* 3");
	//	캐릭터
	g_pCharaInfoDataBase = CharaInfoDataBase::new_object();
	if( g_pCharaInfoDataBase->Create() == false)
	{
		I3PRINTLOG(I3LOG_NOTICE, "* Could not create character database.");
		return false;
	}

	I3PRINTLOG(I3LOG_NOTICE, "* 4");
	//	무기
	
	

	I3PRINTLOG(I3LOG_NOTICE, "* 5");
	// PhysixInfo read
	g_pPhysixInfoDataBase	= PhysixInfoDataBase::new_object();
	if( g_pPhysixInfoDataBase->Create() == false)
	{
		I3PRINTLOG(I3LOG_NOTICE, "* Could not create physics database.");
		return false;
	}

	I3PRINTLOG(I3LOG_NOTICE, "* 6");

	// 캐시 아이템 
	g_pShopItemInfoDataBase = ShopItemInfoDataBase::new_object();
	if( !g_pShopItemInfoDataBase->Create())
	{
		I3PRINTLOG(I3LOG_NOTICE, "* Could not create shop item database.");
		return false;
	}

	I3PRINTLOG(I3LOG_NOTICE, "* 7");

	//	폰트 출력전에 디폴트 크기를 지정한다.
#if defined( LOCALE_INDONESIA) || defined( LOCALE_TURKEY) || defined( LOCALE_NORTHAMERICA) || defined( LOCALE_PHILIPPINES  ) || defined( LOCALE_MIDDLE_EAST) || \
	defined( LOCALE_SINGMAL) || defined( LOCALE_VIETNAM) || defined( LOCALE_KOREA) || defined( LOCALE_JAPAN )
	i3GuiEditBox::SetDefaultFontSize( 10);
	i3GuiControl::SetCaptionDefaultFontSize( 10);
#endif

	// 캐릭터 정보를 읽습니다.
	{
		g_CharaInfoList.clear();
		g_pGlobalInfo = CCharaGlobal2::new_object();

		i3RegKey * pPef = Config::_GetPefFileRegKey("Character");
		I3ASSERT( pPef != nullptr);

		i3RegKey * pRoot = Config::FindKey("Character", pPef);
		I3ASSERT( pRoot != nullptr);
		g_pGlobalInfo->ReadInfo( pRoot);

		g_pGlobalDinoInfo = CCharaGlobal2::new_object();
		i3RegKey * pDinoRoot = Config::FindKey("Dino", pPef);
		I3ASSERT( pDinoRoot != nullptr);
		g_pGlobalDinoInfo->ReadInfo( pDinoRoot);

		INT32 dinoCnt = pDinoRoot->getChildCount();

		g_uShiftGlobal2 = (UINT8)(i3Math::Rand()%7 + 1);

		g_pGlobalInfo->OnEncryptInfo( g_uShiftGlobal2);

		INT32 cnt = pRoot->getChildCount();
		INT32 i = 0;

		I3MEM_SAFE_FREE( g_pGlobalCharaShift);
		g_pGlobalCharaShift = (UINT8*) i3MemAlloc( sizeof( UINT8) * (cnt+dinoCnt));

		for( i = 0; i < cnt; ++i)
		{
			i3RegKey * pKey = ( i3RegKey*)pRoot->getChild(i);
			I3ASSERT( pKey != nullptr);

			CCharaInfo2 * pInfo = CCharaInfo2::new_object();

			pInfo->ReadInfo(pKey);
			g_pGlobalCharaShift[i] = (UINT8)(i3Math::Rand() %7 + 1);
			pInfo->OnEncryptInfo( g_pGlobalCharaShift[i]);

			g_CharaInfoList.push_back(pInfo);
		}

		for( i = 0; i < dinoCnt; i++)
		{
			i3RegKey * pKey = ( i3RegKey*)pDinoRoot->getChild(i);
			I3ASSERT( pKey != nullptr);

			CCharaInfo2 * pInfo = CCharaInfo2::new_object();
			pInfo->ReadInfo(pKey);

			INT32 idx = cnt + i;

			g_pGlobalCharaShift[idx] = (UINT8)(i3Math::Rand() %7 + 1);
			pInfo->OnEncryptInfo( g_pGlobalCharaShift[idx]);

			g_CharaInfoList.push_back(pInfo);
		}
	}

	return true; 
}
#include "i3Base/string/ext/extract_fileext.h"
bool CreateUserFont(void)
{
	// 사용자 폰트 등록
// 	wchar_t szCurDir[MAX_STRING_COUNT] = {0,};
// 
// 	GetCurrentDirectoryW( MAX_STRING_COUNT, szCurDir);
// 	i3::generic_string_cat(szCurDir, L"\\Font\\Gunplay.ttf");
// 
// 	AddFontResourceExW( szCurDir, FR_PRIVATE, 0);
// 
// 	GetCurrentDirectoryW( MAX_STRING_COUNT, szCurDir);
// 	i3::generic_string_cat(szCurDir, L"\\Font\\YGO240.ttf");	
// 	AddFontResourceExW( szCurDir, FR_PRIVATE, 0);
// 	
// 
// //#if defined( LOCALE_KOREA)
// 	GetCurrentDirectoryW( MAX_STRING_COUNT, szCurDir);
// 	i3::rc_wstring font = LocaleValue::GetString( "_FontFile");
// 
// 	if( i3::generic_is_iequal( font, L"NONE") == false)
// 	{
// 		i3::generic_string_cat(szCurDir, L"\\Font\\");
// 		i3::generic_string_cat(szCurDir, font);
// 
// #if defined( LOCALE_JAPAN)
// 		i3::generic_string_cat(szCurDir, L".ttc");
// #else
// 		i3::generic_string_cat(szCurDir, L".ttf");
// #endif
// 
// 	//	if( i3FileStream::isFile( szCurDir))
// 		if (::GetFileAttributesW(szCurDir) != INVALID_FILE_ATTRIBUTES )
// 		{
// 			AddFontResourceExW( szCurDir, FR_PRIVATE, 0);
// 		}
// 	}
// //#endif

	char szCurDir[MAX_STRING_COUNT] = { 0, };
	 
	GetCurrentDirectory( MAX_STRING_COUNT, szCurDir);

	std::string sFontdir;

	sFontdir = szCurDir;
	sFontdir.append("\\Font\\*.*");

	WIN32_FIND_DATA findFileData;			// File Data 구조 Structure
	HANDLE hFileHandle = 0;			// File Descriptor Handle

	hFileHandle = FindFirstFile(sFontdir.c_str(), &findFileData);
	if (hFileHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			char ext[MAX_PATH];
			i3::extract_fileext(findFileData.cFileName, ext);
			// 1. 자신/상위 폴더명은 제외
			if (!((strcmp(findFileData.cFileName, ".") == 0) ||
				strcmp(findFileData.cFileName, "..") == 0) &&
				(strcmp(ext, "ttf") == 0 || strcmp(ext, "ttc") == 0 || strcmp(ext, "otf") == 0)
				)
			{
				std::string sfont;
				sfont = szCurDir;
				sfont.append("\\Font\\");
				sfont.append(findFileData.cFileName);
				if (::GetFileAttributes(sfont.c_str()) != INVALID_FILE_ATTRIBUTES)
				{
					AddFontResourceEx(sfont.c_str(), FR_PRIVATE, 0);
				}

			}
			// 3. 다음 File Handle로 접근
		} while (FindNextFile(hFileHandle, &findFileData));
	}

	FindClose(hFileHandle);

	return true;
}

bool RemoveUserFont(void)
{
	// 사용자 폰트 해제
	char szCurDir[MAX_STRING_COUNT] = {0,};
	GetCurrentDirectory( MAX_STRING_COUNT, szCurDir);
	i3::generic_string_cat(szCurDir, "\\Font\\Gunplay.ttf");	
	RemoveFontResource( szCurDir);

	return true;
}

void MakeVersion(void)
{	
	I3PRINTLOG(I3LOG_NOTICE, "MakeVersion() Enter");

	char szDate[MAX_STRING_COUNT];

#ifndef NC_BUILD
	sprintf_s( szDate, "%s_%s", __DATE__, __TIME__);
#else
	sprintf_s( szDate, "%s_2k.00", __DATE__);	// <-- 날짜 수정은 이것만 해주세요~!
#endif


#ifdef  I3_DEBUG//빌드 정보
	sprintf_s( g_strVersion, "Ver.Debug %s", szDate);
#elif BUILD_RELEASE_QA_VERSION
	sprintf_s( g_strVersion, "Ver.Release %s", szDate);
#else
	sprintf_s( g_strVersion, "Ver.Distribute_%s", szDate);
#endif

	//	Add locale info
	i3::generic_string_cat( g_strVersion, g_strLocale);

#if defined(DEV_BUILD)
	strncat(g_strVersion, " (DEV_BUILD)", i3::generic_string_size(" (DEV_BUILD)"));
#endif

#ifdef BUG_TRAP
	BT_SetAppVersion(g_strVersion);
#endif // end 

	I3PRINTLOG(I3LOG_NOTICE, "MakeVersion() Leave");	
}

bool	DestroyGlobal()
{
	I3PRINTLOG(I3LOG_NOTICE, "DestroyGlobal() Enter");	

	DestroySplashWindow();

	RemoveUserFont();

/*	CGameStringMap::Get()->Destroy();

    I3_SAFE_RELEASE( g_pConfig);
	I3_SAFE_RELEASE( g_pTempConfig);
	I3_SAFE_RELEASE( g_pConfigEx);
	I3_SAFE_RELEASE( g_pEnvSet);
	
	I3_SAFE_RELEASE( g_pSkillSystemValueSet);

	I3_SAFE_RELEASE( g_pRand );
	I3_SAFE_RELEASE( g_pWeaponInfoDataBase);
	I3_SAFE_RELEASE( g_pCharaInfoDataBase);	
	I3_SAFE_RELEASE( g_pPhysixInfoDataBase );
	I3_SAFE_RELEASE( g_pShopItemInfoDataBase);

#if 0
	I3_SAFE_RELEASE( g_pLog);
#endif

	i3::cu::for_each_safe_release_clear(g_CharaInfoList, &CCharaInfo2::Release);
	I3_SAFE_RELEASE( g_pGlobalInfo);
	I3_SAFE_RELEASE( g_pGlobalDinoInfo);
	I3MEM_SAFE_FREE( g_pGlobalCharaShift);*/

	I3MEM_SAFE_FREE( g_pGlobalCharaShift);
	I3_SAFE_RELEASE( g_pGlobalDinoInfo);
	I3_SAFE_RELEASE( g_pGlobalInfo);

	i3::cu::for_each_safe_release_clear( g_CharaInfoList, &CCharaInfo2::Release);

	I3_SAFE_RELEASE( g_pShopItemInfoDataBase);
	I3_SAFE_RELEASE( g_pPhysixInfoDataBase);
	I3_SAFE_RELEASE( g_pCharaInfoDataBase);
	I3_SAFE_RELEASE( g_pRand);

	i3Net::Destroy();

	I3_SAFE_RELEASE( g_pSkillSystemValueSet);
	I3_SAFE_RELEASE( g_pTempConfig);
	I3_SAFE_RELEASE( g_pConfigEx);
	I3_SAFE_RELEASE( g_pConfig);
	I3_SAFE_RELEASE( g_pWeaponInfoDataBase);
	CGameStringMap::Get()->Destroy();
	I3_SAFE_RELEASE( g_pEnvSet);

#if defined (_NPROTECT_)
	nProtect::Close();
#endif

//	i3Net::Destroy();

	I3PRINTLOG(I3LOG_NOTICE, "DestroyGlobal() Leave");	

	return true;
}

void CreateSplashWindow(void)
{   
	I3PRINTLOG(I3LOG_NOTICE, "CreateSplashWindow() Enter");	

	g_pSplash = new CSplashScreenEx();
	MEMDUMP_NEW( g_pSplash, sizeof(CSplashScreenEx));

	g_pSplash->Create(  NULL, 2000, CSS_FADE | CSS_CENTERSCREEN | CSS_SHADOW);
	g_pSplash->SetBitmap( "Gui/Image/SplashWindow.bmp",255,0,255);
	g_pSplash->Show();
	Sleep( 1000);

	I3PRINTLOG(I3LOG_NOTICE, "CreateSplashWindow() Leave");	
}

void DestroySplashWindow(void)
{
	I3PRINTLOG(I3LOG_NOTICE, "DestroySplashWindow() Enter");	

	I3_SAFE_DELETE(  g_pSplash);

	I3PRINTLOG(I3LOG_NOTICE, "DestroySplashWindow() Leave");	
}

void ExitGame()
{
	s_bExitGame = true;
}

bool InitManagers()
{
	I3PRINTLOG(I3LOG_NOTICE, "InitManagers() Enter");

	if( g_pCharaManager != nullptr)
	{
		g_pCharaManager->PostCreate();
	}

	I3ASSERT( g_pSndMng == nullptr);
	g_pSndMng = CGameSoundManager::new_object();
	g_pSndMng->Create( g_pViewer->GetSoundContext());

	I3ASSERT( g_pClanMarkManager == nullptr);
	g_pClanMarkManager = CClanMarkManager::new_object();
	g_pClanMarkManager->Create();

	I3PRINTLOG(I3LOG_NOTICE, "InitManagers() Leave");	
	
	return true;
}

bool DestroyManagers()
{
	I3PRINTLOG(I3LOG_NOTICE, "DestroyManagers() Enter");	

	I3_SAFE_RELEASE( g_pCharaManager );
	I3_SAFE_RELEASE( g_pWeaponManager);
	I3_SAFE_RELEASE( g_pSndMng);
	I3_SAFE_RELEASE( g_pClanMarkManager);	

	I3PRINTLOG(I3LOG_NOTICE, "DestroyManagers() Leave");	

	return true;
}

bool DriveGame()
{
	i3_prof_func(true);
	REAL_PROFILE_START_ROOT();

	if( g_pViewer != nullptr )
	{
		i3Framework::UpdateDXForThread();

		g_pViewer->UpdateInput();
		if(g_bPause == false || g_bStep == true)
		{
			if(CGameMaster::i()->IsPaused())
			{
				g_pViewer->setDeltaTime(0.0f);
			}

			g_pViewer->Update();

			g_pViewer->Trace();
			
			g_bStep = false;
		}

		i3Framework::UpdateDXForThread();
		
		g_pViewer->Begin();
		g_pViewer->OnlyRender();
		g_pViewer->End();
		
		g_pFramework->SaveVideoCapture();
		TwitchTVContext::i()->TTVStreaming();
		g_pFramework->RecLiveUpdate(TwitchTVContext::i()->isBroadCasting(),g_pFramework->IsVideoCapturing());

		i3Framework::UpdateDXForThread();

		if (g_pGameContext != nullptr)
		{
			g_pGameContext->OnUpdate(g_pViewer->getDeltaTime());
		}

	}
	
	return !s_bExitGame;
}

void RemoveResourceManager(void)
{	
	I3PRINTLOG(I3LOG_NOTICE, "RemoveResourceManager() Enter");	

	i3ResourceFile::RemoveResourceManager( i3Chara::static_meta());
	I3PRINTLOG(I3LOG_NOTICE, "RemoveResourceManager() i3Chara ");
	i3ResourceFile::RemoveResourceManager( i3AI::static_meta());
	I3PRINTLOG(I3LOG_NOTICE, "RemoveResourceManager() i3AI ");

	i3ResourceFile::RemoveResourceManager( i3SceneGraphInfo::static_meta());
	I3PRINTLOG(I3LOG_NOTICE, "RemoveResourceManager() i3SceneGraphInfo ");
	i3ResourceFile::RemoveResourceManager( i3GameResChara::static_meta());
	I3PRINTLOG(I3LOG_NOTICE, "RemoveResourceManager() i3GameResChara ");

	i3ResourceFile::RemoveResourceManager( i3Animation::static_meta());
	I3PRINTLOG(I3LOG_NOTICE, "RemoveResourceManager() i3Animation ");
	i3ResourceFile::RemoveResourceManager( i3Animation2::static_meta());
	I3PRINTLOG(I3LOG_NOTICE, "RemoveResourceManager() i3Animation2 ");
	
	i3ResourceFile::RemoveResourceManager( i3Sound::static_meta());
	I3PRINTLOG(I3LOG_NOTICE, "RemoveResourceManager() i3Sound ");
	i3ResourceFile::RemoveResourceManager( i3Texture::static_meta());

	I3PRINTLOG(I3LOG_NOTICE, "RemoveResourceManager() Leave");	
}

bool CloseGame(void)
{
	I3PRINTLOG(I3LOG_NOTICE, "CloseGame() Enter");

	//	I3_SAFE_RELEASE( g_pViewer );
	//  I3_SAFE_RELEASE( g_pFramework );
	//g_pViewer		= nullptr;
	//g_pFramework	= nullptr;
	WeaponLRU_Manager::i()->saveData();

	//I3_SAFE_RELEASE(g_pCommunity);
	I3_SAFE_RELEASE(g_pGameContext);
	I3_SAFE_RELEASE(g_pServerInfo);

	if (g_pViewer != nullptr)
	{
		if (g_pViewer->GetSoundContext())
			g_pViewer->GetSoundContext()->FlushSound(true);
	}

	DestroyGlobal();

	DestroyManagers();

	RemoveResourceManager();

	REAL_PROFILE_DESTROY();

	I3PRINTLOG(I3LOG_NOTICE, "CloseGame() Leave");

	return true;
}

void InitSound(void)
{
	I3PRINTLOG(I3LOG_NOTICE, "InitSound() Enter");

	i3SoundContext * pCtx = g_pViewer->GetSoundContext();
	if( pCtx != nullptr)
	{
		if( g_pEnvSet->m_bEnableBGM)
		{
			pCtx->SetTypeVolume( I3SND_PROP_TYPE_BGM,  (UINT8)g_pEnvSet->m_fVolBGM);
		}
		else
		{
			pCtx->SetTypeVolume( I3SND_PROP_TYPE_BGM,  (UINT8)0);
		}

		if( g_pEnvSet->m_bEnableEffect)
		{
			pCtx->SetTypeVolume( I3SND_PROP_TYPE_EFFECT, (UINT8)g_pEnvSet->m_fVolEffect);
			pCtx->SetTypeVolume( I3SND_PROP_TYPE_VOICE, (UINT8)g_pEnvSet->m_fVolEffect);
		}
		else
		{
			pCtx->SetTypeVolume( I3SND_PROP_TYPE_EFFECT, (UINT8)0);
			pCtx->SetTypeVolume( I3SND_PROP_TYPE_VOICE, (UINT8)0);
		}
	}

	I3PRINTLOG(I3LOG_NOTICE, "InitSound() Leave");
}

#ifdef ENABLE_TEXTURE_CHANGE_I3S

bool	LoadTextureChangei3s()
{
	i3SGTexChangeManager::i()->LoadRegistryFile("config/texture_change_Weapon.pef");
	i3SGTexChangeManager::i()->LoadRegistryFile("config/texture_change_Avatar.pef");

	return true;
}

#endif

bool	LoadDesignationValue()
{
	return DesigUtil::LoadDesignationPef("config/DesignationValue.pef");
}

bool	LoadMissionCard()
{
	return MCardUtil::LoadPef("config/Quest.pef", "Quest");
}

void FindDll()
{
	/*

	HANDLE hProcess = GetCurrentProcess();
	HMODULE hModules[300] = { 0 };
	HMODULE *pMod = hModules;
	MODULEINFO ModInfo;
	DWORD dwModuleSize = 0;
	TCHAR szName[MAX_PATH] = { 0 };

	dwModuleSize = 0;

	::EnumProcessModulesEx( hProcess, pMod, 1024, &dwModuleSize, NULL );

	// ModuleSize 읽지 못할경우 리턴
	if( !dwModuleSize )
		return ;

	dwModuleSize /= sizeof( HMODULE );

	GetModuleInformation(hProcess,hModules[0],&ModInfo,sizeof(ModInfo));
	for( DWORD dwModIndex = 1; dwModIndex < dwModuleSize; ++ dwModIndex )
	{
		//GetModuleBaseName( hProcess, hModules[dwModIndex], szName, MAX_PATH );
		GetModuleFileNameEx( hProcess, hModules[dwModIndex], szName, MAX_PATH );

		//I3TRACE("DLL : %s ",szName);
	}
	CloseHandle(hProcess);
	return ;
	*/
}



// revision 29591
//void LoadStageNameFromPef(void)
//{
//	i3RegKey * pRoot = Config::FindKey("Stage");
//	I3ASSERT( pRoot != nullptr);
//
//	char temp[256] = "";
//	char temp2[256] = "";
//
//	for( INT32 i=STAGE_ORD_NA+1; i<STAGE_ORD_MAX; i++)
//	{
//		getStageCodeName(temp2, i);
//
//		i3RegKey * pKey = Config::FindKey( temp2, pRoot);
//		if( pKey)
//		{
//			sprintf_s(temp, 256, "No Stage Name [ %s ]", temp2 );
//
//			FIND_REG_DATA(pKey, "_Name", temp, 256);	
//
//			//	번역되는 Stage 이름
//			setStageName( i, temp);
//		}
//	}
//}