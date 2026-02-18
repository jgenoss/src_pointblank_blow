#include "pch.h"
#include "StageBattle.h"

#include "GameObject.h"
#include "ObjectManager.h"
#include "GameWorld.h"

#include "UI/UIBattleFrame.h"

#include "CrosshairMgr.h"
#include "GameMode.h"

#include "UtilBattle.h"
#include "HackDefense.h"
#include "ObserverBattle.h"
#include "LogRespawn.h"
#include "GameExitPopup.h"

#include "GameStateMgr.h"
#include "BattleHud.h"
#include "BattleGui.h"

#include "AwayInput.h"
#include "ConfigRenderBattle.h"

REAL32 g_rOne = 1.f;
REAL32 g_rHalf = 0.5f;
#include "UI/UIHudManager.h"
#include "UI/UIHudDefine.h"
#include "UI/UIUtil.h"
#include "Avatar/AvatarManager.h"

//#define i3_profile_enable
#include "i3Base/profile/profile.h"

#include "./StageBattle/UserContext.h"

#include "BattleServerContext.h"
#include "UserInfoContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "HackContext.h"
#include "CQAWeaponCMgr.h"

#ifdef DOMI_NEW_FLOW
#include "UI/HUD/UIHUD_Domination_Manager.h"
#endif

#include "GameMissionManager.h"
#include "GameMaster.h"
#include "Weapon/WeaponItemID.H"

#include "MissionCard/MCardCallback.h"
#include "MissionCard/MCardUtil.h"
#include "UI/UIHudMissionBoard.h"

#define USE_THREAD

#define OBJ_ASSIST_TIME		0.1f	// Object의 인터폴레이션 타임.. 0.1초 단위로 인터폴레이션 합니다.
#define OBJ_ASSIST_TIMEINV	10.f	//
#define OBSERVER_CAMERA_INTERPOLATE_TIME 0.1f
#define DEFENCE_MISSION_HURRYUP_TIME		30.f

I3_CLASS_INSTANCE( CStageBattle); //, i3Stage);

bool CheckCharaInfo(const CCharaInfo* pCharaInfoOrg, CCharaInfo2* pCharaInfo);
bool CheckGlobalInfo();

class MCardCallback_StageBattle : public MCardCallback
{
	virtual void OnCurrQuestCompletionCountChanged_OnBattle(INT32 questIdx, bool bCompleteCard, bool bCompleteQuest) override
	{
		UIHudManager* pMgr = HUD::instance();

		if (pMgr == nullptr)
			return;

		UIHudMissionBoard* pMissionBoard = static_cast<UIHudMissionBoard*>(pMgr->GetHud(HUD::MISSION_BOARD));
		pMissionBoard->UpdateForMissionCard();
	}

	virtual void OnCurrQuestCompleted_OnBattle(INT32 questIdx, bool bCompleteCard) override
	{
		// 퀘스트 한개 수행시마다 '수행완료' 마크 출력	.. 특별이 인수를 쓰지 않는듯..
		CHud* pHUD = CHud::i();
		if (pHUD == nullptr) return;

		pHUD->AddChallengeClear();
	}

	virtual void OnCurrMissionCardCompleted_OnBattle(INT32 lastQuestIdx) override
	{

	}

	virtual void OnCurrMissionCardCompleted_AfterBattle(INT32 completeCardIndex, const MCardInfo* card ) override
	{

	}

	virtual void OnNextMissionCardAutoSelecting_AfterBattle(INT32 prevCardIdx, INT32 currCardIdx) override
	{

	}

	virtual void OnUpdateAllHUDOnBattle() override
	{
		UIHudManager* pMgr = HUD::instance();

		if (pMgr == nullptr)
			return;
		
		UIHudMissionBoard* pMissionBoard = static_cast<UIHudMissionBoard*>(pMgr->GetHud(HUD::MISSION_BOARD));
		pMissionBoard->UpdateForMissionCard();
	}

} g_MCardCallback;


CStageBattle::CStageBattle(void) : m_bLoadCompleteGameData(false), m_BattleLoadState(BLS_STEP1), m_pUIFrame(nullptr)
{
	MCardUtil::SetMCardCallback(&g_MCardCallback);

#if defined( I3_DEBUG)
	MEM_NET_ALLOC_INFO SendBuffer; 
	SendBuffer._Command	= MEMDUMP_CMD_CLEAR;
	DumpWriteFileBuffer( &SendBuffer );
#endif
}

CStageBattle::~CStageBattle(void)
{
	MCardUtil::SetMCardCallback(nullptr);

	Destroy();

	//  리소스 릭 문제로 아래와 같은 주석코드를 풀어서 간혹 확인중임..    ( 인스턴스매니저를 쓰는 것들의 레퍼런스카운트를 조회한다...).... (2013.11.11.수빈)
	/*typedef i3::unordered_map<i3ClassMeta*, i3InstanceManager*> map_type;
	map_type::iterator it = m_InstManagerList.begin();
	map_type::iterator it_end = m_InstManagerList.end();

	for (; it != it_end ; ++it)
	{
		i3InstanceManager* mgr = it->second;
		i3ClassMeta * pMeta = it->first;
		I3TRACE( "instance dump %s ---------------\n", pMeta->class_name());
		mgr->Dump();
	}
*/

}

void CStageBattle::Destroy( void )
{
	_DestroyInDefault();
}

void CStageBattle::_DestroyInDefault()
{
	gstatemgr::i()->ProcessDestroyStage();

	// Portal 설정을 Clear한다.
	{
		//GetFramework()->setCurrentPortal( 0);
		GetFramework()->SetCurrentPortalMask( 0);
	}

	g_pViewer->GetPostProcessManager()->RemoveAllPostProcess();

	gexit_popup::i()->ProcessDestroyStage();
	CrosshairMgr::i()->ProcessDestroyStage();

	if(g_pCamera)
	{
		g_pCamera->InitCamera(nullptr);
	}

	CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FPS);
	BattleServerContext::i()->P2PEndBattle();

	if( g_pSndMng != nullptr)
	{
		g_pSndMng->StopBGM( 0.0f);
		g_pSndMng->LeaveStage();
	}

	if( g_pCharaManager != nullptr )
		g_pCharaManager->LeaveStage();

	if( g_pWeaponManager != nullptr )
		g_pWeaponManager->LeaveStage();

	if( g_pEffectManager != nullptr)
	{
		g_pEffectManager->Reset();
		RemoveChild( g_pEffectManager );
	}

	if( g_pTracerBulletManager != nullptr)
	{
		g_pTracerBulletManager->Destroy();
		RemoveChild( g_pTracerBulletManager);

		I3_SAFE_RELEASE( g_pTracerBulletManager);
	}
	if(CQAWeaponCMgr::i()->getOnWeaponCollision())
	{
		CQAWeaponCMgr::i()->ReleasePhysXBox();
		CQAWeaponCMgr::i()->setOnWeaponCollision(false);
	}

#if !defined( I3_NO_PROFILE)
	this->RemoveChild( g_pWeaponInfoDataBase );
	this->RemoveChild( g_pCharaInfoDataBase );
	this->RemoveChild( g_pPhysixInfoDataBase );
#endif

	if( g_pViewer != nullptr && g_pViewer->GetPhysixContext() != nullptr)
	{
		g_pViewer->GetPhysixContext()->setEnableSimulate( false );
	}

	// 게임종료시 모든 메세지 박스를 닫습니다.
	GameUI::CloseAllMsgBox();

	if(   UIBattleFrame::i() != nullptr)
	{
		RemoveChild( UIBattleFrame::i() );
		I3_SAFE_RELEASE( tinst<UIBattleFrame>() );
	}

	I3_SAFE_RELEASE( tinst<CRevengeManager>() );

	if( g_pEdgeTrail != nullptr)
	{
		RemoveChild( g_pEdgeTrail);
		I3_MUST_RELEASE(g_pEdgeTrail);
		g_pEdgeTrail = nullptr;
	}

	config_render_battle::i()->ProcessDestroyStage();

	I3_SAFE_DELETE( tinst<CAwayInput>());

	tinst<CBattleGui>() = nullptr;
	tinst<CHud>() = nullptr;
	tinst<CGameCameraManager>() = nullptr;

	// SceneGraph resource를 메모리에서 해제시키자
	{
		i3NamedResourceManager * pNamedResMng = i3ResourceFile::GetResourceManager( i3SceneGraphInfo::static_meta());
		I3ASSERT( pNamedResMng != nullptr);
		pNamedResMng->Reset();
	}

	if (g_pFramework->GetHazePtr()) g_pFramework->GetHazePtr()->InitEffect();
}
#ifdef DOMI_NEW_FLOW
void CStageBattle::_DestroyInRoundStageJump()
{
	gstatemgr::i()->ProcessDestroyRoundJumpStage();
	gstatemgr::i()->setStageState(GAMESTAGE_STATE_DOMI_NEW_FLOW);

	// Portal 설정을 Clear한다.
	{
		//GetFramework()->setCurrentPortal( 0);
		GetFramework()->SetCurrentPortalMask( 0);
	}

	gexit_popup::i()->ProcessDestroyStage();
	CrosshairMgr::i()->ProcessDestroyStage();

	if(g_pCamera)
	{
		g_pCamera->InitCamera(nullptr);
	}

	CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FPS);

	if( g_pSndMng != nullptr)
	{
		g_pSndMng->StopBGM( 0.0f);
		g_pSndMng->LeaveStage();
	}

	if( g_pCharaManager != nullptr )
		g_pCharaManager->LeaveStage();

	if( g_pWeaponManager != nullptr )
		g_pWeaponManager->LeaveStage();

	if( g_pEffectManager != nullptr)
	{
		g_pEffectManager->Reset();
		RemoveChild( g_pEffectManager );
	}

	if( g_pTracerBulletManager != nullptr)
	{
		g_pTracerBulletManager->Destroy();
		RemoveChild( g_pTracerBulletManager);

		I3_SAFE_RELEASE( g_pTracerBulletManager);
	}

#if !defined( I3_NO_PROFILE)
	this->RemoveChild( g_pWeaponInfoDataBase );
	this->RemoveChild( g_pCharaInfoDataBase );
	this->RemoveChild( g_pPhysixInfoDataBase );
#endif

	if( g_pViewer != nullptr && g_pViewer->GetPhysixContext() != nullptr)
	{
		g_pViewer->GetPhysixContext()->setEnableSimulate( false );
	}

	// 게임종료시 모든 메세지 박스를 닫습니다.
	GameUI::CloseAllMsgBox();

	if(   UIBattleFrame::i() != nullptr)
	{
		RemoveChild( UIBattleFrame::i() );
		I3_SAFE_RELEASE( tinst<UIBattleFrame>() );
	}

	I3_SAFE_RELEASE( tinst<CRevengeManager>() );

	if( g_pEdgeTrail != nullptr)
	{
		RemoveChild( g_pEdgeTrail);
		I3_MUST_RELEASE(g_pEdgeTrail);
		g_pEdgeTrail = nullptr;
	}

	config_render_battle::i()->ProcessDestroyStage();

	I3_SAFE_DELETE( tinst<CAwayInput>());

	tinst<CBattleGui>() = nullptr;
	tinst<CHud>() = nullptr;
	tinst<CGameCameraManager>() = nullptr;
}
#endif

void CStageBattle::LoadProperties(void)
{
	i3RegKey * pRoot = Config::FindKey("Stage");
	I3ASSERT( pRoot != nullptr);

	i3StageInfo* pInfo = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex());

	i3RegKey * pKey = Config::FindKey( pInfo->GetName(), pRoot);
	I3ASSERT(pKey != nullptr);
	
	if( pKey != nullptr)
	{
		config_render_battle::i()->ProcessLoadProperties(pKey);
		
		VEC3D v3TopViewCameraPos;
		FIND_REG_DATA( pKey, "TopViewCameraPos", &v3TopViewCameraPos);
		ub::set_topview_camera_pos(v3TopViewCameraPos);
	
		gmode::i()->ProcessLoadProperties(pKey);

		StageMovement.init(pKey);
	}
}

bool CStageBattle::OnQueryLoad(INT32	nStageIndex)
{
	i3StageInfo * pInfo = m_pFramework->getStageInfo( nStageIndex);
	if( pInfo != nullptr)
	{
		const i3::vector<i3::rc_string>& List = pInfo->getRSCPath();

		size_t nCount = List.size();
		for( size_t i = 0; i < nCount; ++i)
		{
			const char * pszString = List[i].c_str();
			m_pFramework->LoadRSC( pszString);
		}
	}
	
	hd::check_mapdata::i()->check(nStageIndex);

	return i3Stage::OnQueryLoad( nStageIndex);
}

void CStageBattle::OnLoading(void)
{
	i3Stage::OnLoading();
}

void CStageBattle::OnLoadingGameData(void)
{
	i3Stage::OnLoadingGameData();
	_OnLoadGameData();
}

void CStageBattle::OnLoadEnd(void)
{
	i3Stage::OnLoadEnd();
//	SetWindowPos( 800, 600);
}

i3Node	* CStageBattle::OnQueryAttachNode( i3GameRes * pRes)
{
	if( i3::same_of<i3GameResChara*>(pRes))
	{
		return m_pFramework->GetLayerRoot( "CharaRoot");
	}
	else if( i3::same_of<i3GameResWorld*>(pRes))
	{
		return m_pFramework->GetLayerRoot( "WorldRoot");
	}
	else
		return m_pFramework->GetLayerRoot( "ObjectRoot");
}

void CStageBattle::_InitObject()
{
	gstatemgr::i()->Process_InitObject();
}

void CStageBattle::OnCreate(void)
{
	if( g_pFramework->GetViewer()->GetPhysixContext()->isValidScene() == false)
	{
		g_pFramework->GetViewer()->GetPhysixContext()->CreateScene( NX_TIMESTEP_VARIABLE);
		g_pFramework->InitializePhysXCollisionGroup();
		g_pEffectManager->EnterStage();

		g_pInterface->InitPhysXValues();
	}

	i3Stage::OnCreate();

	m_bLoadCompleteGameData = false;
	m_BattleLoadState = BLS_STEP1;
}

void CStageBattle::_OnLoadGameData(void)
{
	//I3TRACE("CStageBattle::_OnLoadGameData\n");

	switch(m_BattleLoadState)
	{
	case BLS_STEP1:
		{
			if( _IsDominationFlow() )
				_OnLoadStageJump_Step1();
			else
				_OnLoadDefault_Step1();
		}
		break;
	case BLS_STEP2:
		{
			if( _IsDominationFlow() )
				_OnLoadStageJump_Step2();
			else
				_OnLoadDefault_Step2();
		}
		break;
	case BLS_STEP3:
		{
			if( _IsDominationFlow() )
				_OnLoadStageJump_Step3();
			else
				_OnLoadDefault_Step3();
		}
		break;
	case BLS_STEP4:
		{
			if( _IsDominationFlow() )
				_OnLoadStageJump_Step4();
			else
				_OnLoadDefault_Step4();
		}
		break;
	case BLS_STEP5:
		{
			if( _IsDominationFlow() )
				_OnLoadStageJump_Step5();
			else
				_OnLoadDefault_Step5();
		}
		break;
	case BLS_STEP6:
		{
			if( _IsDominationFlow() )
				_OnLoadStageJump_Step6();
			else
				_OnLoadDefault_Step6();
		}
		break;
	case BLS_STEP7:
		{
			if( _IsDominationFlow() )
				_OnLoadStageJump_Step7();
			else
				_OnLoadDefault_Step7();
		}
		break;
	case BLS_COMPLETE:
		{
			m_bLoadCompleteGameData = true;
		}
		break;
	}
}

static bool SelectCharacterForDebugStart( void)
{
	ISTEST_S
	{
		if (g_pEnvSet->m_nUIVersion == UFV_1_5)
		{
			PBCONSOLE_CREATE_FOR_DEBUG_RELEASEQA
		}

		i3Framework* frmWork = i3Framework::getCurrentFramework();
		i3StageInfo* startStageInfo = frmWork->GetStartStageInfo();

		bool startedByLevelTestShotcut = false; 
		// 바로가기 맵이름 형식으로 실행되었습니다.(배경,레벨팀에서 싱글플레이 맵확인모드)

		if(startStageInfo)
			startedByLevelTestShotcut = true;


		// 아바타를 등록해준다.
		CHARA_INFO charaInfo;
		charaInfo.Reset();
		
		NET_CHARA_INFO* pNetCharaInfo = BattleSlotContext::i()->setNetCharaInfo(0);
		pNetCharaInfo->Reset();

		T_ItemID charaItemID = 0;

		BattleSlotContext::i()->setMySlotIdx( 0);
		CCharaInfoContext::i()->SetTeamSlotBySlotIdx(0);
		
		CHARA_RES_ID ResID = CHARA_RES_ID_UNKNOWN;
		while( g_pCharaInfoDataBase->GetCharaDBInfo( charaItemID) == nullptr || ResID > CHARA_RES_ID_HIDE )
		{
			INT32 ResNum  = i3Math::Rand() % g_pCharaInfoDataBase->GetInfoCount(EQUIP::ePART_CHARA);
			charaItemID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, ITEM_TYPE_CHARA, GetCharaTeamType(ResNum), ResNum);
			ResID = CHARA::ItemID2ResID( charaItemID);
		}

		///////////////////////////////////////////////////////////////////////////
		// DebugDummy.txt 무기세팅적용
		///////////////////////////////////////////////////////////////////////////

		CInvenList::i()->SetWeaponListDummy();		//	인벤 무기 구성

		//	장비할 무기 셋팅
		USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;

		UserInfoContext::i()->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);

		InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_PRIMARY ]	= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_PRIMARY,		g_DebugWeaponSet[0][0], g_DebugWeaponSet[0][1]);
		InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_SECONDARY ]	= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_SECONDARY,	g_DebugWeaponSet[1][0], g_DebugWeaponSet[1][1]);
		InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_MELEE ]		= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_MELEE,		g_DebugWeaponSet[2][0], g_DebugWeaponSet[2][1]);
		InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_THROWING1 ]	= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_THROWING1,	g_DebugWeaponSet[3][0], g_DebugWeaponSet[3][1]);
		InfoEquipWeapon.m_TItemID[ CHAR_EQUIPMENT_WEAPON_THROWING2 ]	= MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE,	ITEM_TYPE_THROWING2,	g_DebugWeaponSet[4][0], g_DebugWeaponSet[4][1]);

		UserInfoContext::i()->SetMyUserInfoEquipmentWeapon(InfoEquipWeapon);

		/////////////////////////////////////////////////////////////////////////////

		ITEM_TYPE	Class[6] = {};
		Class[0] = Weapon::GetItemTypeFromClass((WEAPON_CLASS_TYPE)g_DebugWeaponSet[0][0]);
		Class[1] = Weapon::GetItemTypeFromClass((WEAPON_CLASS_TYPE)g_DebugWeaponSet[1][0]);
		Class[2] = Weapon::GetItemTypeFromClass((WEAPON_CLASS_TYPE)g_DebugWeaponSet[2][0]);
		Class[3] = Weapon::GetItemTypeFromClass((WEAPON_CLASS_TYPE)g_DebugWeaponSet[3][0]);
		Class[4] = Weapon::GetItemTypeFromClass((WEAPON_CLASS_TYPE)g_DebugWeaponSet[4][0]);
		Class[5] = Weapon::GetItemTypeFromClass((WEAPON_CLASS_TYPE)g_DebugWeaponSet[5][0]);

		// vv3 - check
		for( INT32 k = 0 ; k < CHAR_EQUIPMENT_WEAPON_COUNT ; ++k )
		{
			charaInfo.getEquipInfo()->m_CharWeaponInfo.m_Info[k].m_TItemID = MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class[k], g_DebugWeaponSet[k][0], g_DebugWeaponSet[k][1]);		// (g_DebugWeaponSet[0][0] << 8) | g_DebugWeaponSet[0][1];
		}
		
		CCharaInfoContext::i()->ClearAll();
		CCharaInfoContext::i()->AddCharacterInfo( charaInfo );
		CCharaInfoContext::i()->SetAllCharaData();
		
		{
			//여긴 콘솔모드 테스트 이므로 2개 만들어둔다..
			GLOBAL_CHARA_INFO* pCharaInfo = g_pCharaManager->setCharaInfo( 0, false);
			
			CHARA_PARTS_INFO* pParts = pCharaInfo->SetParts();

			pParts->SetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA, charaItemID);

			if(startedByLevelTestShotcut)
			{
				// 배경,레벨팀 싱글맵테스트시에는 기본파츠만 사용합니다.
				// 파츠가 없어서 무한로딩걸릴수있으니
			}
			else
			{
				// vv3 - check
				pParts->SetPartsItemID( CHAR_EQUIPMENT_PARTS_SKIN, 1402010);
			}

			CCharaInfoContext::i()->SetCharaPartsInfo(0, pParts);

			pParts->CopyTo( pNetCharaInfo->setCharaParts());

			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_PRIMARY, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class[0], g_DebugWeaponSet[0][0], g_DebugWeaponSet[0][1]));	
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_SECONDARY, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class[1], g_DebugWeaponSet[1][0], g_DebugWeaponSet[1][1]));	
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_MELEE, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class[2], g_DebugWeaponSet[2][0], g_DebugWeaponSet[2][1]));		
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING1, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class[3], g_DebugWeaponSet[3][0], g_DebugWeaponSet[3][1]));	
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_THROWING2, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class[4], g_DebugWeaponSet[4][0], g_DebugWeaponSet[4][1]));	
			pNetCharaInfo->setWeaponItemID( WEAPON_SLOT_MISSION, MAKE_ITEM_FLAG(ITEM_EVENT_TYPE_NONE, Class[5], g_DebugWeaponSet[5][0], g_DebugWeaponSet[5][1]));	

			g_pCharaManager->setCharaResState( 0, CGameCharaManager::RESSTATE_LOADCMD);
		}

		/*ROOM_INFO_BASIC info;
		MyRoomInfoContext::i()->getMyRoomInfo( &info);
		info._StageMask = STAGE_MASK_TYPE_TRAINING;
		MyRoomInfoContext::i()->setMyRoomInfo( &info );*/

		pNetCharaInfo->CopyTo( UserInfoContext::i()->SetMyUICharaInfo(), TEAM_BOTH );

		g_pFramework->getPhysixContext()->setEnableSimulate( true );

		return true;
	}
	ISTEST_E

	return false;
}

bool CStageBattle::_OnLoadCommon_Step1(void)
{
	// NetworkCharacter Layer

	// 1인칭 관전 카메라 용 character 레이어
	i3GameNode* pCharacterLayer = i3GameNode::new_object_ref();
	pCharacterLayer->Create(this);
	AddChild(pCharacterLayer);

	//Character Manager
	g_pCharaManager->SetCharaLayer( pCharacterLayer );	// 관전 카메라 관련

	// weapon manager
	g_pWeaponManager->EnterStage( this);

	g_pCharaManager->EnterStage( this );


	// 자신의 캐릭터를 추가해줍니다
	if( SelectCharacterForDebugStart() == false)
	{
		if( BattleServerContext::i()->IsMovingStage())
		{
			for( UINT8 cnt = 0; cnt < SLOT_MAX_COUNT; ++cnt)
			{
				if( UserContext::i()->IsPlayingSlot(cnt) == false) continue;

				g_pCharaManager->setCharaResState( cnt, CGameCharaManager::RESSTATE_LOADCMD);
				I3TRACE( ">> Domination stage jumping Avatar Load CMD -> idx : %d\n", cnt);
			}
		}
		else
		{
			UserContext::i()->PlayingSlotFlag = 0;

			if(MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_TUTORIAL))
				g_pCharaManager->setCharaResState( 0, CGameCharaManager::RESSTATE_LOADCMD);

			I3ASSERT( g_pCharaManager->getCharaResState( BattleSlotContext::i()->getMySlotIdx()) == CGameCharaManager::RESSTATE_LOADCMD);
			g_pCharaManager->setCharaResState( BattleSlotContext::i()->getMySlotIdx(), CGameCharaManager::RESSTATE_LOADCMD);
		}
	}
	
	BattleServerContext::i()->P2PMyStartBattle();

	_LoadReflectMap();

	_LoadPostProcess();

	// Decal의 ActorPair를 세팅해줘야 합니다.
	i3Decal_InitWorldHitPair();	// <- i3Decal.h

	LoadProperties();

	CGameMissionManager::i()->LoadPropertiesByStage();

#if !defined( I3_NO_PROFILE)
	//	데이터 베이스 정보
	{
		//	캐릭터
		I3_GAMENODE_ADDCHILD( this, g_pCharaInfoDataBase);
		//	무기
		I3_GAMENODE_ADDCHILD( this, g_pWeaponInfoDataBase);
		// Physix
		I3_GAMENODE_ADDCHILD( this, g_pPhysixInfoDataBase );
	}
#endif

	return true;
}

bool CStageBattle::_OnLoadCommon_Step2(void)
{
	// 해당 스탭은 별다른 로딩 없이, 프로그래스바를 시각적으로 99%까지 올리는 부분입니다.
	if( g_pFramework->GetLoadingPercent() < 0.0f)
	{
		g_pFramework->SetLoadingPercent(0.0f);
	}

	if( g_pFramework->GetLoadingPercent() >= 0.99f )
		return true;

	return false;
}

bool CStageBattle::_OnLoadCommon_Step3(void)
{
	CGameMissionManager * pMissionManager =CGameMissionManager::i();

	{
		CSI_STAGE* Stage		= MyRoomInfoContext::i()->getStage();
		STAGE_MODE StageType	= Stage->GetStageMode();
		// Mode에 따른 Object 정보 버퍼를 생성합니다.
		pMissionManager->InitializeObject( StageType);
		// Stage에 따라 초기화
		pMissionManager->StartMission( Stage );

		// SoundManager Enter
		g_pSndMng->EnterStage( this);

		//Weapon Manager
		if( MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CONVOY)  &&
			MyRoomInfoContext::i()->getStage()->IsEventforEventType(STAGE_EVENT_SEASON))
		{
		}
		else
		{
			g_pWeaponManager->CreateThrowWeapon(this);
			g_pWeaponManager->CreateGrenadeShellWeapon( this);
			g_pWeaponManager->CreateRocketLauncherShellWeapon( this);
		}

		g_pWeaponManager->CreateAllThrowKnife( this);

	}

	// 오브젝트 초기화
	_InitObject();

	// CrossHair 추가
	{
		CrosshairMgr::i()->ProcessCreateStage(this);
	}

	I3TRACE( "UIBattleFrame::i() == %p\n", UIBattleFrame::i());
	tinst<UIBattleFrame>() = UIBattleFrame::new_object();
	UIBattleFrame::i()->OnCreate();		//UIHUDDomiantionMgr 보다 먼저 초기화 함. (ex. Domination Fade in/out 시 야전 상점이 안보이도록 하기 위해서)
	I3PRINTLOG(I3LOG_NOTICE, "In-Game UI was been loaded.");

	AddChild(UIBattleFrame::i());
	
	// HUD 추가 - 순서 중요 (앞으로 옮기면 크래쉬 함)

	tinst<CHud>() = CHud::new_object_ref();
	if ( CHud::i()->Create( BattleSlotContext::i()->getMySlotIdx()))
	{
		I3_GAMENODE_ADDCHILD( this, CHud::i() );
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL, "[CStageBattle::Create] Hud Creation Fail");
	}

	tinst<CRevengeManager>() = CRevengeManager::new_object();
	CRevengeManager::i()->InitData();	

	// GUI 추가
	tinst<CBattleGui>() = CBattleGui::new_object_ref();
	if( (CBattleGui::i()->Create()) )
	{
		I3_GAMENODE_ADDCHILD( this, CBattleGui::i());
	}
	else
	{
		I3PRINTLOG(I3LOG_FATAL,  "[CStageBase::Create] Battle Gui Creation Fail");
	}

	//	TracerBullet Manager
	{
		if( g_pTracerBulletManager == nullptr)
		{
			g_pTracerBulletManager = CTracerBulletManager::new_object();
			I3ASSERT( g_pTracerBulletManager != nullptr);
		}
		else
		{
			g_pTracerBulletManager->Destroy();
		}

		i3Node * pEffLayer = g_pFramework->GetLayerRoot( "EffectRoot");
		I3ASSERT( pEffLayer != nullptr);

		if( !g_pTracerBulletManager->Create( pEffLayer))
		{
			I3TRACE( "[Warning! TracerBullet Manager create failed!!\n");
		}

		I3_GAMENODE_ADDCHILD( this, g_pTracerBulletManager);
	}

	// 입력없음 체크 중지 
	tinst<CAwayInput>() = new CAwayInput;
	MEMDUMP_NEW( tinst<CAwayInput>(), sizeof(CAwayInput));

	CAwayInput::i()->ProcessCreateStage();

	return true;
}

bool CStageBattle::_OnLoadCommon_Step4(void)
{
	// 자신의 캐릭터는 반드시 로딩되야 합니다.
#if !defined( USE_THREAD)
	Avatar * pPlayer = (Avatar*) g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	I3ASSERT( pPlayer != nullptr);

	if( pPlayer->GetLoadedParts() == false)
	{
		if( pPlayer->OnUpdateAvatarGen_InThread() == false)
		{
			return false;
		}
	}
#else
	Avatar * pPlayer = (Avatar*) g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	I3ASSERT( pPlayer != nullptr);

	if( pPlayer->GetLoadedParts() == false)
	{
		return false;
	}
#endif

	return true;
}

bool CStageBattle::_OnLoadCommon_Step6(void)
{
	Avatar * pPlayer = (Avatar*) g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	I3ASSERT( pPlayer != nullptr);

	REAL32 theta;
	VEC3D vRespawnPos;

	// 리스폰 위치 설정
	theta = g_pCharaManager->GetRespawnPosition( &vRespawnPos, pPlayer->getCharaInfo()->GetTeam(), BattleSlotContext::i()->getMySlotIdx());
	pPlayer->Cmd_Respawn( &vRespawnPos, theta );
	pPlayer->SetVisible( false);

	//Camera 먼저 FPS모드로 세팅합니다.
	tinst<CGameCameraManager>() = CGameCameraManager::new_object_ref();
	I3_GAMENODE_ADDCHILD( this, CGameCameraManager::i() );
	CGameCameraManager::i()->Create( this, pPlayer);

#if defined( DEF_OBSERVER_MODE)
	if( BattleSlotContext::i()->isObserverMe())
		CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_OBSERVER);
	else
#endif
		CGameCameraManager::i()->ChangeCamera( CAMERA_TYPE_FPS);

	//스테이지에 따라서 카메라의 Far값을 다르게 설정해줍니다.
	i3StageInfo * pStageInfo = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex() );
	CGameCameraManager::i()->SetStageCamFar( pStageInfo->GetName());

	// 순서 중요. 순구
	BattleServerContext::i()->InitRoundData();	// 라운드 초기화를 먼저하고 오브젝트 데이터를 세팅합니다. (오브젝트는 레이어와 관계가 있다.)
	
	CGameMissionManager::i()->ProcessMissionPreStart();

	// 데이터 초기화 - 레이어 / 오브젝트 / HUD 관련 (구 HUD 관련 데이터를 쓰려면 여기서 초기화 해야됨)
	gmode::i()->InitialIzeStage();

	SetObjectPhysixEnable( true );	// Client도 Physix를 켜야됩니다.
	g_pFramework->getPhysixContext()->setEnableSimulate( true );

	//혈흔 관련 이펙트는 연령에 따라서 다른이펙트를 사용하여야 합니다.
	if( g_pEffectManager != nullptr)
	{
		g_pEffectManager->InitAgeLimitEffect();
	}

	// Shadow 설정
	g_pFramework->setShadowQuality( g_pEnvSet->m_nShadowQualityType, false);

	// 7032 : 오브젝트의 material은 적용하지 않도록. (일부 애니메이션 되는 오브젝트의 라이트를 조절하기 위하지만,
		// 너무 어두워지는 현상 및 라이트 컬리젼이 없는 맵에서는 문제가 됨.. swoongoo
	// 배치된 Object들의 Material을 조정한다.
	//_AdjustObjectMaterial();

	g_pSndMng->ResetRadioMessageInfo();

	CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_INGAME_HUD_READY_FOR_GAME"), 180.f);/*게임시작을 준비중입니다.*/

	CGameObjectManager::i()->EnableObject();

	{
		i3RenderContext * pCtx = g_pViewer->GetRenderContext();

		// 성능 향상을 위해 Color Buffer Clear를 Off
		// pCtx->removeClearFlag( I3G_CLEAR_COLOR);
		// 만약 Normal Map이 꺼져있다면 PPL을 Off한다.
		pCtx->SetPPLEnable( false);
	}

	return true;
}

bool CStageBattle::_OnLoadCommon_Step7(void)
{
	
	// 메시지 박스 초기화: 룸->배틀 진입 시 모든 메시지박스를 닫는다.
	GameUI::CloseAllMsgBox();

	// 레디 후 모드변경 통보 팝업 초기화
	MyRoomInfoContext::i()->SetManualReadyFlag(false);

	_ValidateShaders();

	// Checksum check
#if !defined( BUILD_RELEASE_QA_VERSION)
	{
		i3StageInfo * pStageInfo = g_pFramework->getStageInfo( g_pFramework->GetCurrentStageIndex() );
		char szPath[MAX_PATH];

		sprintf_s( szPath, "Pack/%s.i3Pack", pStageInfo->GetName());

		if( i3ResourceFile::CheckProtectionCode( szPath, "Zepetto Inc.", 12) == false)
		{
			/*게임 데이터 파일이 손상되어 게임을 계속할 수 없습니다. 종료합니다.*/
			GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STR_TBL_INGAME_HUD_NOTICE_GAME_DATA_DAMAGED"), nullptr, EXIT_CALLBACK(nullptr,true), this);

			g_pFramework->setControlEnable( false);
		}
	}
#endif

	g_pFramework->CreateBattleStageImageRes();
	
	if ( i3::same_of<CGameWorld*>(getWorld()))
	{
		CGameWorld * pWorld = (CGameWorld *)getWorld();

		//
		// 새로운 Collision Mode 확인
		//
		// Hit Collision으로 Physics Simulation을 하도록 하는 수정한 모드.
		//
		// Hit Collision i3s 파일의 파일명이 "...Hit2.i3s"로 끝날 경우, 새로운 Collision Mode를 사용하는 것으로 간주.
		{
			I3TRACE( "Hit : %s\n", pWorld->getHitCollision()->GetName());
			
			if( i3::icontain_string( pWorld->getHitCollision()->GetName(), "_Hit2.i3s") != -1)
				g_pFramework->InitializePhysXCollisionGroup( true);
			else
				g_pFramework->InitializePhysXCollisionGroup( false);
		}

		// 월샷 해킹 방지 코드 추가
		if ( pWorld->SafeBinded() == false )
		{
			HackContext::i()->HackingDetected(256.5f,0);
		}
	}

	HackContext::i()->resetHackUserSlot();

	return true;
}

void CStageBattle::_OnLoadDefault_Step1(void)
{
	config_render_battle::i()->ProcessCreateStage();
	gstatemgr::i()->ProcessCreateStage(this);

	_OnLoadCommon_Step1();

	m_BattleLoadState = BLS_STEP2;
}

void CStageBattle::_OnLoadDefault_Step2(void)
{
	if( _OnLoadCommon_Step2() )
		m_BattleLoadState = BLS_STEP3;
}

void CStageBattle::_OnLoadDefault_Step3(void)
{
	_OnLoadCommon_Step3();

	m_BattleLoadState = BLS_STEP4;
}

void CStageBattle::_OnLoadDefault_Step4(void)
{
	if( _OnLoadCommon_Step4() == true)
	{
		// 미리 들어와 있는 유저 캐릭터를 등록
		//bool bPreloadAnotherUser = MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_DOMINATION) || MyRoomInfoContext::i()->getStage()->IsAiModeforAiType(AI_USE_CHALLENGE);
		// 다른 유저를 직접 로딩하도록 설정.
		// 자신이 난입이기때문에 다른 유저는 보이도록 한다.
		BattleServerContext::i()->Battle_EntranceAnotherUser( true, true);//!bPreloadAnotherUser);

		m_BattleLoadState = BLS_STEP5;
	}
}

void CStageBattle::_OnLoadDefault_Step5(void)
{
	if( BattleServerContext::i()->LoadingAnotherCharacter() )
	{
		m_BattleLoadState = BLS_STEP6;
	}
}

void CStageBattle::_OnLoadDefault_Step6(void)
{
	_OnLoadCommon_Step6();

	m_BattleLoadState = BLS_STEP7;
}

void CStageBattle::_OnLoadDefault_Step7(void)
{
	_OnLoadCommon_Step7();

	// 순서 중요합니다. 마지막에 호출해줘야 합니다. - 순구.
	//화면에 네트워크를 검사하고 있다는 메시지를 보여준다.
	// Prestart를 보내기전에 서버에서 추가로 보내준 메시지가 있는지 검사한다.
	gstatemgr::i()->ProcessPreUpdate( 0.f);

	// 이미 게임이 끝났다면 보내지 말고 끝나기 프로세스로 넘어간다.
	if( gstatemgr::i()->getStageState() != GAMESTAGE_STATE_ENDGAME && gstatemgr::i()->IsLoadSuccess() )
	{
		if( BattleServerContext::i()->IsMovingStage() )
		{
			// EVENT_PRESTART_BATTLE 처리
			BattleServerContext::i()->P2PIntrudeBattle();
			gstatemgr::i()->ForceProcessEvent( EVENT_PRESTART_BATTLE, BattleSlotContext::i()->getMySlotIdx(), i3::user_data() );		

			// 홀펀칭이 끝나고 시작해야하기 때문에 필요하다.
			gstatemgr::i()->setStageState( GAMESTAGE_STATE_WAIT_HOLEPUNCHING );
		}
		else
		{
			GameEventSender::i()->SetEvent( EVENT_PRESTART_BATTLE );
		}
	}

	ISTEST_S
	{
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_PRESTART_BATTLE, 0);
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_START_BATTLE, 0);

		UserContext::i()->PlayingSlotFlag = 0x0001;
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_M_ROUNDSTART_BATTLE, 0);
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RESPAWN_BATTLE, 0);
		GameEventReceiver::i()->PushReceivedGameEvent(EVENT_RESPAWN_BATTLE, 1);
	}
	ISTEST_E

	m_BattleLoadState = BLS_COMPLETE;
}

bool CStageBattle::_IsDominationFlow(void)
{
	return false;
}

#ifdef DOMI_NEW_FLOW
void CStageBattle::_OnLoadStageJump_Step1(void)
{
	config_render_battle::i()->ProcessCreateStage();

	gstatemgr::i()->ProcessCreateRoundJumpStage(this);

	_OnLoadCommon_Step1();

	m_BattleLoadState = BLS_STEP2;
}

void CStageBattle::_OnLoadStageJump_Step2(void)
{
	if( _OnLoadCommon_Step2() )
		m_BattleLoadState = BLS_STEP3;
}

void CStageBattle::_OnLoadStageJump_Step3(void)
{
	_OnLoadCommon_Step3();

	m_BattleLoadState = BLS_STEP4;
}

void CStageBattle::_OnLoadStageJump_Step4(void)
{
	if( _OnLoadCommon_Step4() == true)
	{
		// 다른 유저를 직접 로딩하도록 설정.
		BattleServerContext::i()->Battle_EntranceAnotherUser( true);

		m_BattleLoadState = BLS_STEP5;
	}
}

void CStageBattle::_OnLoadStageJump_Step5(void)
{
	m_BattleLoadState = BLS_STEP6;
}

void CStageBattle::_OnLoadStageJump_Step6(void)
{
	_OnLoadCommon_Step6();
	m_BattleLoadState = BLS_STEP7;
}

void CStageBattle::_OnLoadStageJump_Step7(void)
{
	_OnLoadCommon_Step7();
	m_BattleLoadState = BLS_COMPLETE;
}

#endif

bool CStageBattle::OnFinish(void)
{
	bool bRet = i3Stage::OnFinish();

	i3Framework::FlushDXForThread();	// thread에서 처리하는 것을 삭제
	
	{
		// Lobby 등에서 캐릭터 그림자는 자동으로 동작하도록...
		i3SceneGraphContext * pCtx = g_pFramework->getSgContext();
		pCtx->setManualBoundEnable( false);

	}

	g_pFramework->SetReflectMap( nullptr);

	// Clear Flag의 원상복구
	{
		i3RenderContext * pCtx = g_pViewer->GetRenderContext();

		pCtx->addClearFlag( I3G_CLEAR_COLOR);
		pCtx->SetPPLEnable( true);
	}

	g_pFramework->DestroyBattleStageImageRes();

	gmode::i()->ProcessOnFinish();

	if( LocaleValue::Enable( "ForciblyRemove" ) )
	{
		if (UIBattleFrame::i())
			UIBattleFrame::i()->CloseAll();			// 강퇴관련 UI등을 꺼두는데 씁니다. 11.06.23.수빈.
	}

	//배틀 종료 후 서버 타입 표시 안함.
	g_pFramework->PrintVersion();

	return bRet;
}

static I3SG_TRAVERSAL_RESULT _CheckDynamicObjIsSleepingProc( i3Node * pNode, void * pUserData, i3MatrixStack * pStack )
{
	i3Transform2 * pTrans = (i3Transform2 *) pNode;
	i3PhysixShapeSet * pShapeSet = pTrans->getShapeSet();
	if((pShapeSet->getNxActor() != nullptr) && !pShapeSet->getNxActor()->isSleeping() )
	{
		*((bool*)pUserData) = true;
		return I3SG_TRAVERSAL_STOP;
	}

	return I3SG_TRAVERSAL_CONTINUE;
}

void CStageBattle::OnUpdate( REAL32 fDeltaTime)
{
	i3_prof_func();

	if ( CGameMaster::i()->IsPaused() )
	{
		fDeltaTime = 0.0f;
	}

	if(	gstatemgr::i()->getStageState() != GAMESTAGE_STATE_ENDGAME && !gstatemgr::i()->IsLoadSuccess() )
	{
		I3TRACE("CStageBattle::OnUpdate() - isLoadFailed\n");
		/*게임에 참여하지 못하였습니다.*/
		GameUI::MsgBox( MSG_TYPE_GAME_OK, GAME_RCSTRING("STBL_IDX_EP_BATTLE_FRIST_MAINLOAD"), nullptr, MAKE_CALLBACK(cbExitBattle), this);
		gstatemgr::i()->setStageState(GAMESTAGE_STATE_ENDGAME);
	}

	gstatemgr::i()->ProcessPreUpdate(fDeltaTime);
	i3Stage::OnUpdate( fDeltaTime);			//캐릭터, 무기, 카메라, 오브젝트 업데이트
	gstatemgr::i()->ProcessUpdate(fDeltaTime);

	// pre Update
	if(CQAWeaponCMgr::i()->getOnWeaponCollision())
	{
		CQAWeaponCMgr::i()->Update();
	}
}

bool CStageBattle::GetChatEnable(void)
{
	if(CHud::i())
		return CHud::i()->GetChatEnable();
	return false;
}

bool CStageBattle::IsEnableChatScroll(void)
{
	if(CHud::i())
		return CHud::i()->GetEnableChatScroll();
	return false;
}

bool CStageBattle::IsEnableGui(void)
{
	return HUD::instance()->IsEnableHud(HUD::RESPAWN_TIMER) || CBattleGui::i()->GetEnableGui(GAME_GUI_RESULT);
}

UINT32	CStageBattle::OnSave( i3ResourceFile * pResFile)
{
	UINT32 Result;//, Rc = 0;
//	i3Stream * pStream = pResFile->GetStream();

	Result = i3Stage::OnSave( pResFile);
	I3_CHKIO( Result);

	return Result;
}

UINT32	CStageBattle::OnLoad( i3ResourceFile * pResFile)
{
	UINT32 Result;//, Rc = 0;
//	i3Stream * pStream = pResFile->GetStream();

	Result = i3Stage::OnLoad( pResFile);
	I3_CHKIO( Result);

	return Result;
}

bool CStageBattle::OnEvent(UINT32 event,i3ElementBase* pObj,UINT32 param2, I3_EVT_CODE code)
{
	if( code == I3_EVT_CODE_ACTIVATE)
	{
		if (I3_EVT_INPUT == event)
		{
			//i3InputDeviceManager* pMgr = (i3InputDeviceManager*)pObj;

			if (CBattleGui::i())
			{
				if (CBattleGui::i()->OnEvent(event, pObj, param2, code))
				{
					return true;
				}
			}

			if (CHud::i())
			{
				if (CHud::i()->OnEvent(event, pObj, param2, code))
				{
					return true;
				}
			}

			if( UIBattleFrame::i() )
			{
				if( UIBattleFrame::i()->OnEvent( event, pObj, param2, code))
				{
					return true;
				}
			}
		}
	}

	return false;
}

void CStageBattle::SetObjectPhysixEnable( bool bValue )
{
	
	StageObjectListPtr	pObjectList = GetStageObjectList();

	if( pObjectList == nullptr )
		return;

	for( size_t i = 0; i < pObjectList->size(); i++ )
	{
		i3Object * pObj = (*pObjectList)[i];
	
		if( pObj->getCurrentHP() > 0.f )
		{
			if( BattleServerContext::i()->IsP2PHost() )
			{
				pObj->setDynamicActiveState( bValue == true);

				// dfly79
				// 방장 교체시 네트워크가 다이나믹모드이지만 Kinematic으로 설정되어 있던 오브젝트(Client에서는 Kinematic으로 세팅됨)는
				// 다이나믹으로 교체해줘야 한다.
				if( (pObj->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_DYNAMIC) &&
					(pObj->getDynamicState() == I3GAME_DYNAMIC_STATE_KINEMATIC) )
					pObj->SetDynamicState( I3GAME_DYNAMIC_STATE_DYNAMIC );
			}
			else
			{
				pObj->setDynamicActiveState( bValue == true);

				// Client는 네트워크가 Dynamic으로 되어 있으면 Kinematic으로 설정
				if( (pObj->getNetworkSyncMode() == I3_NETWORK_GAMEOBJECT_TYPE_DYNAMIC) &&
					(pObj->getDynamicState() == I3GAME_DYNAMIC_STATE_DYNAMIC) )
					pObj->SetDynamicState( I3GAME_DYNAMIC_STATE_KINEMATIC );
			}
		}
	}
}

void CStageBattle::_AdjustObjectMaterial(void)
{
	i3Object * pObj;
	
	StageObjectListPtr pList = GetStageObjectList();

	if( pList == nullptr)
		return;

	for(size_t i = 0; i < pList->size(); i++)
	{
		i3CollideeSphere sphere;
		VEC3D pos;
		INT32 count, j;
		COLORREAL target;

		pObj = (*pList)[i];

		i3Vector::Copy( &pos, pObj->GetPos());
		i3Vector::AddY( &pos, 0.5f );

		sphere.SetCenter( &pos);
		sphere.SetRadius( 1.0f);

		//I3TRACE( "%f %f %f %f\n", pos.x, pos.y, pos.z, m_rColRadius);
	
		count = g_pFramework->getCollisionContext()->CollisionTest( &sphere, CGRP_CHARA_LIGHTVOL, 0); //I3_COLLISION_STYLE_NEAREST_ONE);

		I3_COLLISION_RESULT * pResult = g_pFramework->getCollisionContext()->GetResult();

		if( count > 0)
		{
			i3Color::Set( &target, 1.0f, 1.0f, 1.0f, 1.0f);

			for( j = 0; j < count; j++)
			{
				REAL32 r, g, b;

				r = (REAL32)pResult->m_pTri->m_Color[0] * (1.0f / 255.0f) * 2.0f;
				g = (REAL32)pResult->m_pTri->m_Color[1] * (1.0f / 255.0f) * 2.0f;
				b = (REAL32)pResult->m_pTri->m_Color[2] * (1.0f / 255.0f) * 2.0f;

				i3Color::Add( &target, r, g, b, 1.0f);
				pResult ++;
			}

			i3Color::Scale( &target, &target, (1.0f / count));

			// Object는 너무 어두워져서 min값을 설정
			i3Color::SetR( &target, MIN( 0.8f, i3Color::GetR( &target)));
			i3Color::SetG( &target, MIN( 0.8f, i3Color::GetG( &target)));
			i3Color::SetB( &target, MIN( 0.8f, i3Color::GetB( &target)));
		}
		else
		{
			i3Color::Set( &target, config_render_battle::i()->getDefaultDiffuse() );
		}

		i3Scene::SetMaterial( pObj->getAttachNode(), &target);
	}
}

void CStageBattle::_LoadReflectMap( void)
{
	i3TextureCube * pTex;

	char szPath[ MAX_PATH];
	sprintf_s( szPath, "World/%s/%s_Ref.dds", GetName(), GetName());

	if( !i3FileStream::isFile( szPath))
	{
		g_pFramework->SetReflectMap( nullptr);
		return;
	}

	i3ImageFile file;
	pTex = (i3TextureCube*) file.Load( szPath);
	I3ASSERT( pTex != nullptr);

	g_pFramework->SetReflectMap( pTex);

	I3_MUST_RELEASE(pTex);
}

void CStageBattle::_LoadPostProcess()
{
	// 1. World_PostProcess.xml에 저장되어 있는 것들부터 먼저 추가
	char szPath[MAX_PATH];
	sprintf_s(szPath, "World/%s/%s_PostProcess.xml", GetName(), GetName());

	i3PostProcessManager * pManager = g_pViewer->GetPostProcessManager();

	if(i3System::IsFile(szPath))
	{
		i3XMLFile xmlFile;
		if(xmlFile.Load(szPath))
		{
			pManager->LoadPostProcessFromXML(&xmlFile);
		}
	}
	
	// 2. 그 이후, Framework가 따로 들고 있는 인게임 Post Process들을 매니저에 추가해준다.
	g_pFramework->AddInGamePostProcessToManager(pManager);
}

void CStageBattle::_CalcBoundForShadow(void)
{
}

void CStageBattle::_SubValidateShader( i3ShaderCode * pCode)
{
	i3RenderContext * pCtx = g_pViewer->GetRenderContext();

	// Lighting
	{
		pCode->setLightingEnable( true);

		// 0 Light
		{
			pCtx->ValidateShader( pCode);

			pCode->setVertexColorEnable( true);
			pCtx->ValidateShader( pCode);
			pCode->setVertexColorEnable( false);
		}

		// 1 Light
		{
			pCode->setLightModel( I3G_SHADER_FLAG_LIGHT_MODEL_PHONG);
			pCode->setLightType(	0, I3G_SHADER_FLAG_LIGHT_TYPE_DIRECTIONAL);

			pCtx->ValidateShader( pCode);

			pCode->setVertexColorEnable( true);
			pCtx->ValidateShader( pCode);
			pCode->setVertexColorEnable( false);

			// Reflection
			pCode->setReflectMapEnable( true);
			pCtx->ValidateShader( pCode);

			pCode->setReflectMaskMapEnable( true);
			pCtx->ValidateShader( pCode);

			pCode->setReflectMapEnable( false);
			pCode->setReflectMaskMapEnable( false);
		}

		// 2 Light
		{
			pCode->setLightType(	1, I3G_SHADER_FLAG_LIGHT_TYPE_POINT);

			pCtx->ValidateShader( pCode);

			pCode->setVertexColorEnable( true);
			pCtx->ValidateShader( pCode);
			pCode->setVertexColorEnable( false);

			// Reflection
			pCode->setReflectMapEnable( true);
			pCtx->ValidateShader( pCode);

			pCode->setReflectMaskMapEnable( true);
			pCtx->ValidateShader( pCode);

			pCode->setReflectMapEnable( false);
			pCode->setReflectMaskMapEnable( false);
		}
	}
}

void CStageBattle::_ValidateShaders(void)
{
	i3ShaderCode code;
	
	I3TRACE( "--------------------- PRE-SHADER VALIDATION ----------------------\n");
	i3Shader * pShader;
	INT32 i;
	i3RenderContext * pCtx = g_pViewer->GetRenderContext();
	i3ShaderCache * pCache = pCtx->getShaderCache();
	
	for( i = 0; i < pCache->getActiveShaderCount(); i++)
	{
		pShader = pCache->getActiveShader( i);

		if( pShader->isCustomShader())
			continue;

		if( g_pEnvSet->m_bEnableNormalMap)
		{
			if( pShader->getShaderCode()->isPPLShader())
				pShader->Validate( pCtx);
		}
		else
		{
			if( pShader->getShaderCode()->isPPLShader() == false)
				pShader->Validate( pCtx);
		}
	}
	
	
	I3TRACE( "--------------------------------------------------------------\n");
}

void CStageBattle::_InitEdgeTrail(void)
{
	I3ASSERT( g_pEdgeTrail == nullptr);

	// 칼궤적은 1개만 생성되면 되기 때문에...
	// WeaponKnife의 멤버로 넣지 않고 StageBattle에 둔다.
	g_pEdgeTrail = CEdgeTrailer::new_object();
	g_pEdgeTrail->Create();

	g_pEdgeTrail->SetEnable( false);
}

i3Object* CStageBattle::GetI3Object(i3ClassMeta* pMeta)
{
	const size_t iObjCount = GetStageObjectList()->size();

	for( size_t j = 0 ; j <iObjCount; j++)
	{
		i3Object * pObj = (*GetStageObjectList())[j];
		if( pObj->same_of( pMeta) )
		{
			return pObj;
		}
	}

	return nullptr;
}

i3Object* CStageBattle::GetI3Object(INT32 iIdx)
{
	if(GetStageObjectList() == nullptr)
	{
		return nullptr;
	}

	INT32 iObjCount = GetStageObjectList()->size();

	I3_BOUNDCHK(iIdx, iObjCount);
	
	i3Object * pObj = (*GetStageObjectList())[iIdx];
	
	return pObj;
}

bool CStageBattle::OnRevive( i3RenderContext * pCtx)
{
	if( i3Stage::OnRevive( pCtx) == false)
		return false;
	//	조합된 클랜마크 텍스처 복원
	//g_pClanMarkManager->OnRevive( pCtx);	// CGameFramework::OnRevive 로 이동. 아웃게임 UI에서도 사용하기 때문.

	return true;
}


//LSR_AVATAR_SYSTEM
//일단 캐릭터가 없을때 에러나는 것들을 모아두자
void CStageBattle::sAfterLoadChara( void * pAvatar, INT32 idx /*= -1*/)
{
	TRACE("===========================\n");
	TRACE("StageBattle AfterLoadChara\n");
	TRACE("===========================\n");

	if( g_pFramework == nullptr ||
		(g_pFramework->IsBattleStage() == false) )
		return;

	CGameCharaBase * pPlayer = nullptr;

	//현재 한방에 로드를 해두기때문에..
	//////////////////////////////////////////////
ISTEST_S
	{
		//여긴 콘솔모드 테스트 이므로0->1 순서대로 생성되어 들어오므로..
		//첫째 셋팅때는 넘어가도록 한다..(뒤에 우루루 셋팅하므로)
		pPlayer = g_pCharaManager->AddChara( BattleSlotContext::i()->getMySlotIdx(), g_DebugChara[0] );
		I3ASSERT( pPlayer != nullptr);

		g_pFramework->getPhysixContext()->setEnableSimulate( true );
	}
	else
ISTEST_E
	{
		UI_CHARA_INFO	* pNetCharaInfo = UserInfoContext::i()->SetMyUICharaInfo();
		CCharaInfoContext::i()->GetUICharaInfo( pNetCharaInfo );
		// vv3 - check
		CHARA_RES_ID resID = (CHARA_RES_ID) pNetCharaInfo->getCharaParts()->GetPartsItemID(CHAR_EQUIPMENT_PARTS_CHARA);
		pPlayer = g_pCharaManager->AddChara( BattleSlotContext::i()->getMySlotIdx(), resID);

		
		// Visible로 처리 - 박명진 2008. 7. 17
		if( pPlayer != nullptr)
			pPlayer->SetVisible( false );
	}
}

void CStageBattle::cbExitBattle(void *pThis, int nParam)
{
	INT32 nArg = 0;
	GameEventSender::i()->SetEvent( EVENT_GIVEUP_BATTLE, &nArg );
	CGameFramework::JumpToReady();
}