#include "pch.h"

#include <sstream>
#include <cctype>

// Basement
#include "../../../Common/CommonSource/CommonDediCli.h"
#include "GameFramework.h"

#include "UI/UIFramework.h"
#include "AllStage.h"
#include "GlobalVariables.h"
#include "ServerIPList.h"
#include "EquipShapeManager.h"
#include "Game.h"
#include "TimeUtil.h"
#include "AnnounceConfig.h"
#include "Shop.h"
#include "SecurityHeader.h"
#include "ServerInfo.h"
#include "Lua\LuaManager.h"
#include "Lua\LuaScript.h"
#include "UI.h"
#include "IGShopContext.h"
#include "IGShopItemController.h"
#include "ObjExplosion.h"
#include "HackDefense.h"
#include "GameExitPopup.h"
#include "ui/Batmanager.h"
#include "ui/WinMagnet.h"
#include "UI/UIItemInfoUtil.h"
#include "UI/UIInvenInfoMgr.h"
#include "VideoPlay.h"
#include "UI/VideoUtil.h"
#include "BGLoadThread.h"
#include "BattleObjContext.h"
#include "ShutDownContext.h"
#include "Weapon/WeaponItemID.h"
#include "Weapon/WeaponUtil.h"
// Inbattle

#include "BattleServerContext.h"
#include "TwitchTVContext.h"

#include "UtilBattle.h"
#include "StageObject/ObjectRidingHelicopter.h"
#include "StageObject/ObjectRespawnHelicopter.h"
#include "ObjExplosion.h"
#include "BattleHud.h"
#include "Mode/Domination/SkillObjectModel.h"

#include "GameGUI.h"
#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "StageBattle/AIModeContext.h"
#include "GameMissionManager.h"
#include "i3Base/ColorString.h"
#include "GameMaster.h"

#include "ClientStageInfoUtil.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#ifdef BUG_TRAP
#include "BugTrap.h"
#endif

#if defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)
#include "UnitTest/GameUnitTest.h"
#endif

// string functions
#include "i3Base/string/compare/generic_is_equal.h"
#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/ext/mb_to_utf16.h"
#include "i3Base/string/ext/utf16_to_mb.h"

#include "i3Base/string/ext/contain_char.h"
#include "i3Base/string/ext/contain_string.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "i3Base/string/ext/num_to_str.h"

#include "i3Framework/OccDebugInfo.h"
#include "i3Framework/i3UI/i3UIFilePathMgr.h"

#include "UI/UIHUDLoadingBG.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"
#include "WeaponLRU_Manager.h"
#include "Cheatkey.h"

#if legacy_gui_function
	#include "GuiNotifyReceiver.h"
#endif

#if BUILD_RELEASE_QA_VERSION
#include "TestAutomation/TestAutomationInterface.h"
#endif

UINT32					g_GroupMask[32];

static INT32 s_nAddCharaDebugCount = 1;

static bool s_bNew_StageLoading = true;

//I3_CONCRETE_CLASS_INSTANCE( CGameFramework, i3Framework);
I3_CLASS_INSTANCE( CGameFramework);

#if defined(USE_EDITDLG)

#define UPDATE_DBG_MESSAGE (WM_USER + 3434)
//#include "../EditGameDlg/EditGameDialog.h"

#endif

#if defined( I3_DEBUG ) || defined( I3_RELEASE_PROFILE) //더미 캐릭터 이름
const wchar_t * s_szDebugCharaName[] =
{
	L"더미1",	L"더미2",	L"더미3",	L"더미4",	L"더미5",	L"더미6",	L"더미7",	L"더미8",
	L"더미9",	L"더미10",	L"더미11",	L"더미12",	L"더미13",	L"더미14",	L"더미15",	L"더미16"
};
#endif

namespace inner
{
	bool is_changed_text = false;
	i3::string user_info_text, sprite_count_text, hud_unit_test, polygon_count_text;
}

bool PreLoadFile(char *filePath, bool isCommon){	return WeaponLRU_Manager::i()->cbFindFilePath(filePath, isCommon); }


//
// i3UIManager Post Render
//

void Gameframework_OnI3UIPostRender(i3UIManager* uiMan, i3UIRenderer* renderer)
{
	if(!g_pFramework)
		return;

	if(g_pFramework->m_postDrawUIScenes.empty())
		return;

	renderer->DepthPlus();

	CGameFramework::UISceneList::iterator x = g_pFramework->m_postDrawUIScenes.begin();
	while(x != g_pFramework->m_postDrawUIScenes.end())
	{
		if(*x)
			(*x)->OnDraw(renderer);

		x++;
		
		renderer->DepthPlus();
	}
}


//
// CGameFramework
//

CGameFramework::CGameFramework(void) : m_EnableCapture(true), m_GameKeyMode(GAMEKEY_MODE_MENU), m_pCollisionCtx(nullptr), m_pBGLoadThread(nullptr)
	, m_pAttrSet(nullptr), m_pReflectMapAttr(nullptr), m_pRasterizeDownScale(nullptr), m_pRasterizeBrigthPath(nullptr)
	, m_pRasterizeGaussianBlur(nullptr), m_pRasterizeBloomCombine(nullptr), m_pRasterizeToneMapping(nullptr)
	, m_curVisionMaskingType(VE_NIGHT), m_fExitDeltaTime(0.0f), m_bExitGame(false), m_pChatBox(nullptr), m_pMessageBoxManager(nullptr)
	, m_pKeyboard(nullptr), m_pMouse(nullptr), m_pMessageBoxLayer(nullptr), m_pGUILayer(nullptr), m_pHUDLayer(nullptr), m_pWorldLayer(nullptr)
	, m_pGameSupportSet(nullptr), m_pHUDLoading(nullptr), m_bEnableScreenShot(nullptr), m_fScreenShotDeltaTime(0.f), m_nMouseButtonState(0)
	, m_nMouseButtonStrokeState(0), m_pEquipShapeManager(nullptr), m_pInvenList(nullptr), m_pIGShopContext(nullptr)
	, m_pClanContext(nullptr), m_pAnnounceConfig(nullptr)
	, m_pGameCommandSet(nullptr), m_pOutGameQACommandSet(nullptr), m_pNewUILayer(nullptr), m_prevUISpriteCount(-1)
	, m_timeNP(0.f), m_nNoticeListCount(0), m_nNoticeChatCount(0), m_bNoticeActivation(false)
	, m_bNotictCheckDone(false), m_pAutoMouseTimer(nullptr), m_AutoMouseCount(0), m_pMovie(nullptr), m_pUseMovie(true)
	, m_pUIFirstLoad(nullptr), m_pRecLiveIcon(nullptr), m_pVideoCapture(nullptr), m_pVideoPlay(nullptr), m_OldWndProc(nullptr), m_pLoadMove(nullptr), m_pLuaManager(nullptr)
	, m_pAttrSetRatingLogo(nullptr), m_pRatingLogo(nullptr)
	, m_pPostProcessNightVision(nullptr), m_pPostProcessMotionBlur(nullptr), m_pPostProcessHaze(nullptr),m_pPostProcessGamma(nullptr)
	, m_bChatLimitState(false)

#if !defined( I3_NO_PROFILE)
	, m_hWndEdigGameDlg(nullptr), m_hWndDbgMsgDlg(nullptr), m_hWndProfilingDlg(nullptr), m_hWndSoundEditDlg(nullptr), m_nLightChangeState(0)
#endif

#if defined( USE_HOST_MODULE)
	, m_pHostModule(nullptr)
#endif

#if defined( TEST_AUTOMATION)
	, m_pTestAuto(nullptr)
#endif

	, m_ChattingInputChecker(i3::make_pscoped<CChattingInputChecker>())
{
	i3_prof_degrade_factor(20.0);
	i3_prof_degrade_threshold(0.001);

	ResetInput();

	for( INT32 i=0; i < VE_MAX_COUNT; ++i)
	{
		//m_pVisionMaskingTex[i] = NULL;
		m_pVisionMixRate[i] = 0.0f;
	}

	i3Vector::Set( &m_vMouseMoveDelta, 0.0f, 0.0f, 0.0f);

	m_szLoadingText[0]	= NULL;

	i3mem::FillZero( m_AutoMouseTrack, sizeof(m_AutoMouseTrack));
	i3mem::FillZero( m_CmdString, sizeof(m_CmdString));

	ResetAutoMouse();

	for(INT32 i=0; i<4; i++)
	{
		m_pAttrSetLogo[i] = nullptr;
		m_pSpriteLogo[i] = nullptr;
		m_pTextureLogo[i] = nullptr;
		if(i<2)
		{
			m_pTexture[i] = nullptr;
		}
	}

#if (defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)) && !defined( SALES_BUILD)
	m_pTextVersionInfo = 0;
	m_pTextFPS = 0;
	m_bVersionInfo = true;
#endif

	m_bChatLimitState = false;
	m_ChatLimitSec.SetReverse(true);
	m_ChatLimitCB.SetObj(this);
	m_ChatLimitSec.SetCallback(&m_ChatLimitCB);
}

CGameFramework::~CGameFramework(void)
{
#if defined(_DEBUG)
	i3::print_class_meta_alloc_infos(16, 25, 8192);
#endif

	i3UIManager::SetPostDrawCallback(nullptr);

	if(m_pMessageBoxManager)
		m_pMessageBoxManager->CloseAllMessageBox();



	I3_SAFE_DELETE(m_pLuaManager);
	
	// StageUtils
	I3_SAFE_RELEASE(m_pAnnounceConfig);

	I3_SAFE_RELEASE(m_pInvenList);

	I3_SAFE_RELEASE(m_pIGShopContext);

	I3_SAFE_RELEASE(m_pClanContext);

	I3_SAFE_RELEASE(m_pEquipShapeManager);

	I3_SAFE_RELEASE( m_pCollisionCtx);

	I3_SAFE_RELEASE( m_pAttrSet);

	I3_SAFE_RELEASE( m_pRasterizeDownScale);

	I3_SAFE_RELEASE( m_pRasterizeBrigthPath);

	I3_SAFE_RELEASE( m_pRasterizeGaussianBlur);

	I3_SAFE_RELEASE( m_pRasterizeBloomCombine);

	I3_SAFE_RELEASE( m_pRasterizeToneMapping);

	I3_SAFE_RELEASE(m_pPostProcessNightVision);

	I3_SAFE_RELEASE(m_pPostProcessMotionBlur);

	I3_SAFE_RELEASE(m_pPostProcessGamma);

	I3_SAFE_RELEASE( m_pChatBox);

	I3_SAFE_RELEASE(m_pMessageBoxManager);

	I3_SAFE_RELEASE(m_pPostProcessHaze);

	#if !defined( I3_NO_PROFILE)
	if( ::IsWindow( m_hWndEdigGameDlg))
		::SendMessage( m_hWndEdigGameDlg, WM_CLOSE, 0,0);

	if( ::IsWindow( m_hWndDbgMsgDlg))
		::SendMessage( m_hWndDbgMsgDlg, WM_CLOSE, 0,0);

	if( ::IsWindow( m_hWndProfilingDlg))
		::SendMessage( m_hWndProfilingDlg, WM_CLOSE, 0,0);

	if( ::IsWindow( m_hWndSoundEditDlg))
		::SendMessage( m_hWndSoundEditDlg, WM_CLOSE, 0,0);
	#endif

	I3_SAFE_RELEASE( m_pGameSupportSet);

	if(m_pHUDLoading)
	{
		m_pHUDLoading->OnExitStart();
		m_pHUDLoading->Destroy();
	}


	I3_SAFE_RELEASE( m_pGameCommandSet);

	I3_SAFE_RELEASE( m_pOutGameQACommandSet);

	
	I3_SAFE_RELEASE( m_pAutoMouseTimer);

	for(INT32 i=1; i<4; i++)
	{
		I3_SAFE_RELEASE(m_pTextureLogo[i]);
	}

	for(INT32 i=0; i<2; i++)
	{
		I3_SAFE_RELEASE( m_pTexture[i]);
	}

	for(INT32 i=0; i<4; i++)
	{
		I3_SAFE_RELEASE(m_pAttrSetLogo[i]);
		I3_SAFE_RELEASE(m_pSpriteLogo[i]);
	}

	I3_SAFE_RELEASE( m_pMovie);

	I3_SAFE_RELEASE( m_pRatingLogo);

	I3_SAFE_RELEASE( m_pAttrSetRatingLogo);
	
	Config::UnInit();
	LocaleValue::Release();

#if defined( USE_HOST_MODULE)
	I3_SAFE_RELEASE( m_pHostModule);
#endif

	if( m_pUIFirstLoad)
	{
		m_pUIFirstLoad->SetEnable( false);
		I3_SAFE_RELEASE( m_pUIFirstLoad);
	}

	I3_SAFE_RELEASE(m_pRecLiveIcon);

	I3_SAFE_RELEASE(m_pVideoCapture);

	I3_SAFE_RELEASE(m_pVideoPlay);

	//IGShopItemController::GetInstance()->Destroy();

	I3_SAFE_RELEASE( m_pBGLoadThread);

#if (defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)) && !defined( SALES_BUILD)
	I3_SAFE_RELEASE(m_pTextVersionInfo);
	I3_SAFE_RELEASE(m_pTextFPS);
#endif

#if ENABLE_UNIT_TEST
	UnitTest::release_unit_test();
#endif

#if defined( TEST_AUTOMATION)
	CaseRun::Close();

	if (m_pTestAuto != nullptr)
	{
		delete m_pTestAuto;
		m_pTestAuto = nullptr;
	}
#endif

	PBConsoleManager::Release();

#ifdef _CONSOLELOG
	for (size_t i = 0; i < m_vecConsole.size(); i++)
	{
		I3_SAFE_DELETE(m_vecConsole[i]);
	}
#endif

	i3_prof_save("performance profile.txt");
}

void CGameFramework::ResetInput()
{
	m_nKeyPress		= 0;
	m_nOldKeyPress	= 0;

	m_nKeyStroke	= 0;
	m_nOldKeyStroke = 0;

	m_nSysKeyState = 0;
	m_nSysKeyPress = 0;

	m_nMouseButtonState	= 0;
	m_nMouseButtonStrokeState = 0;
	i3Vector::Zero( &m_vMouseMoveDelta);
}

bool CGameFramework::BeInput(bool bStrokeOnly)
{
	if( !i3Math::IsSame( m_pMouse->GetDeltaX(), 0.0f) ||
		!i3Math::IsSame( m_pMouse->GetDeltaY(), 0.0f) ||
		!i3Math::IsSame( m_pMouse->GetDeltaZ(), 0.0f) )
	{
		return true;
	}

	if (bStrokeOnly)
	{
		if( m_pMouse->GetStrokeButtonState() )
		{
			return true;
		}
	}
	else
	{
		if( m_pMouse->GetButtonState() )
		{
			return true;
		}
	}

	for(INT32 i = I3I_KEY_0; i < I3I_KEY_MAX; i++)
	{
		if (bStrokeOnly)
		{
			if( m_pKeyboard->GetStrokeState( I3I_KEY(i) ) )
			{
				return true;
			}
		}
		else
		{
			if( m_pKeyboard->GetPressState( I3I_KEY(i) ) )
			{
				return true;
			}
		}
	}

	return false;
}

void CGameFramework::InitCollisionContext()
{
	i3PhysixContext *		pCtx;

	pCtx = GetViewer()->GetPhysixContext();

	m_pCollisionCtx = i3CollisionContext::new_object();
	m_pCollisionCtx->Create( 800, 800);

	// CGRP_WORLD
	//World Collision Group (0)
	m_pCollisionCtx->AddGroup(	0,
								CGRP_MASK_WORLD,
								0);

	// CGRP_CHARA_LINE		// 사용 안함
	//모든 캐릭터 용 월드/오브젝트 컬리전용(1)
	m_pCollisionCtx->AddGroup(	I3_COLLISION_GRP_STYLE_WITH_WORLD,
								CGRP_MASK_CHARA_LINE,
								CGRP_MASK_WORLD | CGRP_MASK_OBJECT | CGRP_MASK_WORLD_HERO);
	


	// 네트워크 캐릭터 용 월드/오브젝트/ 컬리전용(2)
	m_pCollisionCtx->AddGroup(	I3_COLLISION_GRP_STYLE_WITH_WORLD,
								CGRP_MASK_CHARA_BOUND,
								CGRP_MASK_WORLD | CGRP_MASK_OBJECT | CGRP_MASK_WORLD_HERO |
								CGRP_MASK_TEAM_RED_ROUGH | CGRP_MASK_TEAM_BLUE_ROUGH);


	m_pCollisionCtx->AddGroup(	0,
								CGRP_MASK_WORLD_HERO,
								0);

	// CGRP_OBJECT_DYNAMIC
	// CGRP_OBJECT_WHOLE_DYNAMIC
	m_pCollisionCtx->AddGroup(	0,
								CGRP_MASK_OBJECT,
								0);

	// CGRP_ITEM
	
	m_pCollisionCtx->AddGroup(	0,
								CGRP_MASK_ITEM,
								0);

	// CGRP_CAMERA
	// Camera 전용 World Collision Group (6)
	m_pCollisionCtx->AddGroup(	I3_COLLISION_GRP_STYLE_WITH_WORLD,
								CGRP_MASK_CAMERA,
								CGRP_MASK_WORLD | CGRP_MASK_OBJECT);

	// CGRP_TEAM_BLUE
	// 팀플레이 전용 Collision Group(7 ~ 10)
	m_pCollisionCtx->AddGroup(	0,
								CGRP_MASK_TEAM_BLUE,
								CGRP_MASK_TEAM_RED | CGRP_MASK_WORLD | CGRP_MASK_OBJECT);

	// CGRP_TEAM_BLUE_ROUGH
	//pCtx->setGroupCollisionEnable(			CGRP_TEAM_BLUE_ROUGH,		CGRP_TEAM_RED_ROUGH);
	m_pCollisionCtx->AddGroup(	0,
								CGRP_MASK_TEAM_BLUE_ROUGH,
								CGRP_MASK_TEAM_RED_ROUGH);

	//pCtx->setGroupCollisionEnable(			 CGRP_TEAM_BLUE_ATTACK,		CGRP_TEAM_RED);
	m_pCollisionCtx->AddGroup( 0,
								CGRP_MASK_TEAM_BLUE_ATTACK,
								CGRP_MASK_TEAM_RED);

	m_pCollisionCtx->AddGroup(	0,
								CGRP_MASK_TEAM_RED,
								CGRP_MASK_TEAM_BLUE);

	// CGRP_TEAM_RED_ROUGH
	m_pCollisionCtx->AddGroup(	0,
								CGRP_MASK_TEAM_RED_ROUGH,
								CGRP_MASK_TEAM_BLUE_ROUGH);

	//pCtx->setGroupCollisionEnable(			CGRP_TEAM_RED_ATTACK,		CGRP_TEAM_BLUE);
	m_pCollisionCtx->AddGroup( 0,
								CGRP_MASK_TEAM_RED_ATTACK,
								CGRP_MASK_TEAM_BLUE);

	//

	// CGRP_WORLD_ATTACK
	// Hit Collision전용 WorldCollision Group(11)
	m_pCollisionCtx->AddGroup(	0,
								CGRP_MASK_WORLD_ATTACK,
								0);

	m_pCollisionCtx->AddGroup(	I3_COLLISION_GRP_STYLE_WITH_WORLD,
								CGRP_MASK_ATTACK,
								CGRP_MASK_WORLD_ATTACK | CGRP_MASK_OBJECT);

	// CGRP_LIGHTVOLUME
	// Light용 Collision Group( 13)
	m_pCollisionCtx->AddGroup( 0,
							   CGRP_MASK_LIGHTVOLUME,
							   0);

	// CGRP_CHARA_LIGHTVOLUME
	m_pCollisionCtx->AddGroup( I3_COLLISION_GRP_STYLE_WITH_WORLD,
								0,
								CGRP_MASK_LIGHTVOLUME);

	m_pCollisionCtx->AddGroup( I3_COLLISION_GRP_STYLE_WITH_WORLD,
								CGRP_MASK_GRENADE,
								CGRP_MASK_WORLD_ATTACK | CGRP_MASK_OBJECT | CGRP_MASK_TEAM_RED_ROUGH | CGRP_MASK_TEAM_BLUE_ROUGH);

	// 데칼에 사용할 컬리젼 그룹을 알려줍니다.
	pCtx->m_nWorldHitCollisionGroup	= CGRP_WORLD_ATTACK;
	pCtx->m_nObjectHitCollisionGroup = CGRP_OBJECT_HIT;
	pCtx->m_nDecalCollisionGroup	= CGRP_DECAL;

	if( m_pViewer != nullptr)
	{
		m_pViewer->GetSceneTracer()->SetCollisionContext( m_pCollisionCtx);
	}

	InitializePhysXCollisionGroup();
}

void CGameFramework::InitializePhysXCollisionGroup( bool bNewCollisionMode)
{
	i3PhysixContext *		pCtx = GetViewer()->GetPhysixContext();

	//////////////////////////////////////////////////////////////////////////////////////////
	// Raycast용 그룹 마스크를 세팅
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_ATTACK,			CGRP_WORLD_ATTACK);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_ATTACK,			CGRP_OBJECT_HIT);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_ATTACK,			CGRP_OBJECT_WHOLE);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_ATTACK,			CGRP_OBJECT_WHOLE_DYNAMIC);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_ATTACK,			CGRP_ITEM_HIT);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_ATTACK,			CGRP_OBJECT_ALL);

	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_CHARA_RED,			CGRP_TEAM_RED);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_CHARA_BLUE,		CGRP_TEAM_BLUE);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_CHARA_ROUGH_RED,	CGRP_TEAM_RED_ROUGH);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_CHARA_ROUGH_BLUE,	CGRP_TEAM_BLUE_ROUGH);

	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_OBJECT,			CGRP_OBJECT_HIT);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_OBJECT,			CGRP_OBJECT_WHOLE);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_OBJECT,			CGRP_OBJECT_WHOLE_DYNAMIC);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_OBJECT,			CGRP_OBJECT_ALL);

	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_GRENADE,			CGRP_TEAM_RED_ROUGH);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_GRENADE,			CGRP_TEAM_BLUE_ROUGH);

	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_ONLYWORLDHIT,		CGRP_WORLD_ATTACK);

	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_ENVIRONMENT,		CGRP_WORLD_ATTACK);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_ENVIRONMENT,		CGRP_OBJECT_HIT);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_ENVIRONMENT,		CGRP_OBJECT_WHOLE);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_ENVIRONMENT,		CGRP_OBJECT_WHOLE_DYNAMIC);
	pCtx->setGroupMaskEnable( CGRP_MASKGROUP_ENVIRONMENT,		CGRP_OBJECT_ALL);

	for(INT32 i=0; i<32; i++) {
		g_GroupMask[i] = pCtx->getGroupMask(i);
	}

	// CGRP_CHARA_BOUND
	pCtx->setGroupCollisionEnable(		CGRP_CHARA_BOUND,		CGRP_OBJECT_ALL_NOHIT);
	pCtx->setGroupCollisionEnable(		CGRP_CHARA_BOUND,		CGRP_OBJECT_ALL);

	// CGRP_TEAM_RED
	if( bNewCollisionMode)
	{
		// World Hit Collision도 적용되도록...
		pCtx->setGroupCollisionEnable(			CGRP_TEAM_RED,		CGRP_WORLD_ATTACK);
	}

	pCtx->setGroupCollisionEnable(			CGRP_TEAM_RED,		CGRP_WORLD);
	pCtx->setGroupCollisionEnable(			CGRP_TEAM_RED,		CGRP_TEAM_RED);
	pCtx->setGroupCollisionEnable(			CGRP_TEAM_RED,		CGRP_OBJECT);
	pCtx->setGroupCollisionEnable(			CGRP_TEAM_RED,		CGRP_OBJECT_WHOLE);
	pCtx->setGroupCollisionEnable(			CGRP_TEAM_RED,		CGRP_OBJECT_WHOLE_DYNAMIC);
	pCtx->setGroupCollisionEnable(			CGRP_TEAM_RED,		CGRP_OBJECT_ALL_NOHIT);
	pCtx->setGroupCollisionEnable(			CGRP_TEAM_RED,		CGRP_OBJECT_ALL);

	if( bNewCollisionMode)
	{
		// World Hit Collision도 적용되도록...
		pCtx->setGroupCollisionEnable(			CGRP_TEAM_BLUE,		CGRP_WORLD_ATTACK);
	}

	pCtx->setGroupCollisionEnable(			CGRP_TEAM_BLUE,		CGRP_WORLD);
	pCtx->setGroupCollisionEnable(			CGRP_TEAM_BLUE,		CGRP_TEAM_BLUE);
	pCtx->setGroupCollisionEnable(			CGRP_TEAM_BLUE,		CGRP_OBJECT);
	pCtx->setGroupCollisionEnable(			CGRP_TEAM_BLUE,		CGRP_OBJECT_WHOLE);
	pCtx->setGroupCollisionEnable(			CGRP_TEAM_BLUE,		CGRP_OBJECT_WHOLE_DYNAMIC);
	pCtx->setGroupCollisionEnable(			CGRP_TEAM_BLUE,		CGRP_OBJECT_ALL_NOHIT);
	pCtx->setGroupCollisionEnable(			CGRP_TEAM_BLUE,		CGRP_OBJECT_ALL);

	if( bNewCollisionMode)
	{
		// World Hit Collision도 적용되도록...
		pCtx->setGroupCollisionEnable(			CGRP_ITEM_WHOLE,		CGRP_WORLD_ATTACK);
	}
	pCtx->setGroupCollisionEnable(			CGRP_ITEM_WHOLE,			CGRP_WORLD );
	pCtx->setGroupCollisionEnable(			CGRP_ITEM_WHOLE,			CGRP_OBJECT);
	pCtx->setGroupCollisionEnable(			CGRP_ITEM_WHOLE,			CGRP_OBJECT_DYNAMIC);
	pCtx->setGroupCollisionEnable(			CGRP_ITEM_WHOLE,			CGRP_OBJECT_WHOLE);
	pCtx->setGroupCollisionEnable(			CGRP_ITEM_WHOLE,			CGRP_OBJECT_WHOLE_DYNAMIC);
	pCtx->setGroupCollisionEnable(			CGRP_ITEM_WHOLE,			CGRP_OBJECT_ALL);
	pCtx->setGroupCollisionEnable(			CGRP_ITEM_WHOLE,			CGRP_OBJECT_ALL_NOHIT);

	// Item용 Collision Group (5)		// 무기 & 장비
	if( bNewCollisionMode)
	{
		// World Hit Collision도 적용되도록...
		pCtx->setGroupCollisionEnable(			CGRP_ITEM,			CGRP_WORLD_ATTACK);
	}
	pCtx->setGroupCollisionEnable(			CGRP_ITEM,			CGRP_WORLD );
	pCtx->setGroupCollisionEnable(			CGRP_ITEM,			CGRP_OBJECT);
	pCtx->setGroupCollisionEnable(			CGRP_ITEM,			CGRP_OBJECT_DYNAMIC);
	pCtx->setGroupCollisionEnable(			CGRP_ITEM,			CGRP_OBJECT_WHOLE);
	pCtx->setGroupCollisionEnable(			CGRP_ITEM,			CGRP_OBJECT_WHOLE_DYNAMIC);
	pCtx->setGroupCollisionEnable(			CGRP_ITEM,			CGRP_OBJECT_ALL);
	pCtx->setGroupCollisionEnable(			CGRP_ITEM,			CGRP_OBJECT_ALL_NOHIT);
	pCtx->setGroupCollisionEnable(			CGRP_ITEM,			CGRP_TRIGGER_ALL);

	// CGRP_OBJECT_WHOLE_DYNAMIC
	// hit, 월드와 반응 (2)
	if( bNewCollisionMode)
	{
		// World Hit Collision도 적용되도록...
		pCtx->setGroupCollisionEnable(		CGRP_MYCHARA_BOUND,		CGRP_WORLD_ATTACK);
	}
	pCtx->setGroupCollisionEnable(		CGRP_MYCHARA_BOUND,		CGRP_WORLD );
	pCtx->setGroupCollisionEnable(		CGRP_MYCHARA_BOUND,		CGRP_CHARA_BOUND );
	pCtx->setGroupCollisionEnable(		CGRP_MYCHARA_BOUND,		CGRP_OBJECT );
	pCtx->setGroupCollisionEnable(		CGRP_MYCHARA_BOUND,		CGRP_OBJECT_WHOLE );
	pCtx->setGroupCollisionEnable(		CGRP_MYCHARA_BOUND,		CGRP_TRIGGER);
	pCtx->setGroupCollisionEnable(		CGRP_MYCHARA_BOUND,		CGRP_OBJECT_ALL_NOHIT);
	pCtx->setGroupCollisionEnable(		CGRP_MYCHARA_BOUND,		CGRP_OBJECT_ALL);
	pCtx->setGroupCollisionEnable(		CGRP_MYCHARA_BOUND,		CGRP_ITEM_WHOLE);

	// Object용 Collision Group(4)
	// Static Object, Kinematic Object는 월드와 컬리젼 하지 않습니다.
	if( bNewCollisionMode)
	{
		// World Hit Collision도 적용되도록...
		pCtx->setGroupCollisionEnable(		CGRP_OBJECT_DYNAMIC,	CGRP_WORLD_ATTACK);
	}
	pCtx->setGroupCollisionEnable(		CGRP_OBJECT_DYNAMIC,	CGRP_WORLD );
	pCtx->setGroupCollisionEnable(		CGRP_OBJECT_DYNAMIC,	CGRP_OBJECT );
	pCtx->setGroupCollisionEnable(		CGRP_OBJECT_DYNAMIC,	CGRP_OBJECT_DYNAMIC );
	pCtx->setGroupCollisionEnable(		CGRP_OBJECT_DYNAMIC,	CGRP_OBJECT_WHOLE );
	pCtx->setGroupCollisionEnable(		CGRP_OBJECT_DYNAMIC,	CGRP_OBJECT_WHOLE_DYNAMIC );

	// Hit를 포함한 시뮬레이션 컬리젼
	if( bNewCollisionMode)
	{
		// World Hit Collision도 적용되도록...
		pCtx->setGroupCollisionEnable(		CGRP_OBJECT_WHOLE_DYNAMIC,	CGRP_WORLD_ATTACK);
	}
	pCtx->setGroupCollisionEnable(		CGRP_OBJECT_WHOLE_DYNAMIC,	CGRP_WORLD );
	pCtx->setGroupCollisionEnable(		CGRP_OBJECT_WHOLE_DYNAMIC,	CGRP_OBJECT );
	pCtx->setGroupCollisionEnable(		CGRP_OBJECT_WHOLE_DYNAMIC,	CGRP_OBJECT_WHOLE );
	pCtx->setGroupCollisionEnable(		CGRP_OBJECT_WHOLE_DYNAMIC,	CGRP_OBJECT_WHOLE_DYNAMIC );

	//Mission Obj
	//CGRP_MISSIONOBJ				18
	// CGRP_CHARA_BOUND
	//모든 케릭터 용 월드/오브젝트/ 컬리전용( 자동테스트 전용)(2)
	pCtx->setGroupCollisionEnable( CGRP_DECAL,				CGRP_WORLD_ATTACK );
	pCtx->setGroupCollisionEnable( CGRP_DECAL,				CGRP_OBJECT_HIT );
	pCtx->setGroupCollisionEnable( CGRP_DECAL,				CGRP_OBJECT_WHOLE );
	pCtx->setGroupCollisionEnable( CGRP_DECAL,				CGRP_OBJECT_WHOLE_DYNAMIC );
	pCtx->setGroupCollisionEnable( CGRP_DECAL,				CGRP_OBJECT_ALL);

	pCtx->setGroupCollisionEnable(			CGRP_OBJECT_ALL_NOHIT,		CGRP_WORLD);
	pCtx->setGroupCollisionEnable(			CGRP_OBJECT_ALL_NOHIT,		CGRP_OBJECT);
	pCtx->setGroupCollisionEnable(			CGRP_OBJECT_ALL_NOHIT,		CGRP_OBJECT_DYNAMIC);
	pCtx->setGroupCollisionEnable(			CGRP_OBJECT_ALL_NOHIT,		CGRP_OBJECT_WHOLE);
	pCtx->setGroupCollisionEnable(			CGRP_OBJECT_ALL_NOHIT,		CGRP_OBJECT_WHOLE_DYNAMIC);
	pCtx->setGroupCollisionEnable(			CGRP_OBJECT_ALL_NOHIT,		CGRP_OBJECT_ALL);
	pCtx->setGroupCollisionEnable(			CGRP_OBJECT_ALL_NOHIT,		CGRP_OBJECT_ALL_NOHIT);

	pCtx->setGroupCollisionEnable(			CGRP_OBJECT_ALL,			CGRP_WORLD);
	pCtx->setGroupCollisionEnable(			CGRP_OBJECT_ALL,			CGRP_OBJECT);
	pCtx->setGroupCollisionEnable(			CGRP_OBJECT_ALL,			CGRP_OBJECT_DYNAMIC);
	pCtx->setGroupCollisionEnable(			CGRP_OBJECT_ALL,			CGRP_OBJECT_WHOLE);
	pCtx->setGroupCollisionEnable(			CGRP_OBJECT_ALL,			CGRP_OBJECT_WHOLE_DYNAMIC);
	pCtx->setGroupCollisionEnable(			CGRP_OBJECT_ALL,			CGRP_OBJECT_ALL);

	pCtx->setGroupCollisionEnable(			CGRP_TRIGGER_ALL,			CGRP_OBJECT_WHOLE_DYNAMIC);
	pCtx->setGroupCollisionEnable(			CGRP_TRIGGER_ALL,			CGRP_OBJECT_DYNAMIC );
	pCtx->setGroupCollisionEnable(			CGRP_TRIGGER_ALL,			CGRP_OBJECT);
	pCtx->setGroupCollisionEnable(			CGRP_TRIGGER_ALL,			CGRP_OBJECT_WHOLE);
	pCtx->setGroupCollisionEnable(			CGRP_TRIGGER_ALL,			CGRP_TEAM_BLUE_ROUGH);
	pCtx->setGroupCollisionEnable(			CGRP_TRIGGER_ALL,			CGRP_TEAM_RED_ROUGH);

	pCtx->setGroupCollisionEnable(			CGRP_TREX_BOUND,			CGRP_WORLD );
	pCtx->setGroupCollisionEnable(			CGRP_TREX_BOUND,			CGRP_TREX_BOUND );
	pCtx->setGroupCollisionEnable(			CGRP_TREX_BOUND,			CGRP_OBJECT );
	pCtx->setGroupCollisionEnable(			CGRP_TREX_BOUND,			CGRP_OBJECT_WHOLE );
	pCtx->setGroupCollisionEnable(			CGRP_TREX_BOUND,			CGRP_TRIGGER);
	pCtx->setGroupCollisionEnable(			CGRP_TREX_BOUND,			CGRP_OBJECT_ALL_NOHIT);
	pCtx->setGroupCollisionEnable(			CGRP_TREX_BOUND,			CGRP_OBJECT_ALL);

	// Light Volume과 충돌 확인하기 위한 Group
	pCtx->setGroupCollisionEnable(	CGRP_CHARA_LIGHTVOL,		CGRP_LIGHTVOLUME);

	pCtx->setActorGroupPairFlags( I3SG_DECAL_ACTOR_GROUP, CGRP_OBJECT_WHOLE_DYNAMIC, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH);
	pCtx->setActorGroupPairFlags( I3SG_DECAL_ACTOR_GROUP, CGRP_OBJECT_ALL, NX_NOTIFY_ON_START_TOUCH | NX_NOTIFY_ON_TOUCH);
}

void CGameFramework::InitSceneGraph()
{
	i3Framework::InitSceneGraph();
}

void CGameFramework::_InitializePostProcess()
{

	//	Post Process - Motion Blur
	{
		m_pPostProcessMotionBlur = i3PostProcessMotionBlur::new_object();
		m_pPostProcessMotionBlur->SetEnable(false);
	}

	// Post Process - Night Vision
	_InitializeNightVision();

	m_pPostProcessHaze = i3PostProcessHaze::new_object();
	m_pPostProcessHaze->SetEnable(false);

	// 다른 픽셀 UV 건드리는 포스트 프로세스와 충돌되는 버그 수정때까지 임시로 끕니다.
#ifdef USE_GAMMAOPTION_BY_SHADER
	// Post Process - Gamma Option (above UI).
	if (nullptr == m_pPostProcessGamma)
	{
		m_pPostProcessGamma = i3PostProcessGamma::new_object();
		m_pPostProcessGamma->SetEnable(true);	// always available shader.

		g_pViewer->GetPostProcessManager()->AddLastPostProcess(m_pPostProcessGamma);
	}
#endif
}

void CGameFramework::AddInGamePostProcessToManager(i3PostProcessManager * pManager)
{
	pManager->AddPostProcess(m_pPostProcessMotionBlur);
	pManager->AddPostProcess(m_pPostProcessNightVision);
	pManager->AddPostProcess(m_pPostProcessHaze);
}


void CGameFramework::_InitializeNightVision(void)
{
	const char * s_VisionMaskingTexName[VE_MAX_COUNT] =
	{
		"Gui/Hud/NightVisionTex.i3i",
		"Gui/Hud/NVTex_Dinoeye.i3i"
	};

	i3RegKey * pConfigKey = Config::_GetRegRoot("Basic/Config");
	if( pConfigKey != nullptr)
	{
		i3RegKey * pnvMixValueKey = Config::FindKey( "nvMixValue", pConfigKey);
		if( pnvMixValueKey != nullptr)
		{
			FIND_REG_DATA( pnvMixValueKey, LS_STR("NightVision"), &m_pVisionMixRate[VE_NIGHT]);
			FIND_REG_DATA( pnvMixValueKey, LS_STR("DinoEye"), &m_pVisionMixRate[VE_DINO_EYE]);
		}
	}

	I3ASSERT( m_pPostProcessNightVision == nullptr);

	m_pPostProcessNightVision = i3PostProcessNightVision::new_object();

	for( UINT32 i = 0; i < VE_MAX_COUNT; i++)
		m_pPostProcessNightVision->AddMaskingTexture( s_VisionMaskingTexName[ i]);

	//기본 모드와 같도록 맨처음엔 강제설정
	m_pPostProcessNightVision->SetMaskingType( VE_NIGHT);
	m_pPostProcessNightVision->SetMixRate( m_pVisionMixRate[VE_NIGHT]);

	m_pPostProcessNightVision->SetEnable( false);
}

void CGameFramework::SetVisionMaskingType( VISION_MASK_TYPE type)
{
	if( type == m_curVisionMaskingType)
		return;

	if( m_pPostProcessNightVision == nullptr)
		return;

	if( type < 0 || type >= VE_MAX_COUNT)
		return;

	m_pPostProcessNightVision->SetMaskingType( type);
	m_pPostProcessNightVision->SetMixRate( m_pVisionMixRate[type]);

	m_curVisionMaskingType = type;
}

void CGameFramework::BuildSceneStruct(void)
{
	INT32 i;
	i3SgLayer * pLayer;

	m_pAttrSet = i3AttrSet::new_object();
#if defined( I3_DEBUG)
	m_pAttrSet->SetName( "GameFramework_m_pAttrSet");
#endif

	{
		i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

		pAttr->Set( true);
		m_pAttrSet->AddAttr( pAttr);
	}

	{
		i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();
		m_pAttrSet->AddAttr( pAttr);

		pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);
		pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT);
	}

	{
		i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();
		m_pAttrSet->AddAttr( pAttr);

		pAttr->Set( true);
	}

	// ReflectMap
	{
		m_pReflectMapAttr = i3ReflectMapBindAttr::new_object_ref();
		m_pAttrSet->AddAttr( m_pReflectMapAttr);
	}

	Get3DRoot()->AddChild( m_pAttrSet);
	
	for( i = 0; i < getLayerCount(); i++)
	{
		pLayer = getLayer( i);

		if( pLayer->getStyle() & I3_SG_LAYER_STYLE_2D)
		{
			Get2DRoot()->AddChild( pLayer->getLayerRoot());

			// 2D Layer 링크
			if( i3::generic_is_iequal( pLayer->GetName(), "MessageRoot") )
			{
				m_pMessageBoxLayer = pLayer;
			}
			else if( i3::generic_is_iequal( pLayer->GetName(), "GuiRoot") )
			{
				m_pGUILayer = pLayer;

				// Lighting을 Gui Layer에 들어올때 Reset합니다.
				m_pGUILayer->getLayerRoot()->SetResetLighting( true);
			}
			else if( i3::generic_is_iequal( pLayer->GetName(), "UIRoot") )
			{
				m_pNewUILayer = pLayer;

				if( getUIRenderer() != nullptr && ( getUIRenderer()->GetRoot() != nullptr))
				{
					getUIRenderer()->GetRoot()->ChangeParent( m_pNewUILayer->getLayerRoot());
				}

				m_pNewUILayer->getLayerRoot()->SetResetLighting( true);
			}
			else if( i3::generic_is_iequal( pLayer->GetName(), "HudRoot") )
			{
				m_pHUDLayer = pLayer;
			}
		}
		else
		{
			{
				if( i3::generic_is_iequal( pLayer->GetName(), "WorldRoot") )
				{
					m_pWorldLayer = pLayer;
				}

				m_pAttrSet->AddChild( pLayer->getLayerRoot());
			}
		}
	}
}

LUASCRIPT* CGameFramework::CreateLuaScript(char* szName)
{
	LUASCRIPT*  scriptTemp = nullptr;

	if( m_pLuaManager != nullptr)
	{
		scriptTemp = m_pLuaManager->CreateScript();
		scriptTemp->RunFile(szName);
	}

	return scriptTemp;
}

void CGameFramework::DeleteLuaScript(LUASCRIPT* pLuaScript)
{
	if( m_pLuaManager != nullptr)
	{
		m_pLuaManager->UnlinkScript(pLuaScript);
		I3_SAFE_DELETE(pLuaScript);
	}
}

void CGameFramework::OnCreate()
{
	i3UIManager::SetPostDrawCallback(&Gameframework_OnI3UIPostRender);

	i3ResourceFile::setSearchExternRefEnable(true);
	I3PRINTLOG(I3LOG_NOTICE, "* 1");

#if defined(USE_LOCALE_UI)
	//다국어 지원. 프레임워크 생성되지마자 기본 로케일 설정해준다.
	//세팅하기 전에 현재 선택한 다국어가 있는지 확인한 후, 없다면 영어로 기본 설정한다.
	SetMainLocale();
#endif

	if (LocaleValue::Enable("EnableRatingLogo"))
	{
		SetStateRatingLogo();
		SetState(STATE_FADEIN_START);
		m_StateAfterFadeIn = STATE_LOGO4_START;
	}
	else
		SetState(STATE_COMMON_LOAD_START);

	if (LocaleValue::Enable("EnableShutDown"))
	{
		ShutDownContext::i()->SetEnableShutDown(true);
	}

	m_pLuaManager = new LUAMANAGER;
	MEMDUMP_NEW(m_pLuaManager, sizeof(LUAMANAGER));

	m_OldWndProc = (WNDPROC)SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG)(LONG_PTR)StaticWndProc);
	if (!m_OldWndProc)
	{
		I3PRINTLOG(I3LOG_NOTICE, "Procedure subclassing fail.");
		return;
	}

	I3PRINTLOG(I3LOG_NOTICE, "* 1-1");
	i3Framework::OnCreate();

	GetViewer()->AddState(I3_VIEWER_STATE_SYNC_FPS);

#if (defined( I3_DEBUG ) || defined( I3_RELEASE_PROFILE))//디버그 텍스트
	GetViewer()->setDebugTextPos(10, 220);
#endif

	OccDebugInfo* pInfo = GetViewer()->getOccDebugInfo();
	if (pInfo)
		pInfo->setDebugTextPos(10, 220);
	
	I3PRINTLOG(I3LOG_NOTICE, "* 2");
	REAL_PROFILE_SET_REPORT_CB(UpdateProfiling);

	m_pKeyboard = m_pViewer->GetInputDeviceManager()->GetKeyboard();
	m_pMouse = m_pViewer->GetInputDeviceManager()->GetMouse();

	I3PRINTLOG(I3LOG_NOTICE, "* 3");
	ResetInput();

	I3PRINTLOG(I3LOG_NOTICE, "* 4");
	// 게임에서 사용되는 폰트를 생성합니다.
	//CreateGameFont();
	GameGUI::InitColor();

	I3PRINTLOG(I3LOG_NOTICE, "* 5");

	if (s_bNew_StageLoading == true)
	{
		// 게임에 사용되는 채팅 박스를 생성합니다.
		m_pHUDLoading = UIHUDLoadingBG::new_object();
		m_pHUDLoading->OnCreate(this);
		m_pHUDLoading->SetEnable(false);			// 초기에 Chat Box는 비활성화 상태입니다.
	}


	// 게임에 사용되는 채팅 박스를 생성합니다.
	m_pChatBox = CGameChatBox::new_object();
	m_pChatBox->SetEnable(false);			// 초기에 Chat Box는 비활성화 상태입니다.
	m_pChatBox->setWhisperEnable(false);
	I3_GAMENODE_ADDCHILD(this, m_pChatBox);

	I3PRINTLOG(I3LOG_NOTICE, "* 6");

	// 게임에서 사용되는 메시지 박스를 생성합니다.
	{
		m_pMessageBoxManager = CMessageBoxManager::new_object();
		m_pMessageBoxManager->Create(g_pFramework->getMessagBoxLayer());
		I3_GAMENODE_ADDCHILD(this, m_pMessageBoxManager);
	}
	I3PRINTLOG(I3LOG_NOTICE, "* 7");

	// 게임에서 사용되는 보조적인 도움 모음입니다.
	m_pGameSupportSet = CGameSupportSet::new_object();

	// 게임에서 사용되는 Command Set입니다.
#if defined( ENABLE_TEST_COMMAND)
	m_pGameCommandSet = CGameCommandSet::new_object();
#endif
	m_pOutGameQACommandSet = COutGameQACommandSet::new_object();

#if ENABLE_UNIT_TEST
	UnitTest::create_unit_test(); //command line 값이 없으면 생성 안 된다.
#endif

	I3PRINTLOG(I3LOG_NOTICE, "* 8");

	//
	InitCollisionContext();

	I3PRINTLOG(I3LOG_NOTICE, "* 9");

	{
		i3AttrSet *	pEffectRoot = GetLayerRoot("EffectRoot");
		I3ASSERT(pEffectRoot != nullptr);

		{
			i3MaterialAttr * pAttr = i3MaterialAttr::new_object_ref();

			pAttr->SetDiffuse(0.8f, 0.8f, 0.8f, 1.0f);
			pAttr->SetSpecular(0.2f, 0.2f, 0.2f, 1.0f);
			pAttr->SetShininess(128.0f);

			pEffectRoot->AddAttr(pAttr);
		}

		{
			i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();

			pAttr->SetMinFilter(I3G_TEXTURE_FILTER_LINEAR_MIPMAP_POINT);
			pAttr->SetMagFilter(I3G_TEXTURE_FILTER_LINEAR);

			pEffectRoot->AddAttr(pAttr);
		}

		// Blend Enable = TRUE;
		{
			i3BlendEnableAttr * pAttr = i3BlendEnableAttr::new_object_ref();

			pAttr->Set(true);
			pEffectRoot->AddAttr(pAttr);
		}

		// Blend Mode( I3G_BLEND_SRCCOLOR, I3G_BLEND_INVSRCCOLOR)
		{
			i3BlendModeAttr * pAttr = i3BlendModeAttr::new_object_ref();

			pAttr->SetSource(I3G_BLEND_SRCALPHA);
			pAttr->SetDestination(I3G_BLEND_ONE);

			pEffectRoot->AddAttr(pAttr);
		}

		//Lighting Enable = FALSE;
		{
			i3LightingEnableAttr * pAttr = i3LightingEnableAttr::new_object_ref();

			pAttr->Set(false);
			pEffectRoot->AddAttr(pAttr);
		}

		// Texture Enable
		{
			i3TextureEnableAttr * pAttr = i3TextureEnableAttr::new_object_ref();
			pAttr->Set(true);
			pEffectRoot->AddAttr(pAttr);
		}


		//Fog Enable Attr
		{
			i3FogEnableAttr * pAttr = i3FogEnableAttr::new_object_ref();

			pAttr->Set(false);
			pEffectRoot->AddAttr(pAttr);
		}

		//Alpha Test
		{
			i3AlphaTestEnableAttr * pAttr = i3AlphaTestEnableAttr::new_object_ref();

			pAttr->Set(true);
			pEffectRoot->AddAttr(pAttr);
		}

		// ZWrite Enable
		{
			i3ZWriteEnableAttr * pAttr = i3ZWriteEnableAttr::new_object_ref();

			pAttr->Set(false);
			pEffectRoot->AddAttr(pAttr);
		}

		// Normal Map
		{
			i3NormalMapEnableAttr * pAttr = i3NormalMapEnableAttr::new_object_ref();
			pAttr->Set(false);
			pEffectRoot->AddAttr(pAttr);
		}


		// Normal Bind
		{
			i3NormalMapBindAttr * pAttr = i3NormalMapBindAttr::new_object_ref();
			pEffectRoot->AddAttr(pAttr);
		}


		// ZBias
		{
			i3ZBiasAttr * pAttr = i3ZBiasAttr::new_object_ref();
			pEffectRoot->AddAttr(pAttr);
		}
	}


	I3PRINTLOG(I3LOG_NOTICE, "* 10");

	// StageUtils
	{
		m_pEquipShapeManager = CEquipShapeManager::new_object();

		i3RegKey* pStageListKey = Config::_GetRegRoot("Stage/Stage");
		m_pEquipShapeManager->_InitStageImageIndex(pStageListKey);

		i3RegKey* pWeaponListKey = Config::_GetRegRoot("Weapon/WeaponList");
		m_pEquipShapeManager->_InitWeaponImageIndex(pWeaponListKey);

#if !defined(USE_LOCALE_UI)
		m_pEquipShapeManager->CreateGlobalImageRes();
#endif

		m_pInvenList = CInvenList::new_object();
		m_pIGShopContext = IGShopContext::new_object();
		m_pClanContext = CClanContext::new_object();
		m_pClanContext->Create();

		m_pAnnounceConfig = CAnnounceConfig::new_object();
		I3ASSERT(m_pAnnounceConfig != nullptr);

		if (!m_pAnnounceConfig->Create())
		{
			I3PRINTLOG(I3LOG_NOTICE, "Fail load file - announce.xml");
		}
	}
	I3PRINTLOG(I3LOG_NOTICE, "* 11");

	_InitializePostProcess();

	I3PRINTLOG(I3LOG_NOTICE, "* 12");

	// Loading Screen용 GUI
	{
		// 여기서 UI2_Font를 설정합니다.
		if (getUIRenderer() != nullptr)
		{
			i3::rc_wstring wstr_font = LocaleValue::GetString("_Font");
			i3::string font;	i3::utf16_to_mb(wstr_font, font);
			getUIRenderer()->getTextManager()->SetFontName(font.c_str());
			I3ASSERT(i3Language::GetCharasetFromCodePage(GetCodePage()) < 256);
			getUIRenderer()->getTextManager()->SetFontCharaSet((UINT8)i3Language::GetCharasetFromCodePage(GetCodePage()));

#if defined(LOCALE_TURKEY) || defined( LOCALE_MIDDLE_EAST )
			getUIRenderer()->getTextManager()->SetExternalFontSize(-2);
#elif(LOCALE_NORTHAMERICA)
			getUIRenderer()->getTextManager()->SetExternalFontSize(-2);
#elif(LOCALE_TAIWAN)
			getUIRenderer()->getTextManager()->SetExternalFontSize(0);
			//#elif(LOCALE_SINGMAL)
			//			getUIRenderer()->getTextManager()->SetExternalFontSize(-3);
#elif(LOCALE_VIETNAM)
			getUIRenderer()->getTextManager()->SetExternalFontSize(-2);
#else
			INT32 fontExtend = LocaleValue::GetInt32("_FontExtendSize");
			getUIRenderer()->getTextManager()->SetExternalFontSize(fontExtend);
#endif


		}

		m_pUIFirstLoad = UIFirstLoad::new_object();
		m_pUIFirstLoad->OnCreate(this);
	}

	I3PRINTLOG(I3LOG_NOTICE, "* 13");

	{
		INT32 idx = g_pFramework->FindLayerByName("EffectRoot");
		I3ASSERT(idx != -1);

		i3SgLayer * pEffectLayer = g_pFramework->getLayer(idx);
		I3ASSERT(pEffectLayer != nullptr);

		getEffectManager()->AttachToLayer(pEffectLayer);

		g_pEffectManager = (CEffectManager *)getEffectManager();
	}

	I3PRINTLOG(I3LOG_NOTICE, "* 14");

	// VersionInfo
	{
#if (defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)) && !defined( SALES_BUILD)
		m_pTextVersionInfo = i3TextNodeDX2::new_object();
		m_pTextFPS = i3TextNodeDX2::new_object();

#if USE_VERTEX_BANK
		m_pTextVersionInfo->Create(GetDefaultFontName(), FONT_COUNT_1024, GAME_FONT_SIZE_VERSION_INFO, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL, 1.0f, true);
#else
		m_pTextVersionInfo->Create( GetDefaultFontName(), FONT_COUNT_1024, GAME_FONT_SIZE_VERSION_INFO, FW_NORMAL, false, false, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL, 1.0f, false);
#endif

		m_pTextFPS->Create(GetDefaultFontName(), FONT_COUNT_1024, GAME_FONT_FPS_TEXT, FW_NORMAL, false, true, DEFAULT_QUALITY, true, FONT_SHADOW_QUALITY_NORMAL, 1.0f, true);
		m_pTextFPS->setSize(1024, 768);
		m_pTextFPS->SetAlign(TEXT_ATTR_ALIGN_RIGHT, TEXT_ATTR_ALIGN_TOP);
		m_pTextFPS->SetColor(250, 255, 0, 255);
		m_pTextFPS->SetEnable(true);

		m_pTextVersionInfo->setSize(1024, 768);
		m_pTextVersionInfo->SetAlign(TEXT_ATTR_ALIGN_LEFT, TEXT_ATTR_ALIGN_TOP);
		m_pTextVersionInfo->SetColor(200, 200, 200, 255);
		m_pTextVersionInfo->SetEnable(true);

		getMessagBoxLayer()->AddChild(m_pTextVersionInfo);
		getMessagBoxLayer()->AddChild(m_pTextFPS);

		// 버전 체크..인듯 한데...특별이 g_strVersion을 수정하지 말고..그냥 여기서 형변환 변경 하고 넘긴다.. ( 2014.07.15.수빈)
		i3::wstring wstrVersion;
		i3::mb_to_utf16(g_strVersion, wstrVersion);

		m_pTextVersionInfo->SetText(wstrVersion);
#endif
	}

	I3PRINTLOG(I3LOG_NOTICE, "* 15");

	m_pVideoCapture = CVideoCapture::new_object();
	m_pVideoCapture->InitHud3Texture();
	SetVideoCaptureInfo(g_pEnvSet->m_nVideoResolution);

	m_pVideoPlay = CVideoPlay::new_object();

	m_pRecLiveIcon = CRecLiveIcon::new_object();
	m_pRecLiveIcon->InitIcon();
	
	// 감마 설정
#ifndef USE_GAMMAOPTION_BY_SHADER
	if (g_pViewer->isFullScreen())
#endif
	VideoUtil::ApplyGammaState(g_pEnvSet->m_fGammaVal);

	m_pBGLoadThread = BGLoadThread::new_object();
	m_pBGLoadThread->Create("BGLoadThread", 0, 4096, nullptr, PRIORITY_LOWEST);

	I3PRINTLOG(I3LOG_NOTICE, "* 16");

	// Filter Table 생성

	m_StringFilter.reset(i3::make_pscoped<CStrFilter>());
	m_StringFilter->CreateFilter(STR_FILTER_INCLUDE_CHAT_NICK);//CreateFIlterAbuse();

#if defined( LOCALE_NORTHAMERICA )
	m_StringFilter2.reset( i3::make_pscoped<CStrFilter>()); 
	m_StringFilter2->CreateFilter( STR_FILTER_INCLUDE_CHAT_NICK2 );
#endif

	// Auto Mouse 관련
	{
		m_pAutoMouseTimer = i3Timer::new_object();
	}

	I3PRINTLOG(I3LOG_NOTICE, "* 17");

#if defined( USE_HOST_MODULE)
	m_pHostModule = CHostModule::new_object();
#endif

#ifdef I3_DEBUG
	//LoadDebugObj();
#endif

#if defined( TEST_AUTOMATION)
	{
		m_pTestAuto = new TestAutomationInterface;

		m_pTestAuto->Create(this);

		CaseRun::Init( *m_pTestAuto);
	}
#endif
}

#ifdef I3_DEBUG
void CGameFramework::LoadDebugObj(void)
{
	i3SceneFile sceneFile;

	i3AttrSet* pDebugAttrSet = i3AttrSet::new_object();

	UINT32 rv = sceneFile.Load("Object/effect/BN_arrow_Green.i3s");

	if (STREAM_ERR == rv)
	{
		I3PRINTLOG(I3LOG_WARN, "Couldn't find %s. Load Default Setting", "Object/effect/BN_arrow_Green.i3s");
		return;
	}

	m_arDebugObject[0] = sceneFile.GetSceneGraph();
	i3Matrix::SetPos( ((i3Transform*)m_arDebugObject[0])->GetMatrix(), 5.f, 2.f, 10.f);

	pDebugAttrSet->AddChild(m_arDebugObject[0]);

	g_pFramework->Get3DRoot()->AddChild( pDebugAttrSet);
}

void CGameFramework::SetPosDebugObj(INT32 idx, VEC3D& v3Pos)
{
	SetPosDebugObj(idx, v3Pos.x, v3Pos.y, v3Pos.z);
}

void CGameFramework::SetPosDebugObj(INT32 idx, REAL32 x, REAL32 y, REAL32 z)
{
	i3Matrix::SetPos( ((i3Transform*)m_arDebugObject[idx])->GetMatrix(), x, y, z);
}

#endif

void CGameFramework::_UpdateCommonLoadScreen(REAL32 fDeltaTime)
{
	if( m_pUIFirstLoad)
	{
		m_pUIFirstLoad->OnUpdate( fDeltaTime);
	}

}

void CGameFramework::_EndCommonLoadScreen(void)
{
	InitManagers();

	I3ASSERT( g_pWeaponManager == nullptr);
	g_pWeaponManager = CGameWeaponManager::new_object();
	g_pWeaponManager->Create();
	CGameWeaponManager::InitValue();

	WeaponLRU_Manager::i()->Init();
	WeaponLRU_Manager::i()->loadData();

	if( m_pUIFirstLoad)
		m_pUIFirstLoad->SetEnable( false);

	//Sound SetPendingEnable을 disable하기 위해 여기다 넣습니다.
	::InitSound();
}

void CGameFramework::OnCommonLoadStart( bool bFirst)
{
	
	// 루아를 읽을 경로를 심는다..
// 국가별 String Table 경로
//	char szLocalePath[MAX_STRING_COUNT] = {0};
//	sprintf_s(szLocalePath, MAX_STRING_COUNT, "Script\\uiLuaString%s", GetLocalePath(LOCALE_PATH_STRING_TABLE));	

	i3::rc_string strCommonUIRootFolder = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(false);

	i3Lua::AddPreloadDirectory(strCommonUIRootFolder.c_str(), false);
	i3Lua::AddPreloadDirectory("Script\\uiLua", false);	
	//
	i3Lua::AddPreloadRequireDirectory(strCommonUIRootFolder.c_str(), false);
	i3Lua::AddPreloadRequireDirectory("Script\\uiLua", false);		
	//

	if( m_pUIFirstLoad)
	{
		m_pUIFirstLoad->SetEnable( true);
		// ItemImageMgr초기화.. (접근은 매니저에서 마이어스싱글턴처리)
		if (g_pEnvSet->m_nUIVersion == UFV_1_5)
		{
			UIMainFrame::InitItemImageMgr();
		}
		else if (g_pEnvSet->m_nUIVersion == UFV_1_1)
		{
			UIMainFrame_V11::InitItemImageMgr();
		}
		if (g_pEnvSet->IsBaseOnUIVersion_1_0())
		{
			UIMainFrame_V10::InitItemImageMgr();
		}
		
	}

//	i3Framework::GetResourceManager()->setCbPreLoad(PreLoadFile);			// 컴파일 오류로 인한 주석처리(2013.07.29.수빈)
	i3Framework::OnCommonLoadStart( bFirst);

// RSG 파일에 등록을 해야하기 때문에 define을 미리 풀어놓습니다 - sanghun.han
//#if defined (MULTYLANGUAGE)
	i3::stack_string strLoadFilePath;
	i3::sprintf( strLoadFilePath, "Locale/%s/Sound/Voice.RSC", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );
	m_pResManager->LoadRSC( strLoadFilePath.c_str(), false);	// Locale 사운드
//#endif

	// BUGFIX : 아래 SetWindowPos함수 호출시, 한국버전 윈도우8.1의 전체화면 모드에서 작업표시줄이 나오는 버그가 있습니다.
	// 일단 주석처리하여 막지만, 후일 문제가 없다면 삭제처리 예정입니다..(2015.02.11.수빈)
	//	::SetWindowPos( g_hWnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
	//	::SetWindowPos( g_hWnd, HWND_NOTOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);

	enum { 
		EFFECT_IMAGE_PATH	= 0,
		EFFECT_HIGH_PATH,
		EFFECT_DEFAULT_FXLIST_PATH,
		EFFECT_SONGKRAN_FXLIST_PATH,
		EFFECT_CHINA_FXLIST_PATH,

		EFFECT_PATH_MAX
	};


	const char * pszEffectFile[EFFECT_PATH_MAX] = {
		// (Effect_Define.i3Pack에 들어갑니다.)
		"Effect/Define/EffectImage.RSC",
		"Effect/Define/Effects_High.RSC",
		"Effect/Define/Default.i3FxList",
		"Effect/Define/Songkran.i3FxList",
		"Effect/Define/China.i3FxList",
	};

	m_pResManager->LoadRSC( pszEffectFile[EFFECT_IMAGE_PATH], true);	// 이미지
	m_pResManager->LoadRSC( pszEffectFile[EFFECT_HIGH_PATH], true);	// 이펙트 상

	//	FxList 경로 지정.
	{

		char szFile[MAX_PATH] = "";

//////////////////////////////	새 방식에서는 Locale 구분 없이 
	#if defined (THAILAND_SONGKRAN_EVENT)		
		//	태국 물축제 피를 하늘색으로
		i3::safe_string_copy( szFile, pszEffectFile[EFFECT_SONGKRAN_FXLIST_PATH], MAX_PATH);
	#else
		//	기본 이펙트 파일 리스트
		i3::safe_string_copy( szFile, pszEffectFile[EFFECT_DEFAULT_FXLIST_PATH], MAX_PATH);
	#endif

		if( !i3FileStream::isFile( szFile))
		{
			I3PRINTLOG(I3LOG_FATAL,  "Cannot open effect file!! (%s)", szFile);
			return;
		}

		g_pFramework->SetCommonFxlistFileName( szFile);
	}

	I3ASSERT( g_pCharaManager == nullptr); 
	g_pCharaManager = CGameCharaManager::new_object();

	// 207MB
	g_pCharaManager->PreCreate();
}

bool CGameFramework::OnCommonLoading( bool bFirst, REAL32 fDeltaTime)
{
	_UpdateCommonLoadScreen(fDeltaTime);

	// 아바타 로딩
	if( g_pCharaManager != nullptr)
	{
		return g_pCharaManager->UpdateLoadDummyChara();
	}

	return i3Framework::OnCommonLoading( bFirst, fDeltaTime);
}

void CGameFramework::OnCommonLoadEnd( bool bFirst)
{
	_EndCommonLoadScreen();

	i3Framework::OnCommonLoadEnd( bFirst);
}

void CGameFramework::OnLoadStart( bool bFirst)
{
//	i3Framework::OnLoadStart( bFirst);

	// 로딩스크린을 생성합니다.
	if( GetCurrentStage() != nullptr && !i3::same_of<CStageExit*>(GetCurrentStage()) )
	{
		if(s_bNew_StageLoading == true)
		{
			if(getStageInfo( GetCurrentStageIndex()) != nullptr && IsBattleStage())
			{
				m_pHUDLoading->OnEntranceStart();
				m_rLoadingPercent = -1.0f;

				//GM observer일때 컬링을 꺼준다 xray이미지가 안보이기때문에,..
				if (UserInfoContext::i()->IsGM_Observer())
					m_pResManager->EnableOccMeshLoad(false);
				else
					m_pResManager->EnableOccMeshLoad(true);
			}

		}
		else
		{
			m_pGameSupportSet->SetLoadingSpr();
		}
	}
}

void CGameFramework::OnLoading( bool bFirst)
{
	i3Framework::OnLoading( bFirst);
}

void CGameFramework::OnLoadingGameData(bool bFirst)
{
	i3Framework::OnLoadingGameData(bFirst);
}

void CGameFramework::OnLoadEnd( bool bFirst)
{
	SetGameCursor();

	i3Framework::OnLoadEnd( bFirst);
}

bool CGameFramework::OnLostDevice( bool bLostDevice)
{
	if( g_pCharaManager != nullptr)
		g_pCharaManager->LostDevice();

	if( bLostDevice)
	{
		if( m_pVideoPlay != nullptr)
		{
			m_pVideoPlay->LostDevice();
		}
	}

	if( i3Framework::OnLostDevice( bLostDevice) == false)
		return false;

	return true;
}

bool CGameFramework::OnRevive(i3RenderContext * pCtx)
{
	if( m_pVideoPlay != nullptr)
		m_pVideoPlay->Revive();

	if( i3Framework::OnRevive( pCtx) == false)
		return false;

	if( g_pCharaManager != nullptr)
		g_pCharaManager->Revive( pCtx);

	if( g_pClanMarkManager != nullptr)
		g_pClanMarkManager->OnRevive( pCtx);

	return true;
}

void CGameFramework::setShadowEnable( bool bFlag)
{
	INT32 i;
	i3Light * pLight;
	i3SceneGraphContext * pCtx = getSgContext();
	I3ASSERT( pCtx != nullptr);

	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	switch( pCaps->getGPUClass())
	{
	case I3G_GPU_CLASS_F :
	case I3G_GPU_CLASS_E :
	case I3G_GPU_CLASS_D :
		break;

	default :
		for( i = 0; i < pCtx->getLightCount(); i++)
		{
			pLight = pCtx->getLight( i);

			if( pLight->GetLightType() == I3G_LIGHT_DIRECTIONAL)
			{
				if( bFlag == true)
					pLight->addStyle( I3G_LIGHT_STYLE_SHADOWMAP);
				else
					pLight->removeStyle( I3G_LIGHT_STYLE_SHADOWMAP);
			}
		}
		break;
	}
}

void CGameFramework::setShadowQuality( INT32 qual, bool bManualBound)
{
	INT32 i;
	i3Light * pLight;
	i3SceneGraphContext * pCtx = getSgContext();
	I3ASSERT( pCtx != nullptr);

	i3GfxCaps * pCaps = i3RenderContext::GetCaps();

	switch( pCaps->getGPUClass())
	{
	case I3G_GPU_CLASS_F :
	case I3G_GPU_CLASS_E :
	case I3G_GPU_CLASS_D :
		break;

	default :
		{
			UINT32 add = I3G_LIGHT_STYLE_SHADOWMAP;

			if( bManualBound )
				add |= I3G_LIGHT_STYLE_CALC_BOUND;

			for( i = 0; i < pCtx->getLightCount(); i++)
			{
				pLight = pCtx->getLight( i);

				if( pLight->GetLightType() == I3G_LIGHT_DIRECTIONAL)
				{
					switch( qual)
					{
						case 1 :	pLight->setShadowMapSize( 512);		pLight->addStyle( add);	break;
						case 2 :	pLight->setShadowMapSize( 1024);	pLight->addStyle( add);	break;
						case 3 :	pLight->setShadowMapSize( 2048);	pLight->addStyle( add);	break;
						default :	pLight->removeStyle( I3G_LIGHT_STYLE_SHADOWMAP);									break;
					}
				}
			}

			pCtx->setManualBoundEnable( bManualBound);
		}
		break;
	}
}

void CGameFramework::SetNormalMapEnable( bool bEnable)
{
	g_pViewer->GetRenderContext()->getGfxOption()->setNormalMapEnable( bEnable);

	// 로딩된 리소스중 NormalMap이 설정되어 있는 것을 꺼준다.
	// Chara Dummy
	g_pCharaManager->SetDummyCharacterOption( bEnable, (g_pEnvSet->m_nSpecularQualityType>0)?true:false);
}

#if !defined( I3_NO_PROFILE)
void CGameFramework::UpdateRecvmsg()
{
#if defined(USE_EDITDLG)
	if( m_hWndDbgMsgDlg != nullptr)
	{
		::SendMessage( m_hWndDbgMsgDlg, UPDATE_DBG_MESSAGE + 1, 0, 0);
	}
#endif
}

void CGameFramework::UpdateSendmsg()
{
	#if defined(USE_EDITDLG)
	if( m_hWndDbgMsgDlg != nullptr)
	{
		::SendMessage( m_hWndDbgMsgDlg, UPDATE_DBG_MESSAGE + 2, 0, 0);
	}
	#endif
}

void CGameFramework::UpdateGrenadeState()
{
	#if defined(USE_EDITDLG)
	if( m_hWndDbgMsgDlg != nullptr)
	{
		::SendMessage( m_hWndDbgMsgDlg, UPDATE_DBG_MESSAGE + 3, 0, 0);
	}
	#endif
}

void CGameFramework::UpdatePacketState()
{
	#if defined(USE_EDITDLG)
	if( m_hWndDbgMsgDlg != nullptr)
	{
		::SendMessage( m_hWndDbgMsgDlg, UPDATE_DBG_MESSAGE + 4, 0, 0);
	}
	#endif
}

void CGameFramework::UpdateCharaState()
{
	#if defined(USE_EDITDLG)
	if( m_hWndDbgMsgDlg != nullptr)
	{
		::SendMessage( m_hWndDbgMsgDlg, UPDATE_DBG_MESSAGE + 5, (WPARAM)g_pCharaManager, 0);
	}
	#endif
}

void CGameFramework::UpdateHolepunchState()
{
	#if defined(USE_EDITDLG)
	if( m_hWndDbgMsgDlg != nullptr)
	{
		::SendMessage( m_hWndDbgMsgDlg, UPDATE_DBG_MESSAGE + 6, 0, 0);
	}
	#endif
}

void CGameFramework::UpdateTcpMessage(UINT32 nType)
{
	#if defined(USE_EDITDLG)
	if( m_hWndDbgMsgDlg != nullptr)
	{
		::SendMessage( m_hWndDbgMsgDlg, UPDATE_DBG_MESSAGE + 7, (WPARAM)nType, 0);
	}
	#endif
}

void CGameFramework::LogMessageToDbgWindow( const char *format, ...)
{
	#if defined(USE_EDITDLG)

	static char dbgstring[256];

	va_list marker;

	va_start( marker, format);

	vsprintf_s( dbgstring, format, marker);

	if( m_hWndDbgMsgDlg != nullptr)
	{
		::SendMessage( m_hWndDbgMsgDlg, UPDATE_DBG_MESSAGE + 8, (WPARAM)dbgstring, 0);
	}
	#endif
}
#endif

struct KEYMAPINFO
{
	UINT32		m_I3IKey;
	UINT64		m_GameKey;
} ;

KEYMAPINFO s_MenuKeyMap[] =
{
	{	I3I_KEY_LEFT,				GAME_KEY_LEFT		},
	{	I3I_KEY_RIGHT,				GAME_KEY_RIGHT		},
	{	I3I_KEY_UP,					GAME_KEY_UP			},
	{	I3I_KEY_DOWN,				GAME_KEY_DOWN		},
	{	I3I_KEY_ENTER,				GAME_KEY_CMD_CHAT	},
	{	I3I_KEY_NUMPADENTER,		GAME_KEY_CMD_CHAT	},
	{	I3I_KEY_ESC,				GAME_KEY_FUNC_ESC	},
	{	I3I_KEY_TAB,				GAME_KEY_TAB		},
	{	0,							0L					},
};

static void sAfterLoadChara_ForAI( void * pAvatar, INT32 idx, AI_BIND_DATA * pAIBind)
{
	GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RESPAWN_BATTLE_FOR_AI, idx);
}

void CGameFramework::OnInput()
{
	UINT32 btn = 0;
	i3InputDeviceManager	*	pMng	= m_pViewer->GetInputDeviceManager();
	i3InputKeyboard			*	pKey	= pMng->GetKeyboard();
	i3InputMouse			*	pMouse	= pMng->GetMouse();

#if defined( TEST_AUTOMATION)
	CaseRun::Tick();
#endif

	m_nOldKeyPress = m_nKeyPress;
	m_nOldKeyStroke = m_nKeyStroke;

	m_nKeyPress = 0;
	m_nKeyStroke = 0;

	if( GetCurrentStage() != nullptr )
	{
		g_pFramework->SetGameCursor();
	}

	if( ::GetFocus() == g_pViewer->GetWindowHandle())
	{

	}

	//else
	//{
	//	// 채팅창에 포커스가 있다면, 게임어플리케이션에도 포커스가 맞추어져 있다고 가정합니다.
	//	if (IsBattleStage())
	//	{
	//		CStageBattle* pStage = (CStageBattle*)GetCurrentStage();

	//		if( pStage != NULL && CHud::i() != NULL)
	//		{
	//			CHud* pHud = CHud::i();		//pStage->getHud()
	//			if( pHud->getChatBox() != NULL)
	//			{
	//				if(pHud->getChatBox()->getIME() != NULL)
	//				{
	//					if( pHud->getChatBox()->getIME()->getFocus())
	//					{
	//						//g_pFramework->SetGameCursor();
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	if( pKey == nullptr)
		return;

	if( pKey->GetPressState( I3I_KEY_LALT))
	{
		if( pKey->GetStrokeState( I3I_KEY_F4))
		{
			pKey->SetStrokeState( I3I_KEY_ESC, true);
		}

#if defined( ENABLE_ALT_ENTER)
		if( pKey->GetStrokeState( I3I_KEY_ENTER))
		{
			i3VideoInfo info;

			g_pViewer->GetVideoInfo( &info);

			info.m_bFullScreen = !info.m_bFullScreen;

			g_pEnvSet->m_bFullScreen = info.m_bFullScreen;
			I3TRACE( "env save start\n");
			g_pEnvSet->Save();
			I3TRACE( "env save end\n");

			i3Stage * pStage = GetCurrentStage();

			g_pViewer->SetVideoInfo( &info);

		}
#endif
		#if defined( I3_DEBUG) || defined( I3_RELEASE_PROFILE)//디버그 키
		if( pKey->GetPressState( I3I_KEY_LCTRL) && pKey->GetPressState( I3I_KEY_LSHIFT))
		{
			if( pKey->GetStrokeState( I3I_KEY_P))
			{
				_LogPerf();
			}
			else if( pKey->GetStrokeState( I3I_KEY_COMMA))
			{
				SetFresnelEnable( true);
			}
			else if( pKey->GetStrokeState( I3I_KEY_PERIOD))
			{
				SetFresnelEnable( false);
			}
		}
		#endif

		// 프로파일러 기록 리셋과 스냅샷을 위해서 삽입 ( 2013.01.23.수빈)

#if defined( BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG ) //디버그 키
		if (pKey->GetStrokeState( I3I_KEY_DEL ) )
		{
			i3_prof_reset();
		}
		if (pKey->GetStrokeState(I3I_KEY_INS))
		{
			i3_prof_save("performance_profile_snapshot.txt");
		}

		if (pKey->GetStrokeState(I3I_KEY_PGUP))
		{
			OccDebugInfo* pOccDebugInfo = this->GetViewer()->getOccDebugInfo();
			if (pOccDebugInfo)
				pOccDebugInfo->SetAABBRenderEnable(!pOccDebugInfo->IsAABBRenderEnabled());
		}
		
		if (pKey->GetStrokeState(I3I_KEY_PGDN))
		{
			if (i3::same_of<CStageBattle*>(GetCurrentStage()))
			{
				OccDebugInfo* pOccDebugInfo = this->GetViewer()->getOccDebugInfo();
				if (pOccDebugInfo)
					pOccDebugInfo->SetWorldQuadRenderEnable(!pOccDebugInfo->IsWorldQuadRenderEnabled());
			}
		}

		if (pKey->GetStrokeState(I3I_KEY_HOME))
		{
			OccDebugInfo* pOccDebugInfo = this->GetViewer()->getOccDebugInfo();
			if (pOccDebugInfo)
				pOccDebugInfo->SetTextEnable(!pOccDebugInfo->IsTextEnabled());
		}
#endif
	}

#if (defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)) && !defined( SALES_BUILD)
	if (pKey->GetStrokeState(I3I_KEY_F11))
	{
		m_bVersionInfo = !m_bVersionInfo;

		m_pTextFPS->SetEnable(m_bVersionInfo);
		m_pTextVersionInfo->SetEnable(m_bVersionInfo);
	}
#endif

	// 맨 앞에 나오는 로고 취소
	if(m_bCommonLoadingStarted == false)
	{
		if( pKey->GetStrokeState( I3I_KEY_ESC))
		{
			if( m_pAttrSetLogo[0] != nullptr)
			{
				if( m_pUseMovie)
					m_pAttrSetLogo[0]->AddFlag(I3_NODEFLAG_DISABLE);
				m_pAttrSetLogo[1]->AddFlag(I3_NODEFLAG_DISABLE);
				m_pAttrSetLogo[2]->AddFlag(I3_NODEFLAG_DISABLE);
				m_pAttrSetLogo[3]->AddFlag(I3_NODEFLAG_DISABLE);
			}
			if( m_pAttrSetRatingLogo != nullptr)
				m_pAttrSetRatingLogo->AddFlag(I3_NODEFLAG_DISABLE);

			//I3_SAFE_RELEASE( m_pMovie);
			SetState(STATE_COMMON_LOAD_START);
		}
	}
	
	if(LocaleValue::Enable("EnableVideoCapture"))
	{
		//환경설정 -  동영상 키 설정중일 경우, 동영상 녹화 방지 코드
		//동영상 녹화 - 양승천[2011.06.17]
		if(m_EnableCapture)
		{			
			const GAME_KEY_STRUCT& vidCapKey = g_pEnvSet->m_KeyMapArray[GAME_KEY_MAP_VIDEOCAPTURE];
			bool doVidCap = false;

			// chunjong 2014-08-27 - 동영상키가 마우스휠업/다운등으로 바뀔때 처리를위해
			if(vidCapKey._nDiviceType == GAME_DEVICE_MOUSE)
			{
				if(pMouse->GetStrokeButtonState() && vidCapKey._nState)
					doVidCap = true;
			}
			else
			{	// keyboard
				if(pKey->GetStrokeState((I3I_KEY)vidCapKey._nState))
					doVidCap = true;					
			}

			if(doVidCap)
				m_pVideoCapture->SetVideoCapture(m_pChatBox);
		}
	}

	//Logger On/Off
	if (pKey->GetPressState(I3I_KEY_LCTRL))
	{
		if (pKey->GetPressState(I3I_KEY_L))
		{
			pKey->SetPressState(I3I_KEY_L, false);
			g_pFramework->ConsoleLogger();
		}
	}

	// Chat Box에 Focus가 맞추어져 있다면, 게임에서 사용되는 키는 입력을 받지 않습니다.
	if( m_GameKeyMode == GAMEKEY_MODE_MENU )
	{
		INT32 i;

		for( i = 0; s_MenuKeyMap[i].m_I3IKey != 0; i++)
		{
			if( pKey->GetPressState( (I3I_KEY) s_MenuKeyMap[i].m_I3IKey))
			{
				m_nSysKeyPress |= s_MenuKeyMap[i].m_GameKey;
				if( pKey->GetStrokeState( (I3I_KEY) s_MenuKeyMap[i].m_I3IKey))	m_nSysKeyState |= s_MenuKeyMap[i].m_GameKey;
			}
		}

		for( i=1; i<GAME_KEY_MAP_COUNT; i++)
		{
			if( g_pEnvSet->m_KeyMapArray[i]._nDiviceType == GAME_DEVICE_KEYBOARD)
			{
				if (pKey->GetStrokeState( (I3I_KEY)g_pEnvSet->m_KeyMapArray[i]._nState))
				{
					switch( i)
					{
						case GAME_KEY_MAP_CHANGEWEAPON:			SetSysStrokeKeyMap(GAME_KEY_CHANGEWEAPON);		break;
						// 기타 관련 키
						case GAME_KEY_MAP_SCREENSHOT: 			m_bEnableScreenShot = true;						break;

						case GAME_KEY_MAP_CHAT_ALL:				SetGameStrokeKeyMap(GAME_KEY_FUNC_F2);			break;
						case GAME_KEY_MAP_CHAT_TEAM:			SetGameStrokeKeyMap(GAME_KEY_FUNC_F3);			break;
						case GAME_KEY_MAP_CHAT_CLAN:			SetGameStrokeKeyMap(GAME_KEY_FUNC_F4);			break;
						case GAME_KEY_MAP_CHAT_HISTORY:			SetGameStrokeKeyMap(GAME_KEY_FUNC_CHATSCROLL);	break;
					}
				}

				if (pKey->GetPressState((I3I_KEY)g_pEnvSet->m_KeyMapArray[i]._nState))
				{
					switch(i)
					{
						case GAME_KEY_MAP_CHANGEWEAPON:			SetSysPressKeyMap(GAME_KEY_CHANGEWEAPON);			break;
					}
				}
			}
			else if( g_pEnvSet->m_KeyMapArray[i]._nDiviceType == GAME_DEVICE_MOUSE)
			{
				btn = pMouse->GetStrokeButtonState();
				if( btn == g_pEnvSet->m_KeyMapArray[i]._nState && g_pEnvSet->m_KeyMapArray[i]._nState != 0)
				{
					switch( i)
					{
						case GAME_KEY_MAP_CHANGEWEAPON:			SetSysPressKeyMap(GAME_KEY_CHANGEWEAPON);		break;
						// 기타 관련 키
						case GAME_KEY_MAP_SCREENSHOT: 			m_bEnableScreenShot = true;						break;

						case GAME_KEY_MAP_CHAT_ALL:				SetGamePressKeyMap(GAME_KEY_FUNC_F2);			break;
						case GAME_KEY_MAP_CHAT_TEAM:			SetGamePressKeyMap(GAME_KEY_FUNC_F3);			break;
						case GAME_KEY_MAP_CHAT_CLAN:			SetGameStrokeKeyMap(GAME_KEY_FUNC_F4);			break;
						case GAME_KEY_MAP_CHAT_HISTORY:			SetGamePressKeyMap(GAME_KEY_FUNC_CHATSCROLL);	break;
					}
				}

				btn = pMouse->GetButtonState();
				if (btn == g_pEnvSet->m_KeyMapArray[i]._nState)
				{
					switch(i)
					{
						case GAME_KEY_MAP_CHANGEWEAPON:			SetSysPressKeyMap(GAME_KEY_CHANGEWEAPON);			break;
					}
				}
			}
		}
	}
	// 이부분에서부터는 실제 게임안에서 사용되는 키!!(FPS)
	else
	{
		for( INT32 i=1; i<GAME_KEY_MAP_COUNT; i++)
		{
			if( g_pEnvSet->m_KeyMapArray[i]._nDiviceType == GAME_DEVICE_KEYBOARD)
			{
				static INT32 o = 0;

				if( pKey->GetPressState( (I3I_KEY)g_pEnvSet->m_KeyMapArray[i]._nState) )
				{
					SetGameKey( 0, i);
				}

				if( pKey->GetStrokeState( (I3I_KEY)g_pEnvSet->m_KeyMapArray[i]._nState))
				{
					SetGameKey( 1, i);
				}
			}
			else if( g_pEnvSet->m_KeyMapArray[i]._nDiviceType == GAME_DEVICE_MOUSE)
			{
				btn = pMouse->GetButtonState();
				m_nMouseButtonState = btn;

				if( btn & g_pEnvSet->m_KeyMapArray[i]._nState)
				{
					SetGameKey( 0, i);
				}

				btn = pMouse->GetStrokeButtonState();
				if( btn & g_pEnvSet->m_KeyMapArray[i]._nState)
				{
					SetGameKey( 1, i);
				}
			}
		}

		if( pKey->GetPressState( I3I_KEY_ESC)  )
		{
			SetGamePressKeyMap(GAME_KEY_FUNC_ESC);
			if( pKey->GetStrokeState( I3I_KEY_ESC))
			{
				SetGameStrokeKeyMap(GAME_KEY_FUNC_ESC);
			}
		}

		if( pKey->GetPressState( I3I_KEY_LALT))
		{
			if( pKey->GetStrokeState( I3I_KEY_F4))
			{
				SetGameStrokeKeyMap(GAME_KEY_FUNC_ESC);
			}
		}

		//////////////////////////////////////////////////////////////////////////////
		//Fly Camera인경우에 방향전환을 가능하도록 하는 키 입니다.
		if( pKey->GetPressState( I3I_KEY_LEFT))
		{
			SetGamePressKeyMap( GAME_KEY_CMD_CAMPIVOT_LEFT);
		}

		if( pKey->GetPressState( I3I_KEY_RIGHT))
		{
			SetGamePressKeyMap( GAME_KEY_CMD_CAMPIVOT_RIGHT);
		}

		if( pKey->GetPressState( I3I_KEY_UP))
		{
			SetGamePressKeyMap( GAME_KEY_CMD_CAMPIVOT_UP);
		}

		if( pKey->GetPressState( I3I_KEY_DOWN))
		{
			SetGamePressKeyMap( GAME_KEY_CMD_CAMPIVOT_DOWN);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// 실제 게임에서는 사용되지 말아야 할키입니다.
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if( pKey->GetPressState( I3I_KEY_NUMPADENTER)  )
		{
			m_nKeyPress |= GAME_KEY_CMD_CHAT;
			if( pKey->GetStrokeState( I3I_KEY_NUMPADENTER))		m_nKeyStroke |= GAME_KEY_CMD_CHAT;
		}

		if( pKey->GetPressState( I3I_KEY_LCTRL) )
		{
			if( pKey->GetPressState( I3I_KEY_LSHIFT) )
			{
				if( pKey->GetPressState( I3I_KEY_9 ) )
				{
					m_nKeyPress |= GAME_KEY_CMD_ALLDEATH;
					if( pKey->GetStrokeState( I3I_KEY_9 ) )				m_nKeyStroke |= GAME_KEY_CMD_ALLDEATH;
				}

				if( pKey->GetPressState( I3I_KEY_0)  )
				{
					m_nKeyPress |= GAME_KEY_CMD_RESET;
					if( pKey->GetStrokeState( I3I_KEY_0))				m_nKeyStroke |= GAME_KEY_CMD_RESET;
				}
			}
		}

		if (pKey->GetStrokeState(I3I_KEY_H))
		{
			g_pCharaManager->SetXrayState();
		}

#if defined(I3_DEBUG) || defined( I3_RELEASE_PROFILE) //디버그 키
	#if defined( DEF_OBSERVER_MODE)
		if( BattleSlotContext::i()->isObserverMe() == false)
		{
	#endif

		if( pKey->GetStrokeState( I3I_KEY_NUMPADMULTIPLY ))
		{
			if( BattleServerContext::i()->IsP2PActivate() )
			{
				if( pKey->GetPressState( I3I_KEY_LSHIFT))
				{
					s_nAddCharaDebugCount--;
					if( s_nAddCharaDebugCount < 1)
						s_nAddCharaDebugCount = 15;
					GameEventReceiver::i()->PushReceivedGameEvent( EVENT_GIVEUP_BATTLE, s_nAddCharaDebugCount );
				}
				else
				{
					AIModeContext::i()->EnemyCount		= 8;
					AIModeContext::i()->FriendCount		= 7;
					AIModeContext::i()->StartLevel		= 1;
					AIModeContext::i()->CurrentLevel	= 1;

					util_battle::AddAiChara( s_nAddCharaDebugCount); 

					s_nAddCharaDebugCount++;
					if( s_nAddCharaDebugCount >= SLOT_MAX_COUNT )
						s_nAddCharaDebugCount	= 1;
				}
			}
		}
		else if( pKey->GetStrokeState( I3I_KEY_NUMPADPLUS) && ( pKey->GetPressState( I3I_KEY_LSHIFT) ))
		{
			// Numpad + 키를 누르면 Random하게 Dummy character를 추가한다.

			if( BattleServerContext::i()->IsP2PActivate() )
			{
				//CHARA_RES_ID CharaType = (CHARA_RES_ID)((i3Math::Rand() % (CHARA_RES_ID_HUMAN_E - 1)) + 1);

				NET_CHARA_INFO * pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo( s_nAddCharaDebugCount);

				pNetCharaInfo->setRespawnIndex( s_nAddCharaDebugCount);

				//	현재 주인공이 가진 무기를 그대로 장비시킨다.
				NET_CHARA_INFO * pMyCharaInfo = BattleSlotContext::i()->setNetCharaInfo( BattleSlotContext::i()->getMySlotIdx() );
				pNetCharaInfo->SetWeaponList( pMyCharaInfo->getWeaponList( WEAPON_SLOT_PRIMARY));

				{
					static WEAPON_CLASS_TYPE s_PriClassTable[] =
					{
						WEAPON_CLASS_ASSAULT,
						WEAPON_CLASS_SMG,
						WEAPON_CLASS_DUALSMG,
						WEAPON_CLASS_SNIPER,
						WEAPON_CLASS_SHOTGUN,
						WEAPON_CLASS_MG,
					};

					static WEAPON_CLASS_TYPE s_SecClassTable[] =
					{
						WEAPON_CLASS_HANDGUN,
						WEAPON_CLASS_DUALHANDGUN,
						WEAPON_CLASS_CIC,
					};

					static WEAPON_CLASS_TYPE s_MeleeClassTable[] =
					{
						WEAPON_CLASS_KNIFE,
						WEAPON_CLASS_DUALKNIFE,
					};

					static WEAPON_CLASS_TYPE s_ItemClassTable[] =
					{
						WEAPON_CLASS_THROWING_CONTAIN,
						WEAPON_CLASS_THROWING_ATTACK,
						WEAPON_CLASS_THROWING_HEAL,
					};

					
					pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, WEAPON::MakeRandomWeaponItem( s_PriClassTable, GET_ARRAY_COUNT( s_PriClassTable)));
					pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_SECONDARY, WEAPON::MakeRandomWeaponItem( s_SecClassTable, GET_ARRAY_COUNT( s_SecClassTable)));
					pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_MELEE, WEAPON::MakeRandomWeaponItem( s_MeleeClassTable, GET_ARRAY_COUNT( s_MeleeClassTable)));
					pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING1, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_THROWING1,	WEAPON_CLASS_THROWING_GRENADE, GetItemIndex("THROWING_K400")));
					pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_MELEE, WEAPON::MakeRandomWeaponItem( s_ItemClassTable, GET_ARRAY_COUNT( s_ItemClassTable)));
				}

				AI_BIND_DATA * pBindData = g_pCharaManager->getAiBindData( s_nAddCharaDebugCount / TEAM_COUNT);

				// vv3 - check
				BattleSlotContext::i()->setCharaType( s_nAddCharaDebugCount, CHARA::TYPE_AI);
				T_ItemID chara_item_id = pBindData->_Parts.GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);

				for( INT32 idx = 0 ; idx < CHAR_EQUIPMENT_PARTS_COUNT ; ++idx)
				{
					if( idx == CHAR_EQUIPMENT_PARTS_CHARA || idx == CHAR_EQUIPMENT_PARTS_SKIN ) 	continue;
					if( idx == CHAR_EQUIPMENT_PARTS_BERET )	continue;	// 임시 예외처리.

					EQUIP::ePART parts_type = EQUIP::CommonParts2eParts( static_cast<CHAR_EQUIPMENT_PARTS>(idx) );
					T_ItemID item_id = GlobalFunc::GetRandParts( parts_type, chara_item_id);

					pBindData->_Parts.SetPartsItemID( static_cast<CHAR_EQUIPMENT_PARTS>(idx), item_id );
				}

				if( pKey->GetPressState( I3I_KEY_LCTRL))
					g_pCharaManager->CreateAvatarAI( (AvatarLoadedCallback) sAfterLoadChara_ForAI, s_nAddCharaDebugCount, pNetCharaInfo, CHARA::TYPE_AI, pBindData);
				else
					g_pCharaManager->CreateAvatarAI( (AvatarLoadedCallback) sAfterLoadChara_ForAI, s_nAddCharaDebugCount, pNetCharaInfo, CHARA::TYPE_PLAYER_NET, pBindData);

				BattleSlotContext::i()->setNickForSlot( s_nAddCharaDebugCount, s_szDebugCharaName[ s_nAddCharaDebugCount ]);

				s_nAddCharaDebugCount++;
				if( s_nAddCharaDebugCount >= SLOT_MAX_COUNT )
					s_nAddCharaDebugCount	= 1;
			}
		}
	#if defined( DEF_OBSERVER_MODE)
		}
	#endif

#endif


	#if defined( BUILD_RELEASE_QA_VERSION)//디버그 키
	{
		if( pKey->GetPressState( I3I_KEY_LCTRL))
		{
			static bool s_nigntvisionenable = true;
			static INT32 s_nvMode = 0;

			if( pKey->GetStrokeState( I3I_KEY_O))
			{
				m_pPostProcessNightVision->SetEnable( !s_nigntvisionenable);

				s_nigntvisionenable = !s_nigntvisionenable;
			}

			if( s_nigntvisionenable == true)//nv가 enable된 상태에서만 masking type이 변하도록.
			{
				if( pKey->GetStrokeState( I3I_KEY_P))
				{
					SetVisionMaskingType( (VISION_MASK_TYPE)s_nvMode);

					s_nvMode++;

					if( s_nvMode >= VE_MAX_COUNT)
						s_nvMode = 0;
				}
			}
		}
	}
	#endif

	#if !defined( I3_NO_PROFILE)

		#if defined(USE_EDITDLG)
			/*
			if( pKey->GetPressState( I3I_KEY_LCTRL))
			{
				if( pKey->GetStrokeState( I3I_KEY_F10))
				{
					if( !::IsWindow( m_hWndDbgMsgDlg))
					{
						m_hWndDbgMsgDlg = (HWND)PopUpDbgMsgDlg( g_pGameContext);
					}
				}

				if( pKey->GetStrokeState( I3I_KEY_F9))
				{
					if( !::IsWindow( m_hWndProfilingDlg))
					{
						m_hWndProfilingDlg = (HWND) PopupProfilingDlg();
					}
				}

				if( pKey->GetStrokeState( I3I_KEY_F11))
				{
					if( !::IsWindow( m_hWndSoundEditDlg))
					{
						m_hWndSoundEditDlg = (HWND)PopUpSoundEditDlg( this);
					}
				}
			}
			else
			{
				if( pKey->GetStrokeState( I3I_KEY_F10))
				{
					if( !::IsWindow( m_hWndEdigGameDlg))
					{
						m_hWndEdigGameDlg = (HWND)PopUpGameEditDlg( this);
					}
				}
			}
			*/
		#endif

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

			if( pKey->GetPressState( I3I_KEY_LCTRL))
			{

#if defined( I3_DEBUG)//디버그 키
				if( pKey->GetStrokeState( I3I_KEY_1))
				{
					CGameMissionManager::i()->setUsurpationIdx(0);
				}

				if( pKey->GetStrokeState( I3I_KEY_2))
				{
					CGameMissionManager::i()->setUsurpationIdx(255);
				}
#endif
				if( pKey->GetStrokeState( I3I_KEY_F4))
				{
					REPORT_PROFILE();
				}

				if( pKey->GetStrokeState( I3I_KEY_H))
				{
					if( pPlayer != nullptr)
						pPlayer->setLeftHand( !pPlayer->isLeftHand());
				}

				if( pKey->GetStrokeState( I3I_KEY_K))
				{
					//g_pPlayer->Shove( &I3_XAXIS, 0.1f);
#if defined( I3_DEBUG)//디버그 키
					i3GfxResource::DumpTextures();
#endif
				}

				#if defined( I3_DEBUG)//디버그 키
				if( pKey->GetStrokeState( I3I_KEY_L))
				{
					//i3MemDump( I3MEMORY_DEBUG_FILE | I3MEMORY_DEBUG_SUM | I3MEMORY_DEBUG_EACH);
					i3ElementBase::DumpObjects( 0, i3MemoryBuffer::static_meta());
				}

				if( pKey->GetStrokeState( I3I_KEY_N))
				{
					i3ElementBase::ReportClassInstanceCounts();
				}
				#endif

				if( pKey->GetPressState( I3I_KEY_LSHIFT))
				{
					#if defined( I3_DEBUG)//디버그 키
					/*if( pKey->GetStrokeState( I3I_KEY_F1))
					{
						g_pFramework->DumpObjects();
					}*/

					if( pKey->GetStrokeState( I3I_KEY_F2))
					{
						g_pViewer->GetPhysixContext()->Dump();
					}
					#endif

					if( pKey->GetStrokeState( I3I_KEY_F5))
					{
						/*
						i3SoundContext * pCtx = g_pViewer->GetSoundContext();
						if( pCtx != NULL && pCtx->m_bEnableFX)
						{
							UINT32 nPreset = pCtx->GetReverbParam();
							nPreset++;

							if( nPreset > DSFX_I3DL2_ENVIRONMENT_PRESET_PLATE)
							{
								nPreset = DSFX_I3DL2_ENVIRONMENT_PRESET_DEFAULT;
							}

							pCtx->SetReverbParam( nPreset);
						}
						*/
					}

					if( pKey->GetStrokeState( I3I_KEY_8))
					{
						BattleObjContext::i()->CrashAllStageObjects();
					}
				}
			}

			if( pKey->GetPressState( I3I_KEY_LSHIFT))
			{
				if( pKey->GetStrokeState( I3I_KEY_D))
				{
					if( m_nLightChangeState < 2)
					{
						m_nLightChangeState++;
					}
					else
					{
						m_nLightChangeState = 0;
					}
				}
				else
				{
					if (IsBattleStage())
					{
						//__AdjustLightAttr( m_nLightChangeState);
					}
				}
			}
		#endif

	#if defined(I3_DEBUG)//디버그 키
			// Debug시 와이어프레임 및 폴리곤으로 전환하기 위한 코드입니다. (송명일)
			if( pKey->GetPressState( I3I_KEY_LCTRL))
			{
				if( pKey->GetStrokeState( I3I_KEY_F5))
				{
					if( GetViewer()->GetRenderContext()->GetRenderMode() == I3G_RENDER_LINE)
					{
						GetViewer()->GetRenderContext()->SetRenderMode( I3G_RENDER_POLYGON);
					}
					else
					{
						GetViewer()->GetRenderContext()->SetRenderMode( I3G_RENDER_LINE);
					}
				}

				if( pKey->GetStrokeState( I3I_KEY_F6))
				{
					g_pPhysixCtx->Dump();
				}
			}
	#endif
	}

	#if defined( BUILD_RELEASE_QA_VERSION)//디버그 키
		if( pKey->GetStrokeState( I3I_KEY_NUMPADPLUS))
		{
			g_bStep = true;
		}
		if( pKey->GetStrokeState( I3I_KEY_NUMPADDIVIDE))
		{
			g_bPause = !g_bPause;
		}
	#endif

	#if defined( I3_DEBUG) || defined( I3_RELEASE_PROFILE) //디버그 키
		if( pKey->GetPressState( I3I_KEY_LCTRL))
		{
			#if defined( AI_MOVE_TO_WEAPON)
			if( pKey->GetStrokeState( I3I_KEY_C))
			{
				CheckWeapons();
			}
			#endif

			if( pKey->GetPressState( I3I_KEY_LALT))
			{
				#if defined( I3_DEBUG)
				if( pKey->GetStrokeState( I3I_KEY_S))
				{
					i3ElementBase::setMemTagValue( 10);
				}
				else if( pKey->GetStrokeState( I3I_KEY_C))
				{
					i3ElementBase::setMemTagValue( 0);
				}
				else if( pKey->GetStrokeState( I3I_KEY_D))
				{
					i3ElementBase::DumpObjects( 0, nullptr, 9);
				}
				else if( pKey->GetStrokeState( I3I_KEY_R))
				{
					i3ElementBase::SnapMemoryTag( 0);
				}
				#endif
			}

			if( pKey->GetStrokeState( I3I_KEY_F8))
			{
				/*I3_SG_DUMP( this->getSgContext()->get3DRoot());
				i3ElementBase::DumpObjects( 0, i3Object::static_meta());
				i3mem::Dump();*/
				i3SceneFile file;
				/*{
					i3OptExternRef opt;
					opt.setExternTextureEnable( false);
					opt.Trace( getSgContext()->get3DRoot());
				}*/
				
				file.SetSceneGraph( GetLayerRoot( "EffectRoot"));
				file.Save( "D:\\dump.i3s");
			}

			if( pKey->GetPressState( I3I_KEY_LSHIFT)
				&& pKey->GetStrokeState( I3I_KEY_0) )
			{
				//const NxSceneStats2 * pStats = this->getPhysixContext()->getScene()->getStats2();
				//for( UINT32 i = 0; i < pStats->numStats; i++ )
				{
				//	I3TRACE( "Physix Stats : ( %s ) \t", pStats->stats[i].name );
				//	I3TRACE( ":cur %d \t", pStats->stats[i].curValue );
				//	I3TRACE( ":max %d\n", pStats->stats[i].maxValue );
				}
				NxScene * pPxScene = getPhysixContext()->getScene();
				//UINT32 nDynamicShapeCount = pPxScene->getNbDynamicShapes();
				UINT32 nPairCount = pPxScene->getNbActorGroupPairs();//pPxScene->getStats2()->stats[0].curValue;//pPxScene->getNbPairs();
				if( nPairCount > 0 )
				{
					NxU32 iter = 0;
					NxActorGroupPair * pPairFlags = (NxActorGroupPair*)i3MemAlloc( sizeof(NxActorGroupPair) * nPairCount);
					pPxScene->getActorGroupPairArray( pPairFlags, nPairCount, iter);
					for( UINT32 i = 0; i < nPairCount; i++)
					{
						//if( pPairFlags[i].isActorPair() )
						{
							I3TRACE( "ActorPairs obj0 : %d, obj1 : %d, flag : %u\n", pPairFlags[i].group0, pPairFlags[i].group1, pPairFlags[i].flags );
						}
					}

					I3MEM_SAFE_FREE_POINTER( pPairFlags);
				}
			}
		}
#endif

	{
		i3Vector::Set( &m_vMouseMoveDelta, pMouse->GetDeltaX(), pMouse->GetDeltaY(), pMouse->GetDeltaZ());

		if( pKey->GetStrokeState( I3I_KEY_NUMPAD7))
		{
			i3Vector::AddY( &m_vMouseMoveDelta, 10.f);
		}
		else if( pKey->GetStrokeState( I3I_KEY_NUMPAD1))
		{
			i3Vector::AddY( &m_vMouseMoveDelta, -10.f);
		}

		i3Vector::Scale( &m_vMouseMoveDelta, &m_vMouseMoveDelta, g_pEnvSet->m_rMouseSensivity);

		if( (g_pCamera != nullptr) && (m_GameKeyMode != GAMEKEY_MODE_MENU))
		{
			i3Vector::Scale( &m_vMouseMoveDelta, &m_vMouseMoveDelta, g_pCamera->getZoomRatio());
		}
	}

	if (m_GameKeyMode == GAMEKEY_MODE_MENU)
	{
		m_nMouseButtonState &= I3I_MOUSE_LBUTTON;
	}
	else
	{
		if( g_pCamera != nullptr && !CGameMaster::i()->IsPaused() )
		{
			g_pCamera->OnInputKeyboard( m_nKeyPress, m_nKeyStroke);

			if( !g_pEnvSet->m_bEnableMouseInvert)
			{
				g_pCamera->OnInputMouse(  &m_vMouseMoveDelta, m_nMouseButtonState);
			}
			else
			{
				VEC3D vTmp;
				i3Vector::Set( &vTmp, getX( &m_vMouseMoveDelta), - getY( &m_vMouseMoveDelta), getX( &m_vMouseMoveDelta));
				g_pCamera->OnInputMouse(  &vTmp, m_nMouseButtonState);
			}
		}
	}

	// OnEvent 방식으로 수정된 입력처리
	// Focus처리는 되어있지 않습니다.
	if(GetState() == STATE_SCENE)// revision 55752 로딩 후 로그인 페이지로 넘어갈 때 강제종료되는 버그 수정
	{
		UINT32 param2;
		REAL32 rDeltaSeconds = m_pViewer->getDeltaTime();
		i3mem::Copy(&param2, &rDeltaSeconds, sizeof(UINT32));


		bool bResutInput = false;

		if(i3UIManager::IsShowConsole())
		{
			//  콘솔창이 보이고있으면 입력처리를 콘솔PBClientContext로 보낸다.
			i3UIManager::OnConsoleEvent(I3_EVT_INPUT, pMng, param2);
			bResutInput = true;
		}
		else
		{

			if (IsMessageBoxEnabled())
			{
				m_pMessageBoxManager->OnEvent(I3_EVT_INPUT, pMng, param2, I3_EVT_CODE_ACTIVATE);
				bResutInput = true;			// 활성화만 되어 있어도 입력값 초기화
			}
			else
			{
				if (g_pGUIRoot)
				{
					// 키보드처리를 다른 객체에 의존하고 있음으로 입력값을 초기화해선 안된다.
					g_pGUIRoot->OnEvent(I3_EVT_INPUT, pMng, param2);
				}

				if (m_pStage && m_pStage->OnEvent(I3_EVT_INPUT, pMng, param2))
				{
					bResutInput = true;		// 처리된 입력이 있는 경우 입력값 초기화
				}
			}
		}

		if (bResutInput)
		{
			// 입력 포커스 관리를 위해 입력값을 초기화한다.
			ResetInput();
		}
	}

	return i3Framework::OnInput();
}

void CGameFramework::addInput(void)
{
	if( m_AutoMouseCount >= g_pConfig->m_nMaxCheckCount)
	{
		// 제일 앞 칸을 비운다.
		memmove( &m_AutoMouseTrack[1], m_AutoMouseTrack, sizeof(REAL32) * (g_pConfig->m_nMaxCheckCount - 1));
	}
	else
	{
		m_AutoMouseCount++;
	}

	m_AutoMouseTrack[0] = m_pAutoMouseTimer->GetDeltaSec();

	if( m_AutoMouseTrack[0] > 0.2f)
	{
		m_AutoMouseCount = 1;
	}
}

bool CGameFramework::IsAutoMouse(void)
{
	INT32 i;
	REAL32		diff, first, dispertion;

	if( m_AutoMouseCount < g_pConfig->m_nMaxCheckCount)
		return false;

	first = m_AutoMouseTrack[0];

	dispertion = 0.0f;

	for( i = 1; i < m_AutoMouseCount; i++)
	{
		diff = i3Math::abs( first - m_AutoMouseTrack[i]);

		dispertion += diff;
	}

	dispertion = dispertion / m_AutoMouseCount;

	if( dispertion < g_pConfig->m_fCheckRate)
	{
		m_AutoMouseAccmCount++;
	}

	if( m_AutoMouseAccmCount >= g_pConfig->m_nAutoAccumCount)
	{
		ResetAutoMouse();
		return true;
	}

	return false;
}

void CGameFramework::ResetAutoMouse(void)
{
	m_AutoMouseCount = 0;
	m_AutoMouseAccmCount = 0;
}

void CGameFramework::SetGameKey(INT32 nKeyState, INT32 nKey)
{
	if( nKeyState == 0 )	// Press
	{
		switch( nKey)
		{
			// 이동관련 키
			case GAME_KEY_MAP_LEFT:					SetGamePressKeyMap(GAME_KEY_MOVE_LEFT);		break;
			case GAME_KEY_MAP_RIGHT: 				SetGamePressKeyMap(GAME_KEY_MOVE_RIGHT);			break;
			case GAME_KEY_MAP_FORWARD: 				SetGamePressKeyMap(GAME_KEY_MOVE_FORWARD);			break;
			case GAME_KEY_MAP_BACKWARD:				SetGamePressKeyMap(GAME_KEY_MOVE_BACKWARD);			break;
			// Action
			case GAME_KEY_MAP_JUMP: 				SetGamePressKeyMap(GAME_KEY_ACTION_JUMP);				break;
			case GAME_KEY_MAP_FOOTSTEP: 			SetGamePressKeyMap(GAME_KEY_ACTION_FOOTSTEP);			break;
			case GAME_KEY_MAP_CROUCH: 				SetGamePressKeyMap(GAME_KEY_ACTION_CROUCH);				break;

			case GAME_KEY_MAP_ATTACK:				SetGamePressKeyMap(GAME_KEY_ACTION_ATTACK);				break;

			case GAME_KEY_MAP_SUBFUNC:				SetGamePressKeyMap(GAME_KEY_ACTION_SUBFUNC);			break;
			case GAME_KEY_MAP_RELOAD: 				SetGamePressKeyMap(GAME_KEY_ACTION_RELOAD);				break;
			case GAME_KEY_MAP_SPECIALACTION:		SetGamePressKeyMap(GAME_KEY_ACTION_SPECIAL);			break;
			case GAME_KEY_MAP_TURNBACK:				SetGamePressKeyMap(GAME_KEY_ACTION_TURNBACK);	break;
			case GAME_KEY_MAP_DROP:					SetGamePressKeyMap(GAME_KEY_ACTION_DROP);				break;
			case GAME_KEY_MAP_WEAPON_EXTENSION:		SetGamePressKeyMap(GAME_KEY_ACTION_WEAPON_EXTENSION);				break;
			// 무기교체 관련키
			case GAME_KEY_MAP_CHANGEWEAPON:			SetGamePressKeyMap(GAME_KEY_CHANGEWEAPON);			break;
			case GAME_KEY_MAP_CHANGEWEAPON_1:		SetGamePressKeyMap(GAME_KEY_CHANGEWEAPON1);			break;
			case GAME_KEY_MAP_CHANGEWEAPON_2: 		SetGamePressKeyMap(GAME_KEY_CHANGEWEAPON2);			break;
			case GAME_KEY_MAP_CHANGEWEAPON_3: 		SetGamePressKeyMap(GAME_KEY_CHANGEWEAPON3);			break;
			case GAME_KEY_MAP_CHANGEWEAPON_4:		SetGamePressKeyMap(GAME_KEY_CHANGEWEAPON4);			break;
			case GAME_KEY_MAP_CHANGEWEAPON_5: 		SetGamePressKeyMap(GAME_KEY_CHANGEWEAPON5);			break;
			case GAME_KEY_MAP_CHANGEWEAPON_6: 		SetGamePressKeyMap(GAME_KEY_CHANGEWEAPON6);			break;
			case GAME_KEY_MAP_CHANGEWEAPON_QUICK: 	SetGamePressKeyMap(GAME_KEY_CHANGEWEAPON_QUICK);		break;
			case GAME_KEY_MAP_CHANGEWEAPON_PREV: 	SetGamePressKeyMap(GAME_KEY_CHANGEWEAPON_NEXT);		break;
			case GAME_KEY_MAP_CHANGEWEAPON_NEXT: 	SetGamePressKeyMap(GAME_KEY_CHANGEWEAPON_PRE);		break;
			case GAME_KEY_MAP_CHANGEWEAPON_7:		SetGamePressKeyMap(GAME_KEY_CHANGEWEAPON7);			break;
			case GAME_KEY_MAP_CHANGEWEAPON_8: 		SetGamePressKeyMap(GAME_KEY_CHANGEWEAPON8);			break;
			case GAME_KEY_MAP_CHANGEWEAPON_9: 		SetGamePressKeyMap(GAME_KEY_CHANGEWEAPON9);			break;
			// 기타 관련 키
			case GAME_KEY_MAP_SHOWSCORE: 			SetGamePressKeyMap(GAME_KEY_CMD_SHOWSCORE);				break;
			case GAME_KEY_MAP_CHAT:					SetGamePressKeyMap(GAME_KEY_CMD_CHAT);						break;
			case GAME_KEY_MAP_VOICECHAT: 			SetGamePressKeyMap(GAME_KEY_CMD_VOICECHAT);				break;
			case GAME_KEY_MAP_RADIOMESSAGE1: 		SetGamePressKeyMap(GAME_KEY_CMD_RADIO1);					break;
			case GAME_KEY_MAP_RADIOMESSAGE2: 		SetGamePressKeyMap(GAME_KEY_CMD_RADIO2);					break;
			case GAME_KEY_MAP_RADIOMESSAGE3: 		SetGamePressKeyMap(GAME_KEY_CMD_RADIO3);					break;
			case GAME_KEY_MAP_MOUSE_SENS_UP:		SetGamePressKeyMap(GAME_KEY_CMD_MOUSESENE_UP);				break;
			case GAME_KEY_MAP_MOUSE_SENS_DOWN: 		SetGamePressKeyMap(GAME_KEY_CMD_MOUSESENE_DOWN);			break;
			case GAME_KEY_MAP_SCREENSHOT: 			break;
			case GAME_KEY_MAP_VIDEOCAPTURE: 		SetGamePressKeyMap(GAME_KEY_FUNC_VIDEOCAPTURE);		break;

			//case GAME_KEY_MAP_TOGGLE_MAP: 			SetGamePressKeyMap(GAME_KEY_FUNC_TOGGLEMAP);		break;
			case GAME_KEY_MAP_MINIMAP_ENLARGE: 		SetGamePressKeyMap(GAME_KEY_PLUS);					break;
			case GAME_KEY_MAP_MINIMAP_REDUCTION: 	SetGamePressKeyMap(GAME_KEY_MINUS);					break;
			case GAME_KEY_MAP_CHAT_ALL: 			SetGamePressKeyMap(GAME_KEY_FUNC_F2);				break;
			case GAME_KEY_MAP_CHAT_TEAM: 			SetGamePressKeyMap(GAME_KEY_FUNC_F3);				break;
			case GAME_KEY_MAP_CHAT_CLAN:			SetGamePressKeyMap(GAME_KEY_FUNC_F4);				break;
			case GAME_KEY_MAP_CHAT_HISTORY: 		SetGamePressKeyMap(GAME_KEY_FUNC_CHATSCROLL);		break;
			// 야전 상점
			case GAME_KEY_MAP_INGAME_SHOP:			SetGamePressKeyMap(GAME_KEY_INGAME_SHOP);			break;			
		}
	}
	else				// Stroke
	{
		switch( nKey)
		{
			// 이동관련 키
			case GAME_KEY_MAP_LEFT:					SetGameStrokeKeyMap(GAME_KEY_MOVE_LEFT);			break;
			case GAME_KEY_MAP_RIGHT: 				SetGameStrokeKeyMap(GAME_KEY_MOVE_RIGHT);			break;
			case GAME_KEY_MAP_FORWARD: 				SetGameStrokeKeyMap(GAME_KEY_MOVE_FORWARD);			break;
			case GAME_KEY_MAP_BACKWARD:				SetGameStrokeKeyMap(GAME_KEY_MOVE_BACKWARD);		break;
			// Action
			case GAME_KEY_MAP_JUMP: 				SetGameStrokeKeyMap(GAME_KEY_ACTION_JUMP);					break;
			case GAME_KEY_MAP_FOOTSTEP: 			SetGameStrokeKeyMap(GAME_KEY_ACTION_FOOTSTEP);				break;
			case GAME_KEY_MAP_CROUCH: 				SetGameStrokeKeyMap(GAME_KEY_ACTION_CROUCH);				break;
			case GAME_KEY_MAP_ATTACK:
				addInput();

#if defined ( ENABLE_DETECT_AUTO_MOUSE)
				if( IsAutoMouse())
				{
					//I3TRACE( "Auto Mouse !!!!!!!!!!!!!!!!!!!!!!!!\n");
					if( i3::same_of<CStageBattle*>(GetCurrentStage()))
					{
						CStageBattle * pStage = (CStageBattle*)GetCurrentStage();
						if( pStage->getHud() != nullptr) pStage->getHud()->SetActivateHackWarnning();
					}
				}
#endif

				SetGameStrokeKeyMap(GAME_KEY_ACTION_ATTACK);
				break;
			case GAME_KEY_MAP_SUBFUNC:				SetGameStrokeKeyMap(GAME_KEY_ACTION_SUBFUNC);				break;
			case GAME_KEY_MAP_RELOAD: 				SetGameStrokeKeyMap(GAME_KEY_ACTION_RELOAD);				break;
			case GAME_KEY_MAP_SPECIALACTION:		SetGameStrokeKeyMap(GAME_KEY_ACTION_SPECIAL);		break;
			case GAME_KEY_MAP_TURNBACK:				SetGameStrokeKeyMap(GAME_KEY_ACTION_TURNBACK);		break;
			case GAME_KEY_MAP_DROP:					SetGameStrokeKeyMap(GAME_KEY_ACTION_DROP );			break;
			case GAME_KEY_MAP_WEAPON_EXTENSION:		SetGameStrokeKeyMap(GAME_KEY_ACTION_WEAPON_EXTENSION );			break;
			// 무기교체 관련키
			case GAME_KEY_MAP_CHANGEWEAPON:			SetGameStrokeKeyMap( GAME_KEY_CHANGEWEAPON);			break;
			case GAME_KEY_MAP_CHANGEWEAPON_1:		SetGameStrokeKeyMap(GAME_KEY_CHANGEWEAPON1);		break;
			case GAME_KEY_MAP_CHANGEWEAPON_2: 		SetGameStrokeKeyMap(GAME_KEY_CHANGEWEAPON2);		break;
			case GAME_KEY_MAP_CHANGEWEAPON_3: 		SetGameStrokeKeyMap(GAME_KEY_CHANGEWEAPON3);		break;
			case GAME_KEY_MAP_CHANGEWEAPON_4:		SetGameStrokeKeyMap(GAME_KEY_CHANGEWEAPON4);		break;
			case GAME_KEY_MAP_CHANGEWEAPON_5: 		SetGameStrokeKeyMap(GAME_KEY_CHANGEWEAPON5);		break;
			case GAME_KEY_MAP_CHANGEWEAPON_6: 		SetGameStrokeKeyMap(GAME_KEY_CHANGEWEAPON6);		break;
			case GAME_KEY_MAP_CHANGEWEAPON_QUICK: 	SetGameStrokeKeyMap(GAME_KEY_CHANGEWEAPON_QUICK);	break;
			case GAME_KEY_MAP_CHANGEWEAPON_PREV: 	SetGameStrokeKeyMap(GAME_KEY_CHANGEWEAPON_PRE);		break;
			case GAME_KEY_MAP_CHANGEWEAPON_NEXT: 	SetGameStrokeKeyMap(GAME_KEY_CHANGEWEAPON_NEXT);	break;
			case GAME_KEY_MAP_CHANGEWEAPON_7:		SetGameStrokeKeyMap(GAME_KEY_CHANGEWEAPON7);		break;
			case GAME_KEY_MAP_CHANGEWEAPON_8: 		SetGameStrokeKeyMap(GAME_KEY_CHANGEWEAPON8);		break;
			case GAME_KEY_MAP_CHANGEWEAPON_9: 		SetGameStrokeKeyMap(GAME_KEY_CHANGEWEAPON9);		break;
			// 기타 관련 키
			case GAME_KEY_MAP_SHOWSCORE: 			SetGameStrokeKeyMap(GAME_KEY_CMD_SHOWSCORE);			break;
			case GAME_KEY_MAP_CHAT: 				SetGameStrokeKeyMap(GAME_KEY_CMD_CHAT);					break;
			case GAME_KEY_MAP_VOICECHAT: 			SetGameStrokeKeyMap(GAME_KEY_CMD_VOICECHAT);			break;
			case GAME_KEY_MAP_RADIOMESSAGE1: 		SetGameStrokeKeyMap(GAME_KEY_CMD_RADIO1);		break;
			case GAME_KEY_MAP_RADIOMESSAGE2: 		SetGameStrokeKeyMap(GAME_KEY_CMD_RADIO2);		break;
			case GAME_KEY_MAP_RADIOMESSAGE3: 		SetGameStrokeKeyMap(GAME_KEY_CMD_RADIO3);		break;
			case GAME_KEY_MAP_MOUSE_SENS_UP:		SetGameStrokeKeyMap(GAME_KEY_CMD_MOUSESENE_UP);			break;
			case GAME_KEY_MAP_MOUSE_SENS_DOWN: 		SetGameStrokeKeyMap(GAME_KEY_CMD_MOUSESENE_DOWN);		break;
			case GAME_KEY_MAP_SCREENSHOT: 			m_bEnableScreenShot = true;								break;
			case GAME_KEY_MAP_VIDEOCAPTURE: 		SetGameStrokeKeyMap(GAME_KEY_FUNC_VIDEOCAPTURE);		break;

			//case GAME_KEY_MAP_TOGGLE_MAP: 			SetGameStrokeKeyMap(GAME_KEY_FUNC_TOGGLEMAP);		break;
			case GAME_KEY_MAP_MINIMAP_ENLARGE: 		SetGameStrokeKeyMap(GAME_KEY_PLUS);					break;
			case GAME_KEY_MAP_MINIMAP_REDUCTION: 	SetGameStrokeKeyMap(GAME_KEY_MINUS);				break;
			case GAME_KEY_MAP_CHAT_ALL: 			SetGameStrokeKeyMap(GAME_KEY_FUNC_F2);				break;
			case GAME_KEY_MAP_CHAT_TEAM: 			SetGameStrokeKeyMap(GAME_KEY_FUNC_F3);				break;
			case GAME_KEY_MAP_CHAT_CLAN:			SetGameStrokeKeyMap(GAME_KEY_FUNC_F4);				break;
			case GAME_KEY_MAP_CHAT_HISTORY: 		SetGameStrokeKeyMap(GAME_KEY_FUNC_CHATSCROLL);		break;
			// 야전 상점
			case GAME_KEY_MAP_INGAME_SHOP:			SetGameStrokeKeyMap(GAME_KEY_INGAME_SHOP);			break;
			case GAME_KEY_MAP_SPECIAL_MISSION:		SetGameStrokeKeyMap(GAME_KEY_SPECIAL_MISSION);		break;
		}
	}
}
void CGameFramework::SetGamePressKeyMap( UINT64 map)
{
	if(map & GAME_KEY_ACTION_ATTACK && m_nKeyStroke & GAME_KEY_ACTION_DROP) {
		return;
	}

	m_nKeyPress |= map;
}

void CGameFramework::SetGameStrokeKeyMap( UINT64 map)
{
	if(map & GAME_KEY_ACTION_DROP && m_nKeyPress & GAME_KEY_ACTION_ATTACK) {
		return;
	}

	if(map & GAME_KEY_ACTION_ATTACK && m_nKeyStroke & GAME_KEY_ACTION_DROP) {
		return;
	}

	m_nKeyStroke |=  map;

}

void CGameFramework::SetSysPressKeyMap(UINT64 map)
{
	SETBIT(m_nSysKeyPress, map);
}

void CGameFramework::SetSysStrokeKeyMap(UINT64 map)
{
	SETBIT(m_nSysKeyState, map);
}

bool CGameFramework::OnEvent( UINT32 event, i3ElementBase * pObj, UINT32 param, I3_EVT_CODE code)
{
	VEC3D vPos;

	if( code == I3_EVT_CODE_ACTIVATE)
	{
		switch( event )
		{
		case I3_EVT_ACTION :
			if( i3::same_of<i3ActionExplosion*>(pObj))
			{
				i3Object * pGameObj = (i3Object *) param;
				i3ActionExplosion * pAction = (i3ActionExplosion *) pObj;

				if( pAction->getAttachEnable())
				{
					i3SceneObject * pSceneObj = pGameObj->getSceneObject();
					{
						MATRIX * pMtx = pSceneObj->FindCacheMatrixByName( pAction->getAttachBoneName());
						if( pMtx != nullptr)
						{
							i3Vector::Copy( &vPos, i3Matrix::GetPos( pMtx));
						}
					}
				}
				else
				{
					i3Vector::Copy( &vPos, pGameObj->GetPos() );
				}

				i3Vector::Add( &vPos, &vPos, pGameObj->getShapeCenterPos() );
				if( GetCurrentStage())
				{
					// KEYWORD : WEAPON_M197
					if( pGameObj->getCurrentHP() == 0.f)
					{
						// 오브젝트에 탑승자는 사망토록 처리.
						if( i3::same_of<CGameObjectRidingHelicopter*>(pGameObj))
							((CGameObjectRidingHelicopter*)pGameObj)->ExplosionObject( pGameObj->getEventSender());
						else if( i3::same_of<CGameObjectRespawnHelicopter*>(pGameObj))
							((CGameObjectRespawnHelicopter*)pGameObj)->ExplosionObject( pGameObj->getEventSender());
					}

					// revision 62269 오브젝트 폭발 컬리전을 여기서 하지 않는다.
					//obj_explosion::i()->call(pGameObj, &vPos, pAction->getRange(), pAction->getForce(), pAction->getDamage(), pAction->getSlope());

				}

				return true;
			}
			break;

		default :
			// UI 에 event를 보내자
			if( GetCurrentStage())
			{
				GetCurrentStage()->OnEvent( event, pObj, param, code);
			}
			break;

		}
	}

	return i3Framework::OnEvent( event, pObj, param, code);
}



void CGameFramework::OnUpdate( REAL32 rDeltaSeconds)
{
	i3_prof_func();

	if(g_pFramework == nullptr)
		return;
#if defined(WINDOW_MODE_DISABLE)
	PB_VM_S
	if (g_DidThisInactive)
	{
		// GOTO_EXIT_STAGE();
		GameEventSender::i()->SetEvent( EVENT_GAME_END ); // 준영. 로그아웃 요청 패킷 전송
	}
	PB_VM_E
#endif

	// 핵실드에 콜백에 의한 종료 처리에만 사용되는 변수입니다.
	if ( m_bExitGame )							
	{
		m_fExitDeltaTime += rDeltaSeconds;
		if ( m_fExitDeltaTime >= 10.f )
		{
			GOTO_EXIT_STAGE();
		}
	}

	if( m_pLuaManager != nullptr) {
		m_pLuaManager->UpdateLuaMgr(rDeltaSeconds);
	}

	// Time Sync Func
	hd::check_elapsedsynctimer::i()->Update(rDeltaSeconds);
	//

	i3Framework::OnUpdate( rDeltaSeconds);

#if (defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)) && !defined( SALES_BUILD)
	if ( m_bVersionInfo)
	{
		PrintUIDebug();
		UpdatePolygonCount();
		UpdatePrintInfoText();
		UpdateFPS();
	}
#endif

	// nProtect의 구동 확인
	{
		m_timeNP += rDeltaSeconds;

		// 10초에 한번씩..
		if( m_timeNP > 10.0f)
		{
			m_timeNP = 0.0f;

			
#if defined (_NPROTECT_)
			if( nProtect::CheckAlive() == false)
			{
				//"nProtect module이 정상 작동하지 않고 있습니다.
				GameUI::MsgBox( MSG_TYPE_ERROR, GAME_RCSTRING("STR_TBL_GLOBALDEF_NPROTECT_NO_WORK"), nullptr, EXIT_CALLBACK(nullptr,true), (void*)g_pFramework);
			
				// 게임 종료
			}
#endif

		}
	}

	
	switch( GetState())
	{
		
	case STATE_FADEIN :
		{
			if(m_StateAfterFadeIn == STATE_LOGO1_START)
			{
				if( m_pUseMovie)
					m_pAttrSetLogo[0]->RemoveFlag(I3_NODEFLAG_DISABLE);
				m_pAttrSetLogo[1]->AddFlag(I3_NODEFLAG_DISABLE);
				m_pAttrSetLogo[2]->AddFlag(I3_NODEFLAG_DISABLE);
				m_pAttrSetLogo[3]->AddFlag(I3_NODEFLAG_DISABLE);
			}
			else if(m_StateAfterFadeIn == STATE_LOGO2_START)
			{
				if( m_pUseMovie)
					m_pAttrSetLogo[0]->AddFlag(I3_NODEFLAG_DISABLE);
				m_pAttrSetLogo[1]->RemoveFlag(I3_NODEFLAG_DISABLE);
				m_pAttrSetLogo[2]->AddFlag(I3_NODEFLAG_DISABLE);
				m_pAttrSetLogo[3]->AddFlag(I3_NODEFLAG_DISABLE);
			}
			else if(m_StateAfterFadeIn == STATE_LOGO3_START)
			{
				if( m_pUseMovie)
					m_pAttrSetLogo[0]->AddFlag(I3_NODEFLAG_DISABLE);
				m_pAttrSetLogo[1]->AddFlag(I3_NODEFLAG_DISABLE);
				m_pAttrSetLogo[2]->RemoveFlag(I3_NODEFLAG_DISABLE);
				m_pAttrSetLogo[3]->AddFlag(I3_NODEFLAG_DISABLE);
			}
			else if(m_StateAfterFadeIn == STATE_LOGO4_START)
			{
				if ( LocaleValue::Enable("EnableRatingLogo"))
				{
					m_pAttrSetRatingLogo->RemoveFlag(I3_NODEFLAG_DISABLE);
				}
				else
				{
					if( m_pUseMovie)
						m_pAttrSetLogo[0]->AddFlag(I3_NODEFLAG_DISABLE);
					m_pAttrSetLogo[1]->AddFlag(I3_NODEFLAG_DISABLE);
					m_pAttrSetLogo[2]->AddFlag(I3_NODEFLAG_DISABLE);
					m_pAttrSetLogo[3]->RemoveFlag(I3_NODEFLAG_DISABLE);
				}
			}
			else if(m_StateAfterFadeIn == STATE_COMMON_LOAD_START)
			{
				//I3PRINTLOG(I3LOG_NOTICE,"* Movie FadeIn ==STATE_COMMON_LOAD_START.1 ");
				if ( LocaleValue::Enable("EnableRatingLogo"))
				{
					m_pAttrSetRatingLogo->AddFlag(I3_NODEFLAG_DISABLE);
				}
				else
				{
					if( m_pUseMovie)
						m_pAttrSetLogo[0]->AddFlag(I3_NODEFLAG_DISABLE);
					m_pAttrSetLogo[1]->AddFlag(I3_NODEFLAG_DISABLE);
					m_pAttrSetLogo[2]->AddFlag(I3_NODEFLAG_DISABLE);
					m_pAttrSetLogo[3]->AddFlag(I3_NODEFLAG_DISABLE);
				}
			}

		}
		break;
		
	case STATE_LOGO1_START :
		{
			if(m_pUseMovie)
			{
				m_rStateTime = 0.f;
				m_pMovie->SetPlay();
				SetState(STATE_LOGO1);
			}
			else
				m_StateAfterFadeIn = STATE_LOGO2_START;

		}
		break;
	case STATE_LOGO1 :
		{
			m_pSpriteLogo[0]->SetTexture( m_pMovie->GetTexture() );
			m_pMovie->DoRender();
			if(m_pMovie->CheckMovieStatus() == EC_COMPLETE)
			{	
				I3PRINTLOG(I3LOG_NOTICE,"* Movie EC_COMPLETE.1 ");
				//I3_SAFE_RELEASE( m_pMovie);
				SetState(STATE_FADEOUT_START);
				m_StateAfterFadeOut = STATE_FADEIN_START;
				m_StateAfterFadeIn = STATE_LOGO2_START;
			}
		}
		break;
	case STATE_LOGO2_START :
		{
			m_rStateTime = 0.f;
			SetState(STATE_LOGO2);
			
		}
		break;
	case STATE_LOGO2 :
		{
			m_rStateTime += rDeltaSeconds;
			if(m_rStateTime > 2.f)
			{
				SetState(STATE_FADEOUT_START);
				m_StateAfterFadeOut = STATE_FADEIN_START;
				m_StateAfterFadeIn = STATE_LOGO3_START;
				
			}
		}
		break;
	case STATE_LOGO3_START :
		{
			m_rStateTime = 0.f;
			SetState(STATE_LOGO3);
			
		}
		break;
	case STATE_LOGO3 :
		{
			m_rStateTime += rDeltaSeconds;
			if(m_rStateTime > 2.f)
			{
				SetState(STATE_FADEOUT_START);
				m_StateAfterFadeOut = STATE_FADEIN_START;
				m_StateAfterFadeIn = STATE_LOGO4_START;
			}
		}
		break;
	case STATE_LOGO4_START :
		{
			m_rStateTime = 0.f;
			SetState(STATE_LOGO4);
			
		}
		break;
	case STATE_LOGO4 :
		{
			m_rStateTime += rDeltaSeconds;
			if(m_rStateTime > 3.f)
			{
				SetState(STATE_FADEOUT_START);
				m_StateAfterFadeOut = STATE_FADEIN_START;
				m_StateAfterFadeIn = STATE_COMMON_LOAD_START;
			}
		}
		break;

	case STATE_LOAD_START:
		break;

	case STATE_LOADING :
		break;

	case STATE_LOADING_GAMEDATA:
		if(s_bNew_StageLoading == true)
		{
			m_rLoadingPercent += (rDeltaSeconds * 0.333f);
			if(m_rLoadingPercent >= 1.0f)
				m_rLoadingPercent = 1.0f;

			m_pHUDLoading->UpdateProgress(m_rLoadingPercent);
		}
		else
		{
			m_pGameSupportSet->UpdateLoadingProgress( rDeltaSeconds);
		}
		break;

	case STATE_LOAD_END :
		I3TRACE( "i3Framework::OnUpdate() - STATE_LOAD_END Enter\n");
		I3TRACE( "i3Framework::OnUpdate() - STATE_LOAD_END Enter\n");
		break;

	case STATE_FADEIN_START :		I3PRINTLOG(I3LOG_NOTICE, "i3Framework::OnUpdate() - STATE_FADEIN_START Enter");
		// 로딩스크린을 해재합니다.
		if(s_bNew_StageLoading == true)
		{
			m_pHUDLoading->OnExitEnd();
		}
		else
			m_pGameSupportSet->ReleaseLoadingSpr();
		I3PRINTLOG(I3LOG_NOTICE, "i3Framework::OnUpdate() - STATE_FADEIN_START Leave");
		break;

	case STATE_SCENE:
		if( g_pSndMng != nullptr)
		{
			g_pSndMng->Drive( rDeltaSeconds);

#if ENABLE_UNIT_TEST
			UnitTest::update_unit_test(rDeltaSeconds);
#endif
		}
		break;
	case STATE_SCENE_END:
		if( g_pSndMng != nullptr)
		{
			g_pSndMng->Drive( rDeltaSeconds, true);
		}
		SetNightVisionEnable( false);
		break;
	default:
		break;
	}

	m_nSysKeyState = 0;
	m_nSysKeyPress = 0;

	if( m_bEnableScreenShot)
	{
		if( GetStageBattle() != nullptr)
		{
			if( BattleServerContext::i()->IsP2PHost())
			{
				if( m_fScreenShotDeltaTime == 0.0f)
				{
					m_pGameSupportSet->SaveScreenShot();
				}

				m_fScreenShotDeltaTime += rDeltaSeconds;
				if( m_fScreenShotDeltaTime > 3.0f)
				{
					m_fScreenShotDeltaTime = 0.0f;
					m_bEnableScreenShot = false;
				}
			}
			else
			{
				m_fScreenShotDeltaTime = 0.0f;
				m_pGameSupportSet->SaveScreenShot();
				m_bEnableScreenShot = false;
			}
		}
		else
		{
			m_fScreenShotDeltaTime = 0.0f;
			m_pGameSupportSet->SaveScreenShot();
			m_bEnableScreenShot = false;
		}

		i3InputKeyboard* pKeyboard = m_pViewer->GetInputDeviceManager()->GetKeyboard();

		{
			// 스크린샷을 저장할때 게임프로세스가 잠시 멈추기때문에 윈도우 MW_KEYUP을 받지 못한다.
			INT32 rv = pKeyboard->FindVKByI3IKey((I3I_KEY)g_pEnvSet->m_KeyMapArray[GAME_KEY_MAP_SCREENSHOT]._nState);
			if (-1 != rv)	m_pViewer->OnKeyUp(rv);
		}
	}

	if( g_pFramework->getKeyboard()->GetStrokeState( I3I_KEY_SLASH))
	{
		// Effect Root Dump
		I3TRACE(">>>>> Effect Root Dump\n");
		//I3_SG_DUMP( GetLayerRoot( "EffectRoot"));
	}

	// MessageBox Test Start!!!
	i3InputKeyboard * pKey = g_pFramework->getKeyboard();
	if( pKey != nullptr)
	{
#if defined I3_DEBUG//디버그 키
		if( pKey->GetPressState( I3I_KEY_LCTRL))
		{
			if( pKey->GetStrokeState( I3I_KEY_F5))
			{
				if( GetViewer()->GetRenderContext()->GetRenderMode() == I3G_RENDER_LINE)
				{
					GetViewer()->GetRenderContext()->SetRenderMode( I3G_RENDER_LINE);
				}
				else
				{
					GetViewer()->GetRenderContext()->SetRenderMode( I3G_RENDER_POLYGON);
				}
			}
		}
#endif

#if !defined( NC_BUILD )
		if( pKey->GetPressState(I3I_KEY_LCTRL) && pKey->GetPressState(I3I_KEY_LSHIFT) )
		{
			if( pKey->GetStrokeState( I3I_KEY_F12) )
			{
				std::ostringstream stream;

				stream.width(0);
				stream.fill(' ');

				if (std::ios::fmtflags(0))
					stream.setf(std::ios::fmtflags(0));

				stream << g_pD3DDevice->GetAvailableTextureMem();

				I3PRINTLOG(I3LOG_NOTICE, "*****************************************************************************" );
				I3PRINTLOG(I3LOG_NOTICE, ">>>>>AvailableTextureMem %s", stream.str().c_str());
				I3PRINTLOG(I3LOG_NOTICE, "*****************************************************************************" );
			}
		}
#endif
//#endif I3_DEBUG//디버그 키
	}

	if (g_pCharaManager)
		g_pCharaManager->OnUpdate(rDeltaSeconds);
	if( PBConsoleManager::getInstance() )
		PBConsoleManager::getInstance()->RunCommand(rDeltaSeconds);

	if (Cheatkey::Context::i())
		Cheatkey::Context::i()->OnUpdate(rDeltaSeconds);

	if (m_bChatLimitState)
		m_ChatLimitSec.Update();
}

bool CGameFramework::_IsMouseClip( bool bActive)
{
	if( GetViewer()->GetRenderContext()->GetVideoInfo()->m_bFullScreen)
		return true;

	if( bActive == false)
	{
		return false;
	}

	if (IsBattleStage())
	{
		if( ::GetFocus() != g_hWnd || m_GameKeyMode == GAMEKEY_MODE_MENU)
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}
}

bool CGameFramework::_IsMouseShow( bool bActive)
{
	if( bActive == false)
	{
		return false;
	}

	if (IsBattleStage())
	{
		//if( ::GetFocus() != g_hWnd || m_GameKeyMode == GAMEKEY_MODE_MENU || (m_pMessageBoxManager != NULL && m_pMessageBoxManager->IsEnabled()))
		if( m_GameKeyMode == GAMEKEY_MODE_MENU || (m_pMessageBoxManager != nullptr && m_pMessageBoxManager->IsEnabled()))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return true;
	}
}

void CGameFramework::SetGameCursor( bool bActive)
{
	//I3TRACE("SET GAME CURSOR : %d\n", bActive);
	//GetCursorInfo를 call하기전에 cbsize를 꼭 sizeof( CURSORINFO)로 셋해주어야한다.
	CURSORINFO curInfo;
	curInfo.cbSize = sizeof( CURSORINFO);

	GetCursorInfo( &curInfo);

 	bool	bShow	= _IsMouseShow( bActive);
	bool	bClip	= _IsMouseClip( bActive);

	if( bClip)
	{
		RECT rt;
		POINT pt1, pt2;

		// Theme 및 Windows의 version에 따라 테두리의 크기 등이 다를 수 있기
		// 때문에, 실질적인 Client Area 영역만을 Clip Cursor하도록 한다.
		// GetWindowRect()로 구해진 영역은 Border, Caption들을 모두 포함한 영역이기
		// 때문에 문제가 됨.
		GetClientRect( g_pViewer->GetWindowHandle(), &rt);

		pt1.x = rt.left;		pt1.y = rt.top;
		pt2.x = rt.right;		pt2.y = rt.bottom;

		ClientToScreen( g_pViewer->GetWindowHandle(), &pt1);
		ClientToScreen( g_pViewer->GetWindowHandle(), &pt2);

		rt.left = pt1.x;		rt.top = pt1.y;
		rt.right = pt2.x;		rt.bottom = pt2.y;

		if( !ClipCursor( &rt))
		{
			I3TRACE( "Clip Fail\n");
		}
	}
	else
	{
		ClipCursor( nullptr);
	}


	INT32 sCnt = 0;
	if( bShow)
	{
		if( curInfo.flags != CURSOR_SHOWING)
		{
			sCnt = ShowCursor( true);

			if( sCnt > 1) ShowCursor( false);
		}
	}
	else
	{
		if( curInfo.flags == CURSOR_SHOWING)
		{
			sCnt = ShowCursor( false);

			if( sCnt < -1) ShowCursor( true);
		}
	}
}

void  CGameFramework::CreateGameFont( void)
{
	
}


bool CGameFramework::_JumpNextStage( const char* szStageName, bool bPush )
{
	for( INT32 i = 0 ; i < getStageInfoCount() ; i++)
	{
		i3StageInfo * pInfo = m_StageInfoList[i];
		if( i3::generic_is_iequal( pInfo->GetName(), szStageName) )
		{
			((i3Framework*)this)->JumpStage((i3StageInfo*) pInfo, bPush);
			I3PRINTLOG(I3LOG_NOTICE,">> JumpStage() %s ", szStageName);
			return true;
		}
	}

	//실패했을때 아무런 메시지가 없어서 추가했습니다.
	I3PRINTLOG(I3LOG_NOTICE,">> Fail JumpStage()");
	I3PRINTLOG(I3LOG_NOTICE,"%s <-Fail JumpStage. Please check the /Config/BC.i3Game file", szStageName);

	return false;
}

void  CGameFramework::GotoNextStage( bool bPush)
{
	i3StageInfo* pInfo = getStageInfo( GetCurrentStageIndex());
	if( pInfo != nullptr && pInfo->getNextStage() != nullptr)
	{
		INT32 nStageIndex = GetStageIndex(pInfo->getNextStage());

		JumpStage( nStageIndex, bPush);
	}
}

void  CGameFramework::GotoNextStage( const char* szStageName, bool bPush, i3StageInfo* pPrevInfo)
{
	I3PRINTLOG(I3LOG_NOTICE,"[ENTER] GotoNextStage( %s )", szStageName);

	_JumpNextStage( szStageName, bPush );

	I3PRINTLOG(I3LOG_NOTICE,"[LEAVE] GotoNextStage( %s )", szStageName);
}

bool  CGameFramework::MoveNextStage( UINT32 nStageID )
{
	i3::string sstageName;
	i3::rc_wstring wsstageName = CStageInfoUtil::GetMapData((UINT32)nStageID)->getCodeName();
	i3::utf16_to_mb(wsstageName, sstageName);

	I3PRINTLOG(I3LOG_NOTICE,"[ENTER] MoveNextStage( %s )", sstageName.c_str());

	bool	rv	= _JumpNextStage(sstageName.c_str(), false );

	if( rv )
		BattleServerContext::i()->MoveStageStart( nStageID );

	I3PRINTLOG(I3LOG_NOTICE,"[LEAVE] MoveNextStage( %s )", sstageName.c_str());

	return rv;
}

void		CGameFramework::SetNotice_Temporary(const i3::rc_wstring& wstrMsg)
{
	i3::wstring wstr;
	i3::generic_string_copy(wstr, wstrMsg);

	I3COLOR color;
	color.b = (UINT8)0;
	color.g = (UINT8)0;
	color.r = (UINT8)255;
	color.a = (UINT8)255;

	::ColorString(wstr, color);

	if (IsBattleStage())
	{
		// 화면 상단 중앙에 띄웁니다.
		if (CHud::i() != nullptr)
			CHud::i()->SetMissionNoticeString(wstr, g_pConfigEx->GetGUI().JackpotNoticeLifeTime);
	}
	else
	{
		if (GetUIMainframe())
		{
			GetUIMainframe()->SetSlideNotice_Temporary(wstr);
		}
	}
}

void		CGameFramework::SetNotice_Permanant(const i3::rc_wstring& wstrMsg, INT32 i32RGB)
{
	i3::wstring wstr;
	i3::generic_string_copy(wstr, wstrMsg);

	BYTE* ui32Color = (BYTE*)&i32RGB;

	I3COLOR color;
	color.b = (UINT8)(ui32Color[0]);
	color.g = (UINT8)(ui32Color[1]);
	color.r = (UINT8)(ui32Color[2]);
	color.a = (UINT8)255;

	::ColorString(wstr, color);

	// V3 UI 구조는 배틀In/Out 시 UITopmenu를 삭제 및 생성하기 때문에 따로 보관해두어야 함
	m_wstrPermNotice = wstr;

	if (IsBattleStage())
	{
		// 화면 상단 중앙에 띄웁니다.
		if (CHud::i() != nullptr)
			CHud::i()->SetMissionNoticeString(wstr, 60.0f);
	}
	else
	{
		if (GetUIMainframe())
		{
			GetUIMainframe()->SetSlideNotice_Permanant(wstr);
		}
	}
}

void CGameFramework::setChatNotice(const i3::rc_wstring& wstr_msg, INT32 i32RGB)
{
	i3::generic_string_copy(m_wstrNoticeChat, wstr_msg);
	BYTE* ui32Color = (BYTE*)&i32RGB;

	I3COLOR color;
	color.b = (UINT8)(ui32Color[0]);
	color.g = (UINT8)(ui32Color[1]);
	color.r = (UINT8)(ui32Color[2]);
	color.a = (UINT8)255;

	::ColorString(m_wstrNoticeChat, color);
}

const i3::rc_wstring& CGameFramework::getChatNotice() const
{
	return m_wstrNoticeChat;
}

const i3::rc_wstring& CGameFramework::getTempNotice() const
{
	return m_wstrTempNotice;
}

const i3::rc_wstring& CGameFramework::getPermNotice() const
{
	return m_wstrPermNotice;
}

bool CGameFramework::IsMessageBoxEnabled(void)
{
	return m_pMessageBoxManager->IsEnabled();
}

void	CGameFramework::PreventMessageBoxWhenPhaseChange(void)
{
	m_pMessageBoxManager->PreventMessageBoxWhenPhaseChange();
}

void CGameFramework::CloseGameMessageBox(void)
{
	GameUI::CloseGameMsgBox();
}

void CGameFramework::ResizeMessageBox(void)
{
	m_pMessageBoxManager->OnResize();

#if defined( TEST_AUTOMATION)
	m_pTestAuto->OnSize();
#endif
}

void CGameFramework::SetGameKeyMode( GAMEKEY_MODE_STATE state)
{
	//GlobalFunc::PB_TRACE("SetGameKeyMode %d", state);
	m_GameKeyMode = state;
}

I3_PHYSIX_HIT_RESULT *	CGameFramework::raycastClosest( i3CollideeLine * pLine, INT32 grp)
{
	i3PhysixContext * pCtx = getPhysixContext();
	return pCtx->RaycastClosest( pLine, grp);
}

I3_PHYSIX_HIT_RESULT *	CGameFramework::raycastClosest( i3CollideeLine * pLine, INT32 grp, i3EventReceiver * pTarget)
{
	I3_PHYSIX_HIT_RESULT * pResult;
//	i3PhysixContext * pCtx = getPhysixContext();

	// Group Mask를 이용한 Filtering
	//pCtx->setFilterConstant( (UINT32) pTarget, (UINT32) pTarget);
	//pCtx->setFilterOps( NX_FILTEROP_XOR, NX_FILTEROP_XOR, NX_FILTEROP_XOR);
	//pCtx->setFilterBool( true);

    pResult = getPhysixContext()->RaycastClosest( pLine, grp);

	// 원래 상태로의 복구
	//pCtx->setFilterConstant( 0xFFFFFFFF, 0xFFFFFFFF);
	//pCtx->setFilterOps( NX_FILTEROP_OR, NX_FILTEROP_OR, NX_FILTEROP_OR);
	//pCtx->setFilterBool( true);

	return pResult;
}

i3Node * CGameFramework::OnQueryAttachNode( i3GameRes * pRes)
{
	if( pRes == nullptr)
		return nullptr;

	if( i3::kind_of<i3GameResSceneGraph*>(pRes))
	{
		i3GameResSceneGraph  * pScene = (i3GameResSceneGraph*)pRes;
		i3SgLayer * pLayer	= nullptr;

		if( pScene->getLayer() != -1)
		{
			pLayer = getLayer( pScene->getLayer());
			return (i3Node*)pLayer->getLayerRoot();
		}
		else
		{
			if( i3::same_of<i3GameResChara*>(pScene))
			{
				i3Chara * pC = ((i3GameResChara*)pScene)->getChara();
				if( i3::same_of<CSkillObjectModel*>(pC) )
				{
					GetLayerRoot( "ObjectRoot");
				}
				else
					return GetLayerRoot( "CharaRoot");
			}
			else if( i3::same_of<i3GameResWorld*>(pScene))
			{
				return GetLayerRoot( "WorldRoot");
			}
			else
			{
				return i3Framework::OnQueryAttachNode( pRes);
			}
		}
	}
	else if( i3::kind_of<i3GameResObject*>(pRes))
	{
		return GetLayerRoot( "ObjectRoot");
	}
	else
	{
		i3Node * pRoot;

		if( GetCurrentStage() != nullptr)
		{
			pRoot = GetCurrentStage()->OnQueryAttachNode( pRes);

			if( pRoot != nullptr)
				return pRoot;
		}
	}

	return Get3DRoot();
}

UINT32					CGameFramework::raycast( i3CollideeLine * pLine, INT32 grp)
{
	return getPhysixContext()->Raycast( pLine, grp);
}

UINT32					CGameFramework::raycast( i3CollideeLine * pLine, INT32 grp, i3EventReceiver * pTarget)
{
	UINT32 cnt;

	getPhysixContext()->Raycast( pLine, grp);

	// Group Mask를 이용한 Filtering
	//getPhysixContext()->setFilterConstant( (UINT32) pTarget, (UINT32) pTarget);
	//getPhysixContext()->setFilterOps( NX_FILTEROP_XOR, NX_FILTEROP_XOR, NX_FILTEROP_XOR);
	//getPhysixContext()->setFilterBool( true);

	cnt = getPhysixContext()->Raycast( pLine, grp);

	//
	//getPhysixContext()->setFilterConstant( 0xFFFFFFFF, 0xFFFFFFFF);
	//getPhysixContext()->setFilterOps( NX_FILTEROP_OR, NX_FILTEROP_OR, NX_FILTEROP_OR);
	//getPhysixContext()->setFilterBool( true);

	return cnt;
}

i3::pack::POINT2D CGameFramework::GetCenterPosition(INT32 nDrawWidth,INT32 nDrawHeight)
{
	i3::pack::POINT2D ptCenter = {0, 0};

	if( m_pViewer != nullptr)
	{
		ptCenter.x = m_pViewer->GetViewWidth() / 2;
		ptCenter.y = m_pViewer->GetViewHeight() / 2;

		ptCenter.x -= nDrawWidth / 2;
		ptCenter.y -= nDrawHeight / 2;
	}

	return ptCenter;
}

i3::pack::POINT2D CGameFramework::GetSreenSize(void)
{
	i3::pack::POINT2D ptSize = {0, 0};

	if (m_pViewer)
	{
		ptSize.x = m_pViewer->GetViewWidth();
		ptSize.y = m_pViewer->GetViewHeight();
	}

	return ptSize;
}

//void CGameFramework::LoadNoticeFromFile(void)
//{
//	if( m_bNotictCheckDone) return;
//
//	m_bNotictCheckDone = TRUE;
//
//	WCHAR16 szwReadLine[512] = L"";
//	char filePath[MAX_PATH] = "";
////	char szReadLine[512*2] = {0,};
//
//	i3FileStream file;
//	CFileFind finder;
//
//	//	multi path
//	ConvertLocalePath( filePath, "config/Notice.txt", GetConfigLocale());
//
//	//	must skip if no have file
//	if( !file.isFile(filePath))
//	{
//		I3PRINTLOG(I3LOG_NOTICE, "No have Notice.txt file.");
//		return;
//	}
//
//	if( FALSE == file.Open( filePath, STREAM_READ | STREAM_SHAREREAD) )	//	파일이 없으면 엔진 내부에서 크래시 창 뜸.
//	{
//		return;
//	}
//
//	//	Check unicode
//	{
//		UINT32 rc = file.Read( szwReadLine, sizeof( WCHAR16));
//		if( rc == STREAM_ERR)
//		{
//			I3PRINTLOG(I3LOG_FATAL,  "Notice file load fail !! (%s)", filePath);
//			file.Close();
//			return;
//		}
//
//		szwReadLine[rc] = 0;
//
//		//	여기에 걸리면 스크립트 파일이 유니코드가 아닙니다. 반드시 유니코드 파일이어야 합니다. komet
//		if( !CHECK_UNICODE(szwReadLine[0]))
//		{
//			I3PRINTLOG(I3LOG_FATAL,  "Notice file have not unicode !! (%s)", filePath);
//			file.Close();
//			return;
//		}
//	}
//
//	BOOL bContinue = TRUE;
//	while( bContinue )
//	{
//		UINT32 wBufSize = sizeof( szwReadLine);
//		i3mem::FillZero( szwReadLine, wBufSize);
////		i3mem::FillZero( szReadLine, sizeof(szReadLine));
//
//		// 유니코드로 된 텍스트를 한줄씩 읽습니다.
//		UINT32 wReadSize = file.ReadLineW( szwReadLine, wBufSize);
//
//		if( wReadSize == 0)	continue;
//
//		I3ASSERT( wReadSize < wBufSize);
//
//		// 버젼
//		if( i3::contain_string( szwReadLine, L"[Notice_Version]" ) >= 0 )
//		{
//			m_wstrNoticeVersion.assign( szwReadLine + 16, wReadSize - 16);
//
//			// 초기 1회만 노출시키도록 체크합니다! (파일 비교 체크)
//			{
//				i3FileStream fileForCheck;
//
//				BOOL bWorking = finder.FindFile("Script/Etc/NoticeCheck.log");
//				finder.Close();
//
//				if( bWorking)
//				{
//					if( fileForCheck.Open( "Script/Etc/NoticeCheck.log", STREAM_READ | STREAM_SHAREREAD))
//					{
//						WCHAR bom;
//						fileForCheck.Read( &bom, sizeof( WCHAR16));
//						
//						// i3UIPhaseLobby쪽의 파일 쓰기쪽은 유니코드로변환했고, 읽기쪽은 호환성을 염두에 두고 2개버전으로 분기했다. (2014.07.21.수빈)
//						// TODO : 호환성이 필요없다면, 한쪽은 제거하도록 한다...(2014.07.21.수빈)
//						if( !CHECK_UNICODE(bom))
//						{
//							char szVersionText[512] = {0,};
//							fileForCheck.SetPosition(0, STREAM_BEGIN);
//							fileForCheck.ReadLine( szVersionText, 512 );
//
//							i3::stack_string strTemp;
//
//							i3::sprintf( strTemp, "%s_%s", m_wstrNoticeVersion, UserInfoContext::i()->GetMyNickName());		// 자동 형변환..
//
//							if( i3::generic_is_equal( strTemp, szVersionText) )
//								m_bNoticeActivation = FALSE;
//							else
//								m_bNoticeActivation = TRUE;
//						}
//						else
//						{
//							wchar_t wstrVersionText[512] = {0, };
//							UINT32 Rc = fileForCheck.ReadLineW( wstrVersionText, 512 );
//							if( Rc == STREAM_ERR)
//							{
//								I3PRINTLOG(I3LOG_FATAL,  "NoticeCheck file load fail !");
//								fileForCheck.Close();
//								return;
//							}
//							
//							i3::stack_wstring wstrTemp;
//
//							i3::sprintf( wstrTemp, L"%s_%s", m_wstrNoticeVersion, UserInfoContext::i()->GetMyNickName());		
//
//							if( i3::generic_is_equal( wstrTemp, wstrVersionText) )
//								m_bNoticeActivation = FALSE;
//							else
//								m_bNoticeActivation = TRUE;
//						}
//					}
//				}
//				else
//				{
//					m_bNoticeActivation = TRUE;
//				}
//			}
//
//		}
//		else if( i3::contain_string( szwReadLine, L"[Notice_Chat]" ) >= 0 )
//		{
//			m_wstrNoticeChat[m_nNoticeChatCount] = szwReadLine + 13;	// 널값이 자꾸 포함되는 문제로 리터럴로 읽어 처리한다. (2014.07.21.수빈)
//					// .assign( szwReadLine + 13,  wReadSize - 13);		
//			m_nNoticeChatCount++;
//		}
//		else if( i3::generic_is_iequal( szwReadLine, L"[NoticeEnd]" ) )
//		{
//			bContinue = FALSE;
//			continue;
//		}
//		else if( szwReadLine[0] != L';')
//		{
//			// 실제 공지사항 내용의 처리입니다.
//			INT32 nLength =	i3::generic_string_size(szwReadLine);	// 널값이 자꾸 포함되는 문제로 리터럴로 읽어 처리한다. (2014.07.21.수빈)
//				// wReadSize;  // i3::generic_string_size(szReadLine);
//		//	I3ASSERT( nLength < sizeof( m_szNoticeList[0]));
//			
//			m_wstrNoticeList[m_nNoticeListCount].resize( nLength );
//			
//			i3::copy(szwReadLine, szwReadLine + nLength, const_cast<wchar_t*>(&m_wstrNoticeList[m_nNoticeListCount][0]) );
//		//	*const_cast<wchar_t*>(&m_wstrNoticeList[m_nNoticeListCount][nLength]) = L'\n';
//			
//		//	i3::string_ncopy_nullpad( m_szNoticeList[m_nNoticeListCount], szReadLine, nLength);
//		//	m_szNoticeList[m_nNoticeListCount][nLength] = '\n';
//		//	m_szNoticeList[m_nNoticeListCount][nLength+1] = 0;
//			m_nNoticeListCount++;
//		}
//	}
//
//	file.Close();
//
//	return;
//}

void CGameFramework::PutUserChatting( const i3::rc_wstring& wstrNick, const i3::wliteral_range& wStringRng, const bool bIsGM )
{
	if( m_pChatBox)
	{
		m_pChatBox->PutUserChatting(wstrNick, wStringRng, bIsGM);
	}
}

void CGameFramework::PutSystemChatting(const i3::rc_wstring&  wstr)
{
	if( m_pChatBox)
	{
		m_pChatBox->PutSystemChatting(wstr);
	}
}

void		CGameFramework::PrintVersion( bool bServerType )
{
#if (defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)) && !defined(SALES_BUILD)

	char strVersionInfo[MAX_STRING_COUNT];

	i3::snprintf( strVersionInfo, sizeof( strVersionInfo), "%s [Server:%s] - IP(%s)", g_strVersion, 
		g_pServerInfo->GetUseServerSet(),
		g_pServerInfo->GetLogInServerIPStr() );

	//-- Server Type Text --//
	if( bServerType )
	{
		char strServerType[BATTLE_SERVER_TYPE_COUNT][MAX_STRING_COUNT] = 
		{
			" SERVER_TYPE_NONE",
			" SERVER_TYPE_P2P",
			" SERVER_TYPE_RELAY",					
			" SERVER_TYPE_DEDICATION"				
		};
		
		char strLogText[MAX_STRING_COUNT] = {0};
		i3::snprintf( strLogText, sizeof(strLogText), "====PRESTARTBATTLE SERVER TYPE :%s ====", strServerType[BattleServerContext::i()->GetServerType()] );
		I3PRINTLOG(I3LOG_DEFALUT,  strLogText );
	}

	i3::wstring wstrVersionInfo;		i3::mb_to_utf16(strVersionInfo, wstrVersionInfo);
	m_pTextVersionInfo->SetText(wstrVersionInfo);
	i3::safe_string_copy(g_strVersionBuild, strVersionInfo, sizeof( g_strVersionBuild) );	
	
	PrintHudUnitTest("");

	if( BaTMaN::use_batman )
		PrintUserInfo();
#endif
}

void		CGameFramework::PrintUIDebug()
{
	i3UIRenderer* renderer = getUIRenderer();
	
	if (renderer)
	{
		INT32 currCount = renderer->GetCurrentSpriteAllocCount();
		if (m_prevUISpriteCount != currCount)
		{
			inner::sprite_count_text = "UISprite Count :";
			i3::stack_string numstr;
			i3::integral_to_str(currCount, numstr);
			inner::sprite_count_text += numstr;
			m_prevUISpriteCount = currCount;
			inner::is_changed_text = true;
		}
	}
}

void		CGameFramework::PrintUserInfo()
{
#if (defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)) && !defined(SALES_BUILD)

	if( __get_user_id().empty() ) return;

	char str[MAX_STRING_COUNT] = "";

	i3::stack_string str_use_batman = (BaTMaN::use_batman) ? "yes" : "no";

	i3::snprintf( str, MAX_STRING_COUNT, 
		"Id : %s, \
		BatMan : %s, \
		Build Info : %s - %s", 
		__get_user_id().c_str(),
		str_use_batman,
		GameUI::pre_macro_date_to_string(),
		GameUI::pre_macro_time_to_string());

	inner::user_info_text = str;
	inner::is_changed_text = true;
	
	::SetWindowText(g_pViewer->GetWindowHandle(), __get_user_id().c_str());

#endif
}

void CGameFramework::UpdatePrintInfoText()
{
#if (defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)) && !defined( SALES_BUILD)
	if( inner::is_changed_text == false ) return;

	i3::string str(g_strVersionBuild);
	str.append("\n");
	str.append( inner::user_info_text );
	str.append("\n");
	str.append(inner::sprite_count_text);
	str.append("\n");
	str.append( inner::polygon_count_text);
	str.append("\n");
	str.append( inner::hud_unit_test );

	i3::wstring wstr;		i3::mb_to_utf16(str, wstr);

	m_pTextVersionInfo->SetText(wstr);

	inner::is_changed_text = false;
#endif
}

void CGameFramework::UpdatePolygonCount()
{
#if (defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)) && !defined( SALES_BUILD)
	inner::polygon_count_text = "Polygon Count :";
	i3::stack_string numstr;
	i3::integral_to_str(g_pViewer->GetTriangleCount(), numstr);
	inner::polygon_count_text += numstr;
	inner::is_changed_text = true;
#endif
}

void CGameFramework::UpdateFPS()
{
#if (defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)) && !defined( SALES_BUILD)
	i3::wstring wstr;

	i3::sprintf(wstr, L"%d", (UINT32)g_pViewer->GetFPS());

	m_pTextFPS->SetText(wstr);

	if (m_pTextFPS->getWidth() != (INT32)g_pViewer->GetViewWidth())
	{
		m_pTextFPS->setWidth(g_pViewer->GetViewWidth());
	}
#endif
}

void CGameFramework::PrintHudUnitTest(const char* text)
{
#if (defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)) && !defined(SALES_BUILD)
	inner::hud_unit_test = text;
	inner::is_changed_text = true;
#endif
}

void CGameFramework::VisiblePrintInfoText(bool visible)
{
#if (defined(BUILD_RELEASE_QA_VERSION) || defined( I3_DEBUG)) && !defined(SALES_BUILD)
	m_pTextVersionInfo->SetEnable(visible);
#endif
}

#if !defined(USE_LOCALE_UI)
void CGameFramework::CreateReadyStageImageRes(void)
{
	m_pEquipShapeManager->CreateReadyStageImageRes();
}

void CGameFramework::CreateBossStageStartImageRes()
{
	m_pEquipShapeManager->CreateBossStageStartImageRes();
}

void CGameFramework::DestroyReadyStageImageRes(void)
{
	m_pEquipShapeManager->DestroyReadyStageImageRes();
}

void CGameFramework::CreateWeaponDescImageRes(INT32 nWeaponFlag, i3GuiStatic* pDescStatic1, i3GuiStatic* pDescStatic2 )
{
	m_pEquipShapeManager->CreateWeaponDescImageRes(nWeaponFlag, pDescStatic1, pDescStatic2 );
}

void CGameFramework::DestroyWeaponDescImageRes(void)
{
	m_pEquipShapeManager->DestroyWeaponDescImageRes();
}
#endif

void CGameFramework::CreateBattleStageImageRes(void)
{
	m_pEquipShapeManager->CreateBattleStageImageRes();
}

void CGameFramework::DestroyBattleStageImageRes(void)
{
	m_pEquipShapeManager->DestroyBattleStageImageRes();
}



bool CGameFramework::SetWeaponKillMark(i3Sprite2D* pSprite, WEAPON_CLASS_TYPE nWeaponClass, WEAPON::RES_ID nWeaponNum, bool bHP_Up, bool bHeadShot)
{
	return m_pEquipShapeManager->SetWeaponKillMark(pSprite, nWeaponClass, nWeaponNum, bHP_Up, bHeadShot);
}

bool CGameFramework::SetWeaponSelectMark(i3GuiStatic* pStatic, WEAPON_CLASS_TYPE nWeaponClass, WEAPON::RES_ID nWeaponNum)
{
	return m_pEquipShapeManager->SetWeaponSelectMark(pStatic, nWeaponClass, nWeaponNum);
}

INT32 CGameFramework::GetWeaponOrdIdx( WEAPON_CLASS_TYPE iWeaponClass, WEAPON::RES_ID iWeaponNum)
{
	return m_pEquipShapeManager->GetWeaponOrdIdx(iWeaponClass, iWeaponNum);
}

bool CGameFramework::SetCharacterShape(i3GuiStatic* pStatic,INT32 nCharacterFlag)
{
	return m_pEquipShapeManager->SetCharacterShape( pStatic, nCharacterFlag);
}

INT32 CGameFramework::GetCharacterShape( T_ItemID ItemID)
{
	return m_pEquipShapeManager->GetCharacterShape( ItemID);
}

INT32 CGameFramework::GetMapShapeIdx( STAGE_ID id)
{
	return m_pEquipShapeManager->GetMapIdx( id);
}

INT32 CGameFramework::GetMapShapeIdx( STAGE_UID id)
{
	return m_pEquipShapeManager->GetMapIdx(id);
}

INT32 CGameFramework::GetEscapeMapShapeIdx(STAGE_ID id)
{
	return m_pEquipShapeManager->GetEscapeMapIdx(id);
}

INT32 CGameFramework::GetEscapeMapShapeIdx(STAGE_UID ord)
{
	return m_pEquipShapeManager->GetEscapeMapIdx(ord);
}

bool CGameFramework::SetRandomMapShape(i3GuiStatic* pStatic)
{
	return m_pEquipShapeManager->SetRandomMapShape(pStatic);
}

INT32 CGameFramework::GetRandomMapIdx( void)
{
	return m_pEquipShapeManager->GetRandomMapIdx();
}

bool CGameFramework::SetCardShape(i3GuiStatic* pStatic,INT32 cardSetId,INT32 cardIdx)
{
	return m_pEquipShapeManager->SetCardShape(pStatic, cardSetId, cardIdx);
}

bool CGameFramework::SetCardSetShape(i3GuiStatic* pStatic,INT32 cardSetId)
{
	return m_pEquipShapeManager->SetCardSetShape(pStatic, cardSetId);
}

bool CGameFramework::SetRibbonShape(i3GuiStatic* pStatic,INT32 ribbonId)
{
	return m_pEquipShapeManager->SetRibbonShape(pStatic, ribbonId);
}

bool CGameFramework::SetEnsignShape(i3GuiStatic* pStatic,INT32 ensignId)
{
	return m_pEquipShapeManager->SetEnsignShape(pStatic, ensignId);
}

bool CGameFramework::SetMedalShape(i3GuiStatic* pStatic,INT32 medalId)
{
	return m_pEquipShapeManager->SetMedalShape(pStatic, medalId);
}

bool CGameFramework::SetCashItemShape(i3GuiStatic* pStatic, INT32 nGameItemFlag)
{
	return m_pEquipShapeManager->SetCashItemShape(pStatic, nGameItemFlag);
}

bool CGameFramework::SetPackageGoodsShape(i3GuiStatic* pStatic, INT32 gameGoodsId)
{
	return m_pEquipShapeManager->SetPackageGoodsShape(pStatic, gameGoodsId);
}

bool	CGameFramework::SetSlotImage(i3GuiStatic* pStatic, T_ItemID ItemID)
{
	switch( GET_ITEM_TYPE(ItemID))
	{
	case ITEM_TYPE_PRIMARY:
	case ITEM_TYPE_SECONDARY:
	case ITEM_TYPE_MELEE:
	case ITEM_TYPE_THROWING1:
	case ITEM_TYPE_THROWING2:
		{
			return SetWeaponSelectMark(pStatic, WEAPON::ItemID2Class( ItemID), WEAPON::ItemID2ResID(ItemID));
		}
		break;

	case ITEM_TYPE_CHARA:
	case ITEM_TYPE_HEADGEAR:
		{
			return SetCharacterShape(pStatic, ItemID);
		}
		break;

	case ITEM_TYPE_MAINTENANCE:
	case ITEM_TYPE_WRAP_PERIOD:
	case ITEM_TYPE_WRAP_COUNT:
	case ITEM_TYPE_POINT:
	case ITEM_TYPE_INGAMEITEM:
		{
			return SetCashItemShape(pStatic, ItemID);
		}
		break;
	}
	return false;
}


// g_pFramework->getChatBox()->PutSystemChatting

bool	CGameFramework::CheckFilter_EnvSetMacroString(const i3::wliteral_range& wstrRng, i3::rc_wstring& out_system_msg)
{
	CChattingInputChecker* input_checker = m_ChattingInputChecker.get();

	input_checker->UpdateText(wstrRng);

	// 욕설필터
	if( input_checker->IsInputBlock() )
	{
		out_system_msg = GAME_RCSTRING("STR_TBL_CHAT_INGAME_NOTICE_WARNING_REPEAT_WORD");/*<도배를 방지하기 위해 연속적인 채팅은 입력되지 않습니다.>*/
		return false;
	}
	else if( input_checker->IsRepeatBlock() )
	{
		out_system_msg = GAME_RCSTRING("STR_TBL_CHAT_INGAME_NOTICE_WARNING_DUPLICATED_WORD");/*<같은 내용을 연속하여 입력할 수 없습니다.>*/
		return false;
	}  
	else if( false == IsFilterOK( wstrRng ) )
	{
		out_system_msg = GAME_RCSTRING("STR_TBL_CHAT_INGAME_NOTICE_WARNIG_WORD");/*<입력 할 수 없는 단어입니다.>*/
		return false;
	}
#if defined( LOCALE_NORTHAMERICA )
	else if( g_pFramework->IsFilter2(  wstrRng ) == false )
	{
		// <입력 할 수 없는 단어입니다.>
		out_system_msg = GAME_RCSTRING("STR_TBL_CHAT_INGAME_NOTICE_WARNIG_WORD");/*<입력 할 수 없는 단어입니다.>*/
		return false;
	}
#endif
	out_system_msg.clear();

	return true;
}

bool	CGameFramework::CheckValidNickname(const i3::wliteral_range& wNameRng)
{
	bool	Rv = true;
	INT32 idxBlank = i3::contain_char(wNameRng, L' ');

	CStrFilterPtr pStringFilter = i3::make_pscoped<CStrFilter>(); 
	CStrFilterPtr pStringFilter2 = i3::make_pscoped<CStrFilter>(); 

	pStringFilter->CreateFilter(STR_FILTER_MATCH_NICK);
	pStringFilter2->CreateFilter(STR_FILTER_INCLUDE_NICK);

	if( -1 != idxBlank )
	{
		I3PRINTLOG(I3LOG_DEFALUT, "NickName Contained Blank");
		Rv = false;
	}
	//else if( FALSE == pStringFilter2->IsFilter( Name ) )
	else if( false == pStringFilter2->IsFilterNickOK( wNameRng ) )
	{
		I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected IsFilter");
		Rv = false;
	}
	else if( false == pStringFilter->IsFilterMatchOK( wNameRng ) )
	{
		I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected IsFilterSame");
		Rv = false;
	}
	//else if( FALSE == IsFilter( Name ) )
	else if( false == m_StringFilter->IsFilterNickOK( wNameRng ) )
	{
		I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected IsFilter");
		Rv = false;
	}
	else if( false == CheckValidCharacterByLocale( wNameRng ) )
	{
		I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected CheckValidCharacterByLocale");
		Rv = false;
	}
	else if( i3::icontain_string( wNameRng, L"col:") >= 0 )
	{
		I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected CheckValidCharacterByLocale");
		Rv = false;
	}
	else if( i3::icontain_string( wNameRng, L"/col") >= 0 )
	{// revision 59160 /col 키워드 필터링 추가
		I3PRINTLOG(I3LOG_DEFALUT, "NickName Detected CheckValidCharacterByLocale");
		Rv = false;
	}

	return Rv;
}

bool CGameFramework::CheckValidCharacterByLocale( const i3::wliteral_range& wNameRng)
{

#if defined ( LOCALE_RUSSIA )
	//	//2011년  3월 29일 - 한상옥
	//	//러시아에서 키릴문자와 라틴문자의 공용사용을 제한하기위해 변경한다.
	//
	//	char * pChar = (char*) Name;
	//
	//	// 한 문자에 대한 ASCII 의 허용 값 범위 (최소값, 최대값)
	//	INT32 nlimitChara [][2] = 
	//	{
	//		{ 48, 57},		// '0' - '9'
	//		{ 65, 90},		// 'A' - 'Z'
	//		{ 97, 122},		// 'a' - 'z'
	////		{ 91, 93},		//	포루투칼어 영역 (브라질)
	////		{ 123, 125},	//  포루투칼어 영역 (브라질)
	//	};
	//
	//		char nIgnoreChar[] = {
	//		'[',
	//		']',
	//		'{',
	//		'}',
	//		'\\',
	//		'|',
	//	};
	//
	//	INT32 nRangeCnt = (sizeof(nlimitChara) / (sizeof(INT32)*2));
	//
	//	INT32 nIgnoreCnt = (sizeof(nIgnoreChar) / (sizeof(char)));
	//
	//	INT32 bFirstvaild = 2;
	//	BOOL bVaild = FALSE;
	//	while( *pChar)
	//	{
	//		for(INT32 j=0; j<nIgnoreCnt; j++)
	//		{
	//			if( *pChar == nIgnoreChar[j])
	//				return FALSE;
	//		}
	//
	//		for(INT32 i=0; i<nRangeCnt; i++)
	//		{
	//			// 문자 ASCII 영역안에 포함이 되는지 안돼는지 체크
	//			if( *pChar >= nlimitChara[i][0] && *pChar <= nlimitChara[i][1] && (i == 0))
	//			{
	//				pChar++;
	//				break;
	//			}
	//			else if( *pChar >= nlimitChara[i][0] && *pChar <= nlimitChara[i][1])
	//			{
	//				if( bFirstvaild == 2)
	//				{
	//					bFirstvaild = 1;
	//					bVaild = TRUE;
	//					pChar++;
	//					break;
	//				}
	//				else 
	//				{
	//					bVaild = TRUE;
	//					pChar++;
	//					break;
	//				}
	//
	//			}
	//			else if(*pChar < 0)
	//			{
	//				if( bFirstvaild == 2)
	//				{
	//					bFirstvaild = 0;
	//					bVaild = FALSE;
	//					pChar++;
	//					break;
	//				}
	//				else 
	//				{
	//					bVaild = FALSE;
	//					pChar++;
	//					break;
	//				}
	//			}
	//		}
	//
	//		if(( bFirstvaild != bVaild) && ( bFirstvaild != 2))
	//		{
	//			return FALSE;
	//		}
	//
	//		pChar++;
	//	}

	// 위 기존 코드에 버그가 있고(영문 키릴 조합이 가능한 케이스가 있음, '*' '!' 포함한 다른 특수문자 입력 가능함) 
	// 분석이 어려워 새로 구현함. ugeun.ji. 2016.06.30.
	// 러시아 닉네임 명명 규칙은 아래와 같음.

	// russian nickname policy.
	/*Username requirements:

	- Unique usernames only (new player can’t choose existing username)
	- Combinations “ALT + <symbol>” (special symbols) are prohibited
	- Allowed symbols:
	1. a-z (lowercase latin symbols)
	2. A-Z (uppercase latin symbols)
	3. а-я (lowercase cyrillic symbols)
	4. А-Я (uppercase cyrillic symbols)
	5. 0-9 (numbers)
	6. ! * (limited list of special symbols)

	- Username can’t consist of both latin and russian (cyrillic) symbols at the same time (“ДелаемTest” is prohibited; "Делаем" is ok, "Test" is ok)
	- Username can’t contain prohibited words (from blacklist)*/

	struct CharacterRange
	{
		UINT8 lower, upper;

		CharacterRange(UINT8 l, UINT8 u) : lower(l), upper(u) {}
		CharacterRange(UINT8 ch) : lower(ch), upper(ch) {}

		bool IsIn(UINT8 character) const
		{
			return (lower <= character) && (character <= upper);
		}
	};

	// alphabets.
	i3::vector<CharacterRange> alphabets;
	alphabets.push_back(CharacterRange('a', 'z'));
	alphabets.push_back(CharacterRange('A', 'Z'));

	// lower/upper cyrillics. http://www.ascii.ca/cp1251.htm
	i3::vector<CharacterRange> cyrillics;
	cyrillics.push_back(CharacterRange(128, 129));
	cyrillics.push_back(CharacterRange(131));
	cyrillics.push_back(CharacterRange(138));
	cyrillics.push_back(CharacterRange(140, 144));
	cyrillics.push_back(CharacterRange(154));
	cyrillics.push_back(CharacterRange(156, 159));
	cyrillics.push_back(CharacterRange(161, 163));
	cyrillics.push_back(CharacterRange(165));
	cyrillics.push_back(CharacterRange(168));
	cyrillics.push_back(CharacterRange(170));
	cyrillics.push_back(CharacterRange(175));
	cyrillics.push_back(CharacterRange(178, 180));
	cyrillics.push_back(CharacterRange(184));
	cyrillics.push_back(CharacterRange(186));
	cyrillics.push_back(CharacterRange(188, 255));

	// digits.
	i3::vector<CharacterRange> digits;
	digits.push_back(CharacterRange('0', '9'));

	// special characters.
	i3::vector<UINT8> specialCharacters;
	specialCharacters.push_back('*');
	specialCharacters.push_back('!');

	// disallow characters.
	i3::vector<UINT8> disallowCharacters;
	disallowCharacters.push_back('[');
	disallowCharacters.push_back(']');
	disallowCharacters.push_back('{');
	disallowCharacters.push_back('}');
	disallowCharacters.push_back('\\');
	disallowCharacters.push_back('|');

	char Name[256]; i3::utf16_to_mb(wNameRng, Name, sizeof(Name));

	i3::vector<UINT8> characters;
	const char* pChar = Name;
	while (*pChar)
	{
		characters.push_back(*pChar);
		++pChar;
	}

	// disallow characters checking.
	for (size_t i = 0; i < characters.size(); i++)
	{
		i3::vector<UINT8>::iterator it = i3::find(disallowCharacters.begin(), disallowCharacters.end(), characters[i]);
		if (it != disallowCharacters.end())
			return FALSE;
	}

	// 규칙에 부합하는 문자들을 벡터에서 제거한 후 남는 문자들을 규칙에 부합하지 않는 것으로 간주하고 진행.

	// eliminate digit characters.
	struct fn : i3::binary_function<CharacterRange, UINT8, bool>
	{
		bool operator()(const CharacterRange& range, UINT8 character) const
		{
			return range.IsIn(character);
		}
	};

	for (i3::vector<UINT8>::iterator it = characters.begin(); it != characters.end();)
	{
		i3::vector<CharacterRange>::iterator digit_it =
			i3::find_if(digits.begin(), digits.end(), i3::bind2nd(fn(), *it));

		if (digit_it != digits.end())
			it = characters.erase(it);
		else
			++it;
	}

	// eliminate special characters.
	for (i3::vector<UINT8>::iterator it = characters.begin(); it != characters.end();)
	{
		i3::vector<UINT8>::iterator special_it =
			i3::find(specialCharacters.begin(), specialCharacters.end(), *it);

		if (special_it != specialCharacters.end())
			it = characters.erase(it);
		else
			++it;
	}

	// 위의 원소 제거 코드가 중복되는 면이 있는데 두 번 정도는 양해 바라며 함수로 따로 빼지 않고 그냥 풀어씁니다.

	// 이제 alphabet, cyrillic 문자만 남음.
	// 두 종류의 문자 조합 여부에 대한 판단은 각각 별도의 벡터에 저장 후 원소 개수를 비교하는 것으로 처리.
	i3::vector<UINT8> alphabetsOnly;
	for (i3::vector<UINT8>::iterator it = characters.begin(); it != characters.end();)
	{
		i3::vector<CharacterRange>::iterator alphabets_it =
			i3::find_if(alphabets.begin(), alphabets.end(), i3::bind2nd(fn(), *it));

		if (alphabets_it != alphabets.end())
		{
			alphabetsOnly.push_back(*it);
			it = characters.erase(it);
		}
		else
			++it;
	}

	i3::vector<UINT8> cyrillicsOnly;
	for (i3::vector<UINT8>::iterator it = characters.begin(); it != characters.end();)
	{
		i3::vector<CharacterRange>::iterator cyrillics_it =
			i3::find_if(cyrillics.begin(), cyrillics.end(), i3::bind2nd(fn(), *it));

		if (cyrillics_it != cyrillics.end())
		{
			cyrillicsOnly.push_back(*it);
			it = characters.erase(it);
		}
		else
			++it;
	}

	if (!characters.empty())	// 규칙에 부합하는 문자들을 제거한 상태. 남은 문자가 있다면 규칙을 위배하는 문자로 간주.
		return FALSE;

	if (!alphabetsOnly.empty() && !cyrillicsOnly.empty())	// 알파벳 키릴 문자 조합 불가.
		return FALSE;

	return TRUE;
#endif

	return true;
}

UINT32 CGameFramework::GetAvailableWeaponFlagInMyInven( WEAPON_CLASS_TYPE nWeaponClass, 
													   bool bCheckAuth /*= false*/, bool bKnuckle/* = false */)
{
	//	처음 발견되는 샷건 반환합니다.
	//	bCheckAuth 가 TRUE면 인증된 것만 반환

	CShop* pShop = CShop::i();

	INT32 Count = m_pInvenList->GetItemCount( INVEN::DB_WEAPON);
	for(INT32 i = 0 ; i < Count ; i++)
	{
		// 아이템 아이디를 얻는다.
		T_ItemID ItemID = m_pInvenList->GetItemID( INVEN::DB_WEAPON, i);

		// 무기 타입을 얻어낸다.
		WEAPON_CLASS_TYPE weaponType = WEAPON::ItemID2Class( ItemID);
		if( weaponType != nWeaponClass)
			continue;

		if (GetDefaultWeapon(WEAPON::ItemID2Slot(ItemID), weaponType) == ItemID)
			return ItemID;

		//너클류 검사일 경우
		if( bKnuckle )
		{
			INT32 _Number = WEAPON::ItemID2ResID( ItemID );
			if( _Number != 2 &&	//BrassKnuckle		
				_Number != 3 &&		//SilverKnuckle
				_Number != 4 &&		//BlackKnuckle
				_Number != 5 &&		//BlackKnuckle
				_Number != 6 &&		//BlackKnuckle
				_Number != 7 )		//BareFist
				continue;
		}

		// 구입인증여부를 확인하기 위해서 아이템 타입을 얻는다.
		UINT8 ItemType = 0;
		if ( !m_pInvenList->GetItemTypeByIdx( INVEN::DB_WEAPON, i, &ItemType, nullptr) )
			continue;

		// 개수제인지 기간제인지 알아낸다.
		UINT8 AuthType = pShop->GetAuthType(ItemID);

		if ( 0 == AuthType )
			continue;

		// bCheckAuth가 TRUE라면 인증하지 않은 무기는 스킵합니다.
		// 기간제 아이템인데 인증하지 않았으면 스킵합니다.
		if( bCheckAuth && ITEM_TYPE_TIME == AuthType && ItemType == ITEM_ATTR_TYPE_BUY)
			continue;

		return ItemID;
	}

	return 0;	//	없다.
}

bool CGameFramework::IsBattleStage(void)
{
	i3Stage* stage = i3Framework::GetCurrentStage();

	if (i3::same_of<CStageBattle*>(stage))
		return true;

	return false;
}

bool CGameFramework::IsUIStage( void)
{
	i3Stage* stage = i3Framework::GetCurrentStage();

	if( i3::same_of<UIFramework*>(stage))
		return true;
	return false;
}

bool CGameFramework::IsUsedWeapon(INT32 nWeaponFlag)
{
	USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;
	UserInfoContext::i()->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);

	if( (UINT32)nWeaponFlag == InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_PRIMARY ] )	return true;
	if( (UINT32)nWeaponFlag == InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_SECONDARY ] )	return true;
	if( (UINT32)nWeaponFlag == InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_MELEE ] )		return true;
	if( (UINT32)nWeaponFlag == InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_THROWING1 ] )	return true;
	if( (UINT32)nWeaponFlag == InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_THROWING2 ] )	return true;

	return false;
}

bool CGameFramework::EquipWeaponToMyInven( T_ItemID WeaponID, bool bUpdateViewModel)
{
	g_pFramework->OnEvent( UIEVENT_CHANGE_WEAPON, this, WeaponID, I3_EVT_CODE_ACTIVATE);
	
	return true;
}

bool CGameFramework::EquipWeaponInGameMode(void)
{
	//USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;
	//UserInfoContext::i()->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);

	UINT8 WeaponFlag = MyRoomInfoContext::i()->GetMyRoomWeaponFlag();
	
	const UI_CHARA_INFO * pCharaInfo;
	pCharaInfo = UserInfoContext::i()->GetMyUICharaInfo();

	//	저격전은 저격 무기만 장비
	if( TESTBIT( WeaponFlag, LOCK_SNIPER))
	{
		//	현재 장비가 저격무기가 아닐경우 강제로 기본 저격총으로 변경
		if (WEAPON::ItemID2Class( pCharaInfo->getWeaponItemID(WEAPON_SLOT_PRIMARY)) != WEAPON_CLASS_SNIPER )
		{
			//	인벤토리에 저격무기가 있다면 (인벤토리에 저격무기가 없으면 스킵)
			if( m_pInvenList->HaveItemBySubtype( INVEN::DB_WEAPON, WEAPON_CLASS_SNIPER) )
			{
				INT32 nSniperFlag = g_pFramework->GetAvailableWeaponFlagInMyInven( WEAPON_CLASS_SNIPER, true);

				if ( nSniperFlag > 0)
					return EquipWeaponToMyInven( nSniperFlag, true);
			}
		}
	}
	else if( TESTBIT( WeaponFlag, LOCK_SHOTGUN))
	{
		// REFACTOR_TARGET_SHOTGUN
		//	현재 장비가 샷건전일 경우 샷건이 아니면 인벤토리에 있는 샷건으로 강제로 변경
		if (WEAPON::ItemID2Class( pCharaInfo->getWeaponItemID(WEAPON_SLOT_PRIMARY)) != WEAPON_CLASS_SHOTGUN )
		{
			//	인벤토리에 샷건이 있다면 (인벤토리에 샷건이 없으면 스킵)
			if( m_pInvenList->HaveItemBySubtype( INVEN::DB_WEAPON, WEAPON_CLASS_SHOTGUN))
			{
				INT32 nShotgunFlag = g_pFramework->GetAvailableWeaponFlagInMyInven( WEAPON_CLASS_SHOTGUN, true);

				if ( nShotgunFlag > 0)
					return EquipWeaponToMyInven( nShotgunFlag, true);
			}
		}
	}
	else if( TESTBIT( WeaponFlag, LOCK_KNUCKLE))
	{
		// REFACTOR_TARGET_SHOTGUN
		//	현재 장비가 너클전일 경우 너클이 아니면 인벤토리에 있는 너클으로 강제로 변경
		if (WEAPON::ItemID2Class( pCharaInfo->getWeaponItemID(WEAPON_SLOT_MELEE)) != WEAPON_CLASS_KNUCKLE )
		{
			//	인벤토리에 너클이 있다면 (인벤토리에 너클이 없으면 스킵)
			if( m_pInvenList->HaveItemBySubtype( INVEN::DB_WEAPON, WEAPON_CLASS_KNUCKLE))
			{
				INT32 nKnuckleFlag = g_pFramework->GetAvailableWeaponFlagInMyInven( WEAPON_CLASS_KNUCKLE, true, true);

				if ( nKnuckleFlag > 0)
					return EquipWeaponToMyInven( nKnuckleFlag, true);
			}
		}
	}

	return false;
}

T_ItemID CGameFramework::FindWeaponIDForGameMode(const WEAPON_CLASS_TYPE class_type )
{
	T_ItemID item_id = 0;

	if( m_pInvenList->HaveItemBySubtype( INVEN::DB_WEAPON, class_type) )
	{
		item_id = GetAvailableWeaponFlagInMyInven( class_type, true);
	}
	
	if( item_id == 0 )
	{
		WEAPON::RES_ID weapon_number = 0;

		switch(class_type)
		{
		case WEAPON_CLASS_SNIPER			: weapon_number = WEAPON::getItemIndex(WEAPON::SSG69_IN_MODE);		break;
		case WEAPON_CLASS_SHOTGUN			: weapon_number = WEAPON::getItemIndex(WEAPON::S870MCS_IN_MODE);	break;
		case WEAPON_CLASS_ROCKET_LAUNCHER	: weapon_number = WEAPON::getItemIndex(WEAPON::RPG7);				break;
		case WEAPON_CLASS_KNUCKLE			: weapon_number = WEAPON::getItemIndex(WEAPON::BAREFIST_IN_MODE);	break;
		}

		if( weapon_number != 0 )
			item_id = MAKE_ITEM_FLAG( ITEM_EVENT_TYPE_NONE, Weapon::GetItemTypeFromClass(class_type), class_type, weapon_number );
	}

	return item_id;
}

T_ItemID CGameFramework::FindWeaponIDForGameMode(const T_ItemID ItemID)
{
	UINT8 WeaponFlag = MyRoomInfoContext::i()->GetMyRoomWeaponFlag();

	WEAPON_SLOT_TYPE slot = WEAPON::ItemID2Slot(ItemID);
	WEAPON_CLASS_TYPE class_type = WEAPON::ItemID2Class(ItemID);

	if( TESTBIT( WeaponFlag, LOCK_RPG7) )
	{
		slot = WEAPON_SLOT_PRIMARY;
		class_type = WEAPON_CLASS_ROCKET_LAUNCHER;
	}
	else if( TESTBIT( WeaponFlag, LOCK_SHOTGUN))
	{
		slot = WEAPON_SLOT_PRIMARY;
		class_type = WEAPON_CLASS_SHOTGUN;
	}
	else if( TESTBIT( WeaponFlag, LOCK_SNIPER))
	{
		slot = WEAPON_SLOT_PRIMARY;
		class_type = WEAPON_CLASS_SNIPER;
	}
	else if( TESTBIT( WeaponFlag, LOCK_KNUCKLE))
	{
		slot = WEAPON_SLOT_MELEE;
		class_type = WEAPON_CLASS_KNUCKLE;
	}

	T_ItemID item_id = ItemID;

	if( WEAPON::ItemID2Slot(item_id) == slot && WEAPON::ItemID2Class(item_id) != class_type )
		item_id = FindWeaponIDForGameMode( class_type );

	return item_id;
}

bool CGameFramework::IsUsedCharacter(INT32 nCharacterFlag)
{
	//<캐릭터> 해당 함수는 V11 기준으로 자신이 장비하고 있는 파츠 및 캐릭터가 있는지를 판단하는 함수입니다.
	//현재 캐릭터가 개발 완료가 되지 않아 임시로 만듭니다.

	return false;
}

bool CGameFramework::EquipCharacterToMyInven( T_ItemID ItemID )
{
	g_pFramework->OnEvent( UIEVENT_CHANGE_PARTS, this, ItemID, I3_EVT_CODE_ACTIVATE);

	return true;
}

void	CGameFramework::OnActivate( bool bActivate)
{
	SetGameCursor( bActivate);

	i3Framework::OnActivate( bActivate);
}

bool CGameFramework::OnQueryClose(void)
{
	if(g_pFramework == nullptr)
		return false;

	i3Stage * pStage = GetCurrentStage();
 
	if(pStage != nullptr) 
	{
		// 인게임 화면
		if( i3::kind_of<CStageBattle*>(pStage))
		{
			if( static_cast<CStageBattle*>(pStage)->IsLoadCompleteGameData() )
				gexit_popup::i()->open();
		}
		else if( i3::same_of<UIFramework*>(pStage))
		{	// 로비
			UIFramework * pFrame = (UIFramework*)pStage;
			pFrame->OnEscape();
		}
		else if( i3::same_of<UILogin*>(pStage))
		{	// 로그인
			UILogin * pLogin = (UILogin*) pStage;

			pLogin->OnEscape();
		}
	}

	return false;
}

void CGameFramework::OnChangeVideo( i3VideoInfo * pInfo)
{
	i3GuiRoot::ChangeResolution( GetViewer());

	//	메시지박스 크기 재계산 (메시지 박스는 GUI를 사용하는 것이 아니기 때문에 따로 여기서 호출합니다. komet)
	ResizeMessageBox();

	i3Framework::OnChangeVideo( pInfo);
}

void CGameFramework::_LogPerf(void)
{
#if !defined( I3_NO_PROFILE)
	GetViewer()->LogPerformanceReport( "info.txt");
#endif
}


void CGameFramework::SetReflectMap( i3TextureCube * pTex )
{
	m_pReflectMapAttr->SetTexture( pTex);

	//CHAR_SOLO_RENDER
	//m_pPlayerReflectMapAttr->SetTexture( pTex );
}

void CGameFramework::SetFresnelEnable( bool bFlag)
{
	i3AttrStackManager * pAttrManager = GetViewer()->GetSceneTracer()->GetAttrStackManager();
	i3AttrStack * pAttrStack = pAttrManager->GetAttrStackByID( I3_ATTRID_FRESNEL);

	pAttrStack->SetEnable( bFlag == true);
}

INT32 CGameFramework::GetAnnounceCount(void) const
{
	return m_pAnnounceConfig->GetCount();
}

const char* CGameFramework::GetAnnounceName( INT32 idx ) const
{
	return m_pAnnounceConfig->GetName(idx);
}

const char* CGameFramework::GetAnnounceURL(INT32 idx) const
{
	return m_pAnnounceConfig->GetURL(idx);
}

const char*	 CGameFramework::GetAnnouncePath(INT32 idx) const
{
	return m_pAnnounceConfig->GetPath(idx);
}

bool CGameFramework::IsAnnounceScroll(INT32 idx) const
{
	return m_pAnnounceConfig->IsScroll(idx);
}

void CGameFramework::EnableExitPopup(const i3::rc_wstring& wstr_msg)
{
	GameUI::MsgBox(MSG_TYPE_GAME_WAIT, wstr_msg);
	SetExitGame(true);
}

bool CGameFramework::IsStage(STAGE_ID eID)
{
	return (MyRoomInfoContext::i()->getStage()->m_ui32StageID == eID);
}

void	CGameFramework::SetCommandString(const char * pCmdString)
{
	StringCchCopy(m_CmdString, 1024, pCmdString);
}

LRESULT CALLBACK CGameFramework::StaticWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return g_pFramework->WndProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CGameFramework::WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	case WM_CREATE :
		WinMagnet::CreateProc();
		break;

	case WM_WINDOWPOSCHANGING:
		WinMagnet::WindowPosChangingProc(lParam);
		break;

	case WM_ACTIVATE:
		{
			//
			// wParam Low : WA_ACTIVE, WA_CLICKACTIVE, WA_INACTIVE
			// wParam Hi : 0이 아니면 Minimized
			//
			//BOOL	Minimized = (0 != HIWORD(wParam));
			//INT32	State = LOWORD(wParam);

#if defined(WINDOW_MODE_DISABLE)
			PB_VM_S
			g_DidThisInactive = true;
			PB_VM_E
			I3PRINTLOG(I3LOG_NOTICE,"WM_ACTIVATE %d", wParam);
#endif
		}
		break;

	case WM_ACTIVATEAPP:
		{
			//
			// lParam 값은 어플이 활성 또는 비활성화될 때 반대로 비활성 또는 활성화가 되는 프로세스의 윈도우 스레드 ID를 받는다.
			// 서로 다른 프로세스간의 전환에만 발생하며 같은 프로세스간 윈도우 전환에는 WM_ACTIVATE를 사용함.
			//
#if defined(WINDOW_MODE_DISABLE)
			PB_VM_S
			g_DidThisInactive = true;
			PB_VM_E
			I3PRINTLOG(I3LOG_NOTICE,"WM_ACTIVATEAPP %d", wParam);
#endif
			if( wParam != false)
			{
				// 활성화 될 때
#if defined(LOCALE_RUSSIA) || defined(LOCALE_LATIN_AMERICA) || defined(DEV_BUILD) || defined(LOCALE_TAIWAN)
				// 캐쉬 충전 후에 보유금액을 갱신하기 위함이다.
				// 타국가에서는 서버에서 처리해주나 러시아는 그게 안되기 때문이다.
				if( IsUIStage() )
				{
					
	#if defined(LOCALE_RUSSIA)
					UIMainFrame * pFrame = GetUIMainframe();
					if( pFrame->GetCurrentPhaseType() == UIPHASE_LOBBY ||
						pFrame->GetCurrentPhaseType() == UIPHASE_READY_ROOM)
	#endif
					{
						GameEventSender::i()->SetEvent(EVENT_GET_REMAIN_MONEY);
						//I3PRINTLOG(I3LOG_NOTICE,"EVENT_GET_REMAIN_MONEY");
					}
				}
#endif
			}
			else
			{
				i3Viewer * pViewer = g_pFramework->GetViewer();
				if (pViewer)
				{
					i3VideoInfo* pOrigVideo = pViewer->GetRenderContext()->GetVideoInfo();
					if (pOrigVideo)
					{
						if (!pOrigVideo->m_bFullScreen)
							return DefWindowProc(hWnd, uMsg, wParam, lParam);
					}
				}
			}
			//I3TRACE("WM_ACTIVATEAPP : [wParam: %d (lo:%d/hi:%d)] [lParam: %d(lo:%d/hi:%d]\n", wParam, LOWORD(wParam), HIWORD(wParam), lParam, LOWORD(lParam), HIWORD(lParam));
		}
		break;
	case WM_SIZE:
		//중복되는 코드라 case문을 합치고 싶지만, 또 다른 기능 추가를 위하여 따로 작성합니다. ENG_김대영.
		//윈도우 사이즈가 변경 되었을 때 이벤트 발생. ( 최소화, 최대화 등 )
		if( TwitchTVContext::i()->isBroadCasting() )
		{
			if(TwitchTVContext::i()->TTVStreamStop())
			{
				if(!IsBattleStage())
					GameUI::MsgBox(MSG_TYPE_GAME_QUERY,GAME_RCSTRING("STR_POPUP_TWITCH_EXCEPTION_ALT_TAP"),&GAME_RCSTRING("STBL_IDX_COMMON_MSGBOX_CAPTION"),MAKE_CALLBACK(TwitchTVContext::i()->CbFastStopStreaming),this);	
				else
					GameUI::MsgBox(MSG_TYPE_GAME_OK,GAME_STRING("STR_POPUP_TWITCH_EXCEPTION_ALT_TAP_2"));
			}	
		}
		break;
	case WM_EXITSIZEMOVE:
		//윈도우 프레임을 클릭 하여 윈도위 위치 이동 한 후 이벤트 발생.
		if( TwitchTVContext::i()->isBroadCasting() )
		{
			if(TwitchTVContext::i()->TTVStreamStop())
			{
				if(!IsBattleStage())
					GameUI::MsgBox(MSG_TYPE_GAME_QUERY,GAME_RCSTRING("STR_POPUP_TWITCH_EXCEPTION_ALT_TAP"),&GAME_RCSTRING("STBL_IDX_COMMON_MSGBOX_CAPTION"),MAKE_CALLBACK(TwitchTVContext::i()->CbFastStopStreaming),this);	
				else
					GameUI::MsgBox(MSG_TYPE_GAME_OK,GAME_STRING("STR_POPUP_TWITCH_EXCEPTION_ALT_TAP_2"));
			}	
		}
		break;
	}

	return CallWindowProc(m_OldWndProc, hWnd, uMsg, wParam, lParam);
}

#if defined( AI_MOVE_TO_WEAPON)
void CGameFramework::CheckWeapons(void)
{

	INT32 i;
	i3GameRes * pRes;
	char szMsg[1024];
	bool bOK;

	for( i = 0; i < this->GetResourceCount(); i++)
	{
		pRes = this->GetResource( i);

		if( !i3::same_of<i3GameResChara*>(pRes))
			continue;

		if( i3String::Contain( pRes->GetName(), "Weapon") == -1)
			continue;

		I3TRACE( "----- %s\n", pRes->GetName());

		i3GameResChara * pResChara = (i3GameResChara *) pRes;
		i3Chara * pChara = pResChara->getChara();
		i3AIContext * pAICtx = pChara->FindAI( "CharaAI");

		if( pAICtx != nullptr)
		{
			_Rec_CheckAIState( pAICtx, "Base_FM_KnEys");
			_Rec_CheckAIState( pAICtx, "Base_M_AcdPl");
		}
		else
		{
			I3TRACE( "\tNO AI-STATE!!!!\n");
		}
	}
}

bool CGameFramework::_Rec_CheckAIState( i3AIContext * pCtx, const char * pszCharaName)
{
	INT32 i;
	i3AIState * pState;
	char szAnim[ MAX_PATH];

	for( i = 0; i < pCtx->getAIStateCount(); i++)
	{
		pState = pCtx->getAIState( i);

		if( (pState->getStyle() & I3_AISTATE_STYLE_ANIM_TEMPLATE) == 0)
			continue;

		i3::safe_string_copy( szAnim, pState->getAnimName(), MAX_PATH );
	//	i3String::ReplaceNoCase( szAnim, sizeof(szAnim), "%c", pszCharaName);
		i3::generic_string_ireplace_all(szAnim, "%c", pszCharaName);

		i3Animation * pAnim = (i3Animation *) FindResourceFromManager( szAnim, i3Animation::static_meta());

		if( pAnim == nullptr)
		{
			I3TRACE( "ERROR : [%s] - %s\n", pszCharaName, szAnim);
		}
	}

	i3GameNode * pChild = pCtx->getFirstChild();

	while( pChild != nullptr)
	{
		if( i3::same_of<i3AIContext*>(pChild))
		{
			_Rec_CheckAIState( (i3AIContext*) pChild, pszCharaName);
		}

		pChild = pChild->getNext();
	}

	return true;
}


#endif


bool CGameFramework::OpenWebBrowser( const i3::wliteral_range& wURLRng, LPCTSTR postData /*= NULL*/, bool isPopup /*= false*/ )
{

	
	return ExecuteInternetExplorer( wURLRng.c_str(), postData, isPopup);
}

void CGameFramework::OpenWebBrowserCurrSet(const i3::wliteral_range& wURLRng)
{
	wchar_t path[MAX_PATH];

	HFILE h = _lcreat("dummy.htm", 0);
	_lclose(h);

	FindExecutableW(L"dummy.htm", NULL, path);

	DeleteFile(TEXT("dummy.htm"));

	ShellExecuteW(NULL, L"open", path, wURLRng.c_str(), NULL, SW_SHOWNA);
}

CEquipShapeManager* CGameFramework::GetEquipShapeManager() const
{
	return m_pEquipShapeManager;
}

UIMainFrame * CGameFramework::GetUIMainframe( void)
{
	if(g_pFramework == nullptr)	return nullptr;

	i3Stage * pStage = GetCurrentStage();
	if(  i3::same_of<UIFramework*>(pStage))
		return ((UIFramework*)pStage)->GetUIMainframe();

	return nullptr;
}

//UIBattleFrame *	CGameFramework::GetUIBattleFrame( void)
//{
//	i3Stage * pStage = GetCurrentStage();
//	if( pStage != NULL)
//	{
//		I3ASSERT( i3::same_of<CStageBattle*>(pStage)); //->IsExactTypeOf( CStageBattle::static_meta()));
//		return ((CStageBattle*)pStage)->GetUIBattleFrame();
//	}
//
//	return NULL;
//}

/*static*/ void CGameFramework::JumpToReady( bool bToLobby)
{
#if defined( UI2_RESOLUTION_FIX)
	i3Viewer * pViewer = g_pFramework->GetViewer();

	i3VideoInfo Video;

	i3VideoInfo* pOrigVideo = pViewer->GetRenderContext()->GetVideoInfo();
	Video = *pOrigVideo;

	i3VideoInfo* pVideo = &Video;

	pVideo->m_bFullScreen			= false;
	pVideo->m_Width					= 1024;
	pVideo->m_Height				= 768;
	pVideo->m_RefreshRate			= (REAL32) g_pEnvSet->m_nScreenHz;
	pVideo->m_bVSync				= g_pEnvSet->m_bVSync;

	if( g_pEnvSet->m_ScreenDepth == 32)
		pVideo->m_RenderTargetFormat	= I3G_IMAGE_FORMAT_BGRX_8888;
	else
		pVideo->m_RenderTargetFormat	= I3G_IMAGE_FORMAT_BGR_565;

	if( g_pEnvSet->m_b16BitsDepth)
		pVideo->m_ZBufferBits			= 16;
	else
		pVideo->m_ZBufferBits			= 24;
	pVideo->m_MultiSampling			= g_pEnvSet->m_AA;
	pVideo->m_MultiSamplingQuality	= g_pEnvSet->m_AAQ;

	pViewer->SetVideoInfo( pVideo);

	g_pFramework->SetVideoCaptureInfo(g_pEnvSet->m_nVideoResolution);

#endif
	if( bToLobby)
	{
//		I3ASSERT( g_pFramework->GetUIMainframe() != NULL);		// 메인프레임은 충분이 널값이 될수 있습니다..(스테이지는 대체로 1개만 존재)(11.10.14.수빈)
		UIMainFrame::SetStartPhase( UIPHASE_LOBBY);				// static멤버함수..
	}

	g_pFramework->GotoNextStage("NewUIFrame2");
}

/*static*/ void CGameFramework::JumpToLogin( void)
{
	g_pFramework->GotoNextStage("NewLogin");
}

void CGameFramework::JumpToBattle(void)
{
	i3::string szStageCodeName;
	i3::rc_wstring wsstageName = CStageInfoUtil::GetMapData((UINT32)MyRoomInfoContext::i()->getStage()->m_ui32StageID)->getCodeName();
	i3::utf16_to_mb(wsstageName, szStageCodeName);
	
	UIPhaseBase * pBase = g_pFramework->GetUIMainframe()->GetCurrentPhase();
	pBase->m_bMyPhase = true;
	GameEventSender::i()->SetEvent(EVENT_LOADSTAGE, szStageCodeName.c_str());
	UserInfoContext::i()->SetLastGameWeapon(UserInfoContext::i()->SetMyUICharaInfo()->getWeaponItemID(WEAPON_SLOT_PRIMARY));

	GotoNextStage( szStageCodeName.c_str());
}

void CGameFramework::SetStateChina()
{
	REAL32 fScreenWidthRate =  g_pEnvSet->m_nScreenWidth / 1024.0f;
	REAL32 fScreenHeightRate =  g_pEnvSet->m_nScreenHeight / 768.0f;

	for(INT32 i=0; i<4; i++)
	{
		if(i != 0)
		{
			if( m_pTextureLogo[i] == nullptr)
			{
				char szTemp[256];
#if defined(USE_LOCALE_UI)

				i3::rc_string strCommonUIRootFolder = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(false);

				i3::snprintf(szTemp, 256, "%s/Image/Loading/FirstLoading%d.i3i", strCommonUIRootFolder, i+1);
#else
				i3::snprintf(szTemp, 256, "Gui/Loading/FirstLoading%d.i3i", i+1);
#endif
				m_pTextureLogo[i] = g_pFramework->GetResourceManager()->LoadTexture(szTemp);
				I3ASSERT( m_pTextureLogo[i] != nullptr);
			}
		}
		else
		{
			if( m_pMovie == nullptr)
			{
				//동영상 재생
				m_pMovie = i3GfxMovie::new_object();
				m_pTexture[0] = i3TextureDX::new_object();	
				m_pTexture[1] = i3TextureDX::new_object();	
				m_pMovie->SetTexture( m_pTexture );
#if defined(USE_LOCALE_UI)

				i3::rc_string strCommonUIRootFolder = i3UIFilePathMgr::i()->Retrieve_CommonUIRootDirectory(false);

				char fullPath[MAX_PATH];
				i3::sprintf(fullPath, "%s/Image/Loading/FirstLoading1.avi", strCommonUIRootFolder.c_str() );
				if( m_pMovie->InitDShowTextureRenderer(fullPath) )
				{
					m_pTextureLogo[0] = m_pMovie->GetTexture();
					m_pMovie->SetStop();
				}
#else
				if( m_pMovie->InitDShowTextureRenderer( "Gui/Loading/FirstLoading1.avi" ))
				{
					m_pTextureLogo[0] = m_pMovie->GetTexture();
					m_pMovie->SetStop();
				}
#endif
				else
				{
					m_pUseMovie = false;
				}
			}
		}

		if( m_pTextureLogo[i] != nullptr)
		{
			if( m_pSpriteLogo[i] == nullptr)
			{
				m_pSpriteLogo[i] = i3Sprite2D::new_object();
				m_pSpriteLogo[i]->Create( 1, true, true);
			

				m_pSpriteLogo[i]->SetTexture( m_pTextureLogo[i] );
				m_pSpriteLogo[i]->SetEnable( 0, true);
				m_pSpriteLogo[i]->SetRect( 0, 0.0f, 0.0f, 1024.0f * fScreenWidthRate, 768.0f * fScreenHeightRate);
				m_pSpriteLogo[i]->SetColor( 0, 255,255,255,255);
				if( i !=0 )
					m_pSpriteLogo[i]->SetTextureCoord( 0, 0.0f, 0.0f, 1024.0f, 768.0f);
				else
					m_pSpriteLogo[i]->SetTextureCoord( 0, 0.0f, (REAL32)m_pTextureLogo[0]->GetHeight(), (REAL32)m_pTextureLogo[0]->GetWidth(), 0.0f);					
					
					
			}
		}

		if( m_pSpriteLogo[i] != nullptr)
		{
			if( m_pAttrSetLogo[i] == nullptr)
			{
				m_pAttrSetLogo[i] = i3AttrSet::new_object();
				m_pAttrSetLogo[i]->AddChild(m_pSpriteLogo[i]);

				i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();
				pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR);
				pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);

				m_pAttrSetLogo[i]->AddAttr( pAttr);
	
				getGuiLayer()->AddChild( m_pAttrSetLogo[i]);
				m_pAttrSetLogo[i]->AddFlag( I3_NODEFLAG_DISABLE);
			}
		}
	}
}

void CGameFramework::SetStateRatingLogo()
{
#if defined (MULTYLANGUAGE)
	i3::stack_string strLoadFilePath;
	i3::sprintf( strLoadFilePath, "Locale/%s/GUI/Loading/Loading_Teenager.i3i", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage) );	
	i3Texture* logo_img = g_pFramework->GetResourceManager()->LoadTexture(strLoadFilePath.c_str());
#else
#if defined (USE_LOCALE_UI)
	char szFullPath[MAX_PATH];
	i3::snprintf(szFullPath, sizeof(szFullPath), "%s/Image/Loading/Loading_Teenager.i3i", i3UI::getManager()->GetCommonLocaleUIFolder()->c_str());

	i3Texture* logo_img = g_pFramework->GetResourceManager()->LoadTexture(szFullPath);
#else
	i3Texture* logo_img = g_pFramework->GetResourceManager()->LoadTexture("Gui/Loading/Loading_Teenager.i3i");
#endif
#endif

	REAL32 w = (REAL32)g_pEnvSet->m_nScreenHeight * 16.f / 9.f;

	REAL32 fScreenWidthRate =  g_pEnvSet->m_nScreenWidth / 1024.0f;
	REAL32 fScreenHeightRate =  g_pEnvSet->m_nScreenHeight / 576.f;

	REAL32 lenth = (w - (REAL32)g_pEnvSet->m_nScreenWidth)/4;

	m_pRatingLogo = i3Sprite2D::new_object();
	if( m_pRatingLogo == nullptr ) return;

	m_pRatingLogo->Create( 1, true, true);
	m_pRatingLogo->SetTexture( logo_img );		I3_SAFE_RELEASE(logo_img);

	m_pRatingLogo->SetEnable( 0, true);
	m_pRatingLogo->SetRect( 0, 0.0f, 0.0f, (1024.0f * fScreenWidthRate), 576.f * fScreenHeightRate);
	m_pRatingLogo->SetColor( 0, 255,255,255,255);
	m_pRatingLogo->SetTextureCoord( 0,  0.0f + lenth, 0.0f, 1024.0f - lenth, 576.f);				


	m_pAttrSetRatingLogo = i3AttrSet::new_object();
	if( m_pAttrSetRatingLogo == nullptr ) return;

	m_pAttrSetRatingLogo->AddChild(m_pRatingLogo);

	i3TextureFilterAttr * pAttr = i3TextureFilterAttr::new_object_ref();
	pAttr->SetMinFilter( I3G_TEXTURE_FILTER_LINEAR);
	pAttr->SetMagFilter( I3G_TEXTURE_FILTER_LINEAR);

	m_pAttrSetRatingLogo->AddAttr( pAttr);

	getGuiLayer()->AddChild( m_pAttrSetRatingLogo);
	m_pAttrSetRatingLogo->AddFlag( I3_NODEFLAG_DISABLE);
}


CStageBattle* CGameFramework::GetStageBattle()
{
	i3Stage * pStage = GetCurrentStage();

	if( i3::same_of<CStageBattle*>(pStage))
		return (CStageBattle*)pStage;

	return nullptr;
}

i3Node* CGameFramework::OnPlayerQueryAttachNode(i3GameRes * pRes)
{
	return m_pLoadedPlayerRoot;
}

i3GameRes * CGameFramework::QuaryResource( const char * pszName, bool bCommonLoad)
{
	// 로딩된 리소스 찾기
	//i3GameRes * pRes = FindResourceByOnlyName( pszName);
	const char * pszFullPath = FindResourceFullPathByOnlyName( pszName);
	if( pszFullPath != nullptr)
	{
		i3GameRes * pRes = FindResourceByPath( pszFullPath);

		if( pRes == nullptr)
		{	// 로드되지 않은 경우.
			// 로드하자!!!
			if( i3FileStream::isFile( pszFullPath))
				pRes = LoadBackgroundRes( pszName, bCommonLoad);
		}

		return pRes;
	}

	return nullptr;
}

BGLoadThread * CGameFramework::GetBGLoadThread( void)
{
	if( m_pBGLoadThread->IsFree())
		return m_pBGLoadThread;

	return nullptr;
}

bool	CGameFramework::SaveImageCapture( const char* path, INT nQuality, INT nCaptureMode, bool bCursor )
{
	if( nullptr == path )
	{
		return false;
	}

	std::wstring wsz = _bstr_t( path );
	return SaveImageCapture( wsz.c_str(), nQuality, nCaptureMode, bCursor );
}

bool	CGameFramework::SaveImageCapture( const wchar_t* path, INT nQuality, INT nCaptureMode, bool bCursor )
{
	if( nullptr == path )
	{
		return false;
	}

	std::wstring realPath = path;

	INT nFileType = BCAP_IMAGE_PNG;	
	size_t _pos = realPath.rfind( '.' );

	if( std::wstring::npos != _pos )
	{
		std::wstring ext = realPath.substr( _pos +1 );
		std::transform( ext.begin(), ext.end(), ext.begin(), std::tolower );

		if( ext == L"png" )
		{
			nFileType = BCAP_IMAGE_PNG;
		}
		else if( ext == L"bmp" )
		{
			nFileType = BCAP_IMAGE_BMP;
		}
		else if( ext == L"jpg" )
		{
			nFileType = BCAP_IMAGE_JPG;
		}
		else
		{
			nFileType = BCAP_IMAGE_JPG;
			realPath = realPath.substr( 0, _pos );
			realPath += L".jpg";
		}
	}
	else
	{
		nFileType = BCAP_IMAGE_JPG;
		realPath += L".jpg";
	}

	return m_pVideoCapture->CaptureImage( realPath.c_str(), nFileType, nQuality, nCaptureMode, bCursor ) == true;
}

// revision 55519 로딩 후 로그인 화면으로 전환 시 강제종료 되는 버그 수정
void	CGameFramework::OnFadeInStart( bool bFirst)
{
	i3Framework::OnFadeInStart( bFirst);
	getUIManager()->setInputDisable(true);
}

void	CGameFramework::OnFadeInEnd( bool bFirst)
{
	i3Framework::OnFadeInEnd( bFirst);
	getUIManager()->setInputDisable(false);
}

void CGameFramework::LoadAIStateAnimation( i3AIState * pAIState)
{
	I3ASSERT( pAIState != nullptr);

	if( pAIState->getAnim() == nullptr)
	{
		// 무기를 주운경우에는 애니메이션을 아직 설정하지 않은 관계로 여기서 애니메이션을 로딩해야한다.
		if( pAIState->getAnimName()[0] != 0)
		{
			i3AnimationResManager * pManager = static_cast<i3AnimationResManager*>(i3NamedResourceManager::FindByMeta( i3AnimationResManager::static_meta()));
			i3Animation * pAnim = reinterpret_cast<i3Animation*>(pManager->GetResource( nullptr, nullptr, pAIState->getAnimName()));
			pAIState->SetAnim( pAnim);
		}
	}
}

void CGameFramework::AddPostDrawUIScene(i3UIScene* scene)
{
	if(!scene)
		return;

	UISceneList::iterator x = m_postDrawUIScenes.begin();
	while(x != m_postDrawUIScenes.end())
	{
		if((*x) == scene) return;
		x++;
	}

	m_postDrawUIScenes.push_back(scene);
}

void CGameFramework::RemovePostDrawUIScene(i3UIScene* scene)
{
	if(!scene)
		return;

	UISceneList::iterator x = m_postDrawUIScenes.begin();
	while(x != m_postDrawUIScenes.end())
	{
		if((*x) == scene)
		{
			m_postDrawUIScenes.erase(x);
			return;
		}

		x++;
	}
}

#if defined(USE_LOCALE_UI)

/*

const char * GlobalFunc::GetLocaleFolderName(INT32 languageType)
{
	switch(languageType)
	{
	case RESOURCE_LANGUAGE_DEV:
	return "Dev";

	case RESOURCE_LANGUAGE_KOREA:
	return "Korea";

	case RESOURCE_LANGUAGE_THAILAND:
	return "Thai";

	case RESOURCE_LANGUAGE_INDONESIA:
	return "Indonesia";

	case RESOURCE_LANGUAGE_RUSSIA:
	return "Russia";

	case RESOURCE_LANGUAGE_TURKEY:
	return "Turkey";

	case RESOURCE_LANGUAGE_BRAZIL:
	return "Brazil";

	case RESOURCE_LANGUAGE_LATIN_AMERICA:
	return "Latin_America";

	case RESOURCE_LANGUAGE_NORTHAMERICA:
	return "NorthAmerica";

	case RESOURCE_LANGUAGE_ITALY:
	return "Italy";

	case RESOURCE_LANGUAGE_ENGLISH:
	return "English";

	case RESOURCE_LANGUAGE_MIDDLE_EAST:
	return "MiddleEast";

	case RESOURCE_LANGUAGE_PHILIPPINES:
	return "Philippines";

	case RESOURCE_LANGUAGE_TAIWAN:
	return "Taiwan";

	case RESOURCE_LANGUAGE_SINGMAL:
	return "Singmal";

	case RESOURCE_LANGUAGE_VIETNAM:
	return "Vietnam";

	case RESOURCE_LANGUAGE_JAPAN:
	return "Japan";

	default:
	return "Dev";
	}
}
*/


void CGameFramework::SetMainLocale()
{
	//현재 윈도우 언어가 Locale 폴더 내에 존재하는지 확인.
	//만약 존재하지 않는다면 영문으로 기본 설정.	
//	char fullPath[MAX_PATH];
//	char fullCommonPath[MAX_PATH];

	if (g_pEnvSet == nullptr)
		return;

	if (g_pEnvSet->m_nUIVersion == UFV_1_5)
	{
		i3UIFilePathMgr::i()->SetUIVersionString("UI_V15");
	}
	else if (g_pEnvSet->m_nUIVersion == UFV_1_1)
	{
		i3UIFilePathMgr::i()->SetUIVersionString("UI_V11");
	}
	else if (g_pEnvSet->m_nUIVersion == UFV_1_0)
	{
		i3UIFilePathMgr::i()->SetUIVersionString("UI_V10");
	}
	else if (g_pEnvSet->m_nUIVersion == UFV_1_2)
	{
		i3UIFilePathMgr::i()->SetUIVersionString("UI_V12");
	}

	i3Language_ID_Str_Set langSet;

	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_DEV, "Dev");
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_KOREA, "Korea");
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_THAILAND, "Thai");
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_INDONESIA, "Indonesia");
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_RUSSIA, "Russia");
	
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_TURKEY, "Turkey");
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_BRAZIL, "Brazil");
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_LATIN_AMERICA, "Latin_America");
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_NORTHAMERICA, "NorthAmerica");
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_ITALY, "Italy");

	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_ENGLISH, "English");
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_MIDDLE_EAST, "MiddleEast");
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_PHILIPPINES, "Philippines");
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_TAIWAN, "Taiwan");
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_SINGMAL, "Singmal");
	
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_VIETNAM, "Vietnam");
	vt_util::i3Language_ID_Str_AddLanguage(langSet, RESOURCE_LANGUAGE_JAPAN, "Japan");

	i3UIFilePathMgr::i()->SetLanguageSetting(langSet, true);			// V3UI!!
	
	i3UIFilePathMgr::i()->RemoveNoFolderLanguageSetting();

	i3UIFilePathMgr::i()->SetCurrentLanguageID(g_pEnvSet->m_nLanguage);



//	if (g_pEnvSet->m_nLanguage > 0)
//		i3::snprintf(fullPath, sizeof(fullPath), "Locale/%s/%s", GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage), fullCommonPath);
	/*
	if (i3System::IsDirectory(fullPath))
	{
		i3UI::getManager()->SetLocale(GlobalFunc::GetLocaleFolderName(g_pEnvSet->m_nLanguage));

		std::string test = fullCommonPath;
		test += "\\VirtualTexture";
		i3UI::getManager()->SetUIResourceFolder(test.c_str());
		i3UI::getManager()->SetCommonLocaleUIFolder(fullCommonPath);
		i3UI::getManager()->SetUIVersionName(test.c_str());
	}
	*/
	//추후에는 pef 로딩 후, 그것을 바탕으로 메인 로캘 설정
}

#endif


void CGameFramework::PrintConsole(int attributes, const char *format, ...)
{
#ifdef _CONSOLELOG
	if (m_vecConsole.size() > 0)
	{
		int ret;
		char tmp[1024];

		va_list argList;
		va_start(argList, format);
#ifdef WIN32
		ret = _vsnprintf(tmp, sizeof(tmp) - 1, format, argList);
#else
		ret = vsnprintf(tmp, sizeof(tmp) - 1, format, argList);
#endif
		tmp[ret] = 0;


		va_end(argList);

		std::string st;
		st = tmp;
		st += "\n";
		m_vecConsole[attributes]->cprintf(st.c_str());
	}
#endif
}

void CGameFramework::ConsoleLogger()
{
#ifdef _CONSOLELOG
	if (m_vecConsole.size() == 0)
	{
		CConsoleLoggerEx *pProtocol = new CConsoleLoggerEx;
		CConsoleLoggerEx *pEtc = new CConsoleLoggerEx;

		pProtocol->Create("Protocol Logger");
		pEtc->Create("Etc Logger");

		m_vecConsole.push_back(pProtocol);
		m_vecConsole.push_back(pEtc);
	}
	else
	{
		for (size_t i = 0; i < m_vecConsole.size(); i++)
		{
			m_vecConsole[i]->Close();
			I3_SAFE_DELETE(m_vecConsole[i]);
		}
		m_vecConsole.clear();
	}
#endif
}

///////////////////////////////////////////////////////////////////////
///////					ChatLimit								///////
///////////////////////////////////////////////////////////////////////

void CGameFramework::SetChatLimit(INT32 result_time)
{
	m_ChatLimitSec.SetBaseSec(result_time);
}

void CGameFramework::ChatLimit_Start()
{
	if (m_ChatLimitSec.GetBaseSec() <= 0)
		return;

	m_bChatLimitState = true;
	m_ChatLimitSec.Start();
}

bool CGameFramework::CheckChatLimit(INT32 result_time)
{
	if ( false == m_bChatLimitState || m_ChatLimitSec.GetBaseSec() <= 0)
		return false;

	return true;
}

void CGameFramework::OnChatLimit_Start(INT32 result_time)
{

}

void CGameFramework::OnChatLimit_Change(INT32 result_time)
{

}

void CGameFramework::OnChatLimit_DoneOrStop(INT32 result_time)
{
	m_bChatLimitState = false;
	g_pFramework->getChatBox()->PutSystemChatting(GAME_STRING("STR_MESSAGE_BLOCK_END"));
}