#include "pch.h"
//#include "ChallengeBoard.h"
#include "TutorialMode.h"
#include "UI/HUD/UI_HUD_TutorialMode.h"
#include "BattleHud.h"
#include "StageObject/TargetInTutorial.h"
#include "GameCharaWeaponContext.h"
#include "GameObject.h"
#include "StageBattle.h"
#include "WeaponM197.h"

#include "UserInfoContext.h"

#include "UI/UIHudManager.h"
#include "BattleSlotContext.h"
#include "TutorialContext.h"
#include "GameMissionManager.h"

I3_CLASS_INSTANCE(CTutorialMode);//, i3ElementBase);

CTutorialMode::CTutorialMode() : m_rStateTime(0.0f)
								, m_bShowKeyDesc(false)
								, m_bNeedReloadState(false)
								, m_rNeedReloadStateTime(0.0f)
								, m_iAppearChallengeClearEffectCount(0)
								, m_bEndByTimeOut(false)
{
	m_eTutorialState = TS_NOT_SET;
	m_InfoEquipWeaponBackup.Reset();
	m_iEndCountInTutorial = 0;
	m_bOnceFlag = false;
	m_bOnceFlag2 = false;
	m_pHudTutorial = nullptr;

	m_pTutorialSound[0]	= nullptr;
	m_pTutorialSound[1]	= nullptr;
}

CTutorialMode::~CTutorialMode()
{

}

void CTutorialMode::OnCreate(i3GameNode * pParent)
{
	USER_INFO_EQUIPMENT_WEAPON	InfoEquipWeapon;
	UserInfoContext::i()->GetMyUserInfoEquipmentWeapon(InfoEquipWeapon);
	m_InfoEquipWeaponBackup = InfoEquipWeapon;

	for(INT32 i=0; i<TTAT_COUNT; i++)
	{
		m_vecTarget[i].clear();
	}

	CGameMissionManager::i()->setMyMissionObject( MY_MISSION_OBJECT_OFFENSIVE );

	m_pHudTutorial = UI_HUD_Tutorial::new_object();
	m_pHudTutorial->OnCreate( pParent);
	m_iEndCountInTutorial = 0;
	m_bShowKeyDesc = true;
	m_bNeedReloadState = false;
	
	LoadTutorialSound();

	TutorialContext::i()->SetOldTutorialIng();
	TutorialContext::i()->SetWasTutorial(true);

	m_bEndByTimeOut = false;
}

void CTutorialMode::OnFinish()
{
	m_pHudTutorial->OffAllTutorialHUD();
	m_pHudTutorial->Destroy();
	m_pHudTutorial=nullptr;
	ReleaseTutorialSound();
}

void CTutorialMode::FirstTickInTutorialState()
{
	// GlobalFunc::PB_TRACE("m_eTutorialState %d", m_eTutorialState);

	if(m_eTutorialState == TS_START)
	{
		if(m_iEndCountInTutorial == 0)
		{
			m_iAppearChallengeClearEffectCount = 0;
		}
	}

	if(m_iEndCountInTutorial == 1)
	{
		INT32 idx = -1;
		switch(m_eTutorialState)
		{
		case TS_START:		idx = 28;		break;

		case TS_MOVE:		case TS_AVOID_HURDLE:		case TS_JUMP:		case TS_PREPARE_SHOOT:		case TS_SHOOT:
		case TS_PREPARE_SHOOT_EXT:		case TS_TARGET_LOCKON:		case TS_SHOOT_EXT:		case TS_PREPARE_GRENADE:
		case TS_SWAP_GRENADE:		case TS_THROW_GRENADE:		case TS_THROW_SMOKE:		case TS_PREPARE_GRENADE_LAUNCHER:
		case TS_SHOOT_GRENADE_LAUNCHER:		case TS_PREPARE_C4:		case TS_SWAP_C4:		case TS_INSTALL_C4:
		case TS_REMOVE_C4:		case TS_PREPARE_MELEE:		case TS_SWAP_MELEE:		case TS_MELEE:
		case TS_MOVE_TO_HELICOPTER:		case TS_BOARD_HELICOPTER:		case TS_SWAP_M187:		case TS_SHOOT_M187:
		case TS_RELEASE_M187:		case TS_MOVE_TO_EXIT:
			break;

		case TS_END_TIME_ATTACK:	idx = 22;	break;
		}

		if( idx != -1 && HUD::instance())
		{
			HUD::instance()->SetParameter(HUD::NOTICE, HUD::NMT_Tutorial, idx);
		}
	}
	else if(m_iEndCountInTutorial == 0)
	{
		INT32 idx = -1;

		switch(m_eTutorialState)
		{
		case TS_MOVE:						///< 움직이기 시작해서 처음 화살표를 먹기전 상태
		case TS_AVOID_HURDLE:				///< 처음 화살표를 먹고 장애물을 피하는 구간
		case TS_JUMP:						///< 점프 구간
		case TS_PREPARE_SHOOT:				///< 점프 구간을 통과하고 사격장 앞 화살표를 먹기 전 상태
		case TS_SHOOT:						///< 실제 사격을 할 수 있는 상태. 
		case TS_PREPARE_SHOOT_EXT:			///< 첫번째 사격장의 타겟을 모두 쓰러뜨린 후. 
			idx = m_eTutorialState;
			m_bOnceFlag = true;
			m_bOnceFlag2 = true;
			break;
		case TS_TARGET_LOCKON:				///< 익스텐션 사격장 앞 화살표를 먹고 난 후 사격이 가능하나 조준경을 off한 상태
		case TS_SHOOT_EXT:					///< 익스텐션 사격장 앞 화살표를 먹고 난 후 실제로 익스텐션 사격이 가능한 상태. 
			if(m_bOnceFlag)
			{
				idx = TS_TARGET_LOCKON;
				m_bOnceFlag = false;
			}
			break;
		case TS_PREPARE_GRENADE:				///< 타겟이 모두 쓰러진 후 다음 화살표를 먹기 전 상태. 
			idx = 8;
			m_bOnceFlag = true;
			break;
		case TS_SWAP_GRENADE:				///< 수류탄으로 타겟을 쓰러뜨릴 수 있으나 주무기가 수류탄이 아닐경우
		case TS_THROW_GRENADE:				///< 투척 사격장 앞 화살표 먹고 난 후 실제로 수류탄으로 타겟을 쓰러뜨릴 수 있는 상태. 
			if(m_bOnceFlag)
			{
				idx = 9;
				m_bOnceFlag = false;
			}
			break;
		case TS_PREPARE_GRENADE_LAUNCHER:	///< 수류탄 투척후 유탄 준비 상태 설명 문구를 위해 있는 상태 3초후에 다음 상태로 바뀜.>
		case TS_SHOOT_GRENADE_LAUNCHER:	///< 유탄을 쏴서 타겟을 쓰러뜨릴 수 있는 상태. 
			if(m_bOnceFlag2)
			{
				idx = 12;
				m_bOnceFlag2 = false;
			}
			break;
		case TS_PREPARE_C4:						///< 유탄으로 모든 타겟을 넘어뜨렸다면 이 상태가 된다. 
			idx = 13;
			m_bOnceFlag = true;
			break;
		case TS_SWAP_C4:						///< 다른 무기를 들고있을시 C4로 교체하라는 메세지
		case TS_INSTALL_C4:						///< 폭탄 설치 지역에 있는 화살표를 먹으면 이 상태가 된다. 
			if(m_bOnceFlag)
			{
				idx = 14;
				m_bOnceFlag = false;
			}
			break;
		case TS_REMOVE_C4:					///< 폭탄 설치가 완료되면 이 상태가 된다. 
			idx = 15;
			break;
		case TS_PREPARE_MELEE:				///< 근접 무기 훈련 통로로 이동하라는 메세지
			idx = 16;
			break;
		//case TS_SWAP_MELEE:					///< 현재 무기가 보조무기가 아닐경우 메세지
		//case TS_MELEE:						///< 폭탄 설치를 완료하고 칼로 타겟을 넘어뜨릴 수 있는 상태. 
		case TS_MOVE_TO_HELICOPTER:			///< 칼로 제거할 수 있는 타겟이 모두 쓰려지면 되는 상태. 
			idx = 17;
			break;
		case TS_BOARD_HELICOPTER:			///< 헬기 위 화살표를 먹으면 이 상태가 된다. 
			idx = 18;
			m_bOnceFlag = true;
			break;
		case TS_SWAP_M187:					///< 헬기위에서 M187을 장착하고 있지 않을시 이상태가 된다.
		case TS_SHOOT_M187:					///< E키를 눌러 헬기에 탑승하면 이 상태가 된다. 
			if(m_bOnceFlag)
			{
				idx = 19;
				m_bOnceFlag = false;
			}
			break;
		case TS_RELEASE_M187:				///< M197 사격 종료후 E키를 눌러 해제하라는 메세지.
		case TS_MOVE_TO_EXIT:				///< 해제하고 난 후 도착지점으로 이동하라는 메세지. 
			idx = 20;
			break;
		case TS_END:							///< 마지막 화살표를 먹고 난 후 상태 (처음 튜토리얼을 하는 거라면). 
			idx = 21;
			break;
		}

		if( idx != -1 && HUD::instance())
		{
			HUD::instance()->SetParameter(HUD::NOTICE, HUD::NMT_Tutorial, idx);
		}
	}
	else
	{
		I3ASSERT_0;
	}
	
	//TurnOnKeyDescription();

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	I3ASSERT( pPlayer != nullptr);

	if(CHud::i() != nullptr)
	{
		switch(m_eTutorialState) // 튜토리얼 모드 : 화면 중앙 안내 텍스트
		{
		case TS_MOVE:
			{
				pPlayer->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_MOVE_1"), 1800.f, 0, 0, true);
				
				m_pHudTutorial->OnOffTutorialHUD(TH_MISSION_START, true);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_MOVE, true);
			}
			break;
		case TS_AVOID_HURDLE:
			{
				m_pHudTutorial->OnOffTutorialHUD(TH_MISSION_START, false);
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_MOVE_2"), 1800.f);
			}
			break;
		case TS_JUMP:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_MOVE_3"), 1800.f);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_MOVE, false);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_JUMP, true);
			}
			break;
		case TS_PREPARE_SHOOT:
			{
				//이동하라로 교체해야될듯
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_MOVE_4"), 1800.f);
				//m_pHudTutorial->OnOffTutorialHUD(TH_KEY_JUMP, false);
				//m_pHudTutorial->OnOffTutorialHUD(TH_KEY_SHOOT, true);
			}
			break;
		case TS_SHOOT:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_ATTACK_1"), 1800.f);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_JUMP, false);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_SHOOT, true);
				if(m_iEndCountInTutorial == 0 && m_eTutorialState == TS_PREPARE_SHOOT && m_iAppearChallengeClearEffectCount == 0)
				{
					CHud::i()->AddChallengeClear();
					m_iAppearChallengeClearEffectCount++;
				}
			}
			break;
		case TS_PREPARE_SHOOT_EXT:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_MOVE_5"), 1800.f);
				
			}
			break;
		case TS_TARGET_LOCKON: // 화살표를 밟았을시 진입
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_ATTACK_2_1"), 1800.f);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_SHOOT, false);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_EXT, true);
			}
			break;
		case TS_SHOOT_EXT:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_ATTACK_2_2"), 1800.f);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_SHOOT, false);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_EXT, true);
			}
			break;
		case TS_PREPARE_GRENADE:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_ATTACK_3_1"), 1800.f);
			}
			break;
		case TS_SWAP_GRENADE:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_ATTACK_3_2"), 1800.f);
			}
			break;
		case TS_THROW_GRENADE:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_ATTACK_3_3"), 1800.f);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_EXT, false);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_GRENADE_SMOKE, true);
			}
			break;
		//case TS_THROW_SMOKE:
		case TS_PREPARE_GRENADE_LAUNCHER:
			{
				i3::vector<CTargetInTutorial*>::iterator itor;
				for(itor=m_vecTarget[TTAT_GRENADE].begin(); itor!=m_vecTarget[TTAT_GRENADE].end(); itor++)
				{
					CTargetInTutorial* pTarget = *itor;
					if(pTarget->GetTargetState() != TAS_IDLE)
					{
						pTarget->SetTargetState(TAS_RETURN);	// Target changed normal state for shooting again.
					}
				}
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_GRENADE_SMOKE, false);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_GRENADE_LAUNCHER, true);
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_ATTACK_5_1"), 1800.f);
			}
			break;
		case TS_SHOOT_GRENADE_LAUNCHER:
			{
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_GRENADE_LAUNCHER, true);
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_ATTACK_5_2"), 1800.f);
			}
			break;
		case TS_PREPARE_C4:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_1_1"), 1800.f);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_GRENADE_LAUNCHER, false);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_C4, true);
				if(m_iEndCountInTutorial == 0 && m_iAppearChallengeClearEffectCount == 1)
				{
					CHud::i()->AddChallengeClear();
					m_iAppearChallengeClearEffectCount++;
				}
			}
			break;
		case TS_SWAP_C4:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_1_2"), 1800.f);
			}
			break;
		case TS_INSTALL_C4:
			{	
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_1_3"), 1800.f);
			}
			break;
		case TS_REMOVE_C4:
			{
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_C4, true);
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_1_4"), 1800.f);
			}
			break;
		case TS_PREPARE_MELEE:
			{
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_C4, false);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_MELLE, true);
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_2_1"), 1800.f);
			}
			break;
		case TS_SWAP_MELEE:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_2_2"), 1800.f);
			}
			break;
		case TS_MELEE:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_2_3"), 1800.f);	
			}
			break;
		case TS_MOVE_TO_HELICOPTER:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_3_1"), 1800.f);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_MELLE, false);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_HELICOPTER, true);
			}
			break;
		case TS_BOARD_HELICOPTER:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_3_2"), 1800.f);
			}
			break;
		case TS_SWAP_M187:
			{
				CHud::i()->SetMissionNoticeString(GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_3_3"), 1800.f);
			}
			break;
		case TS_SHOOT_M187:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_3_4"), 1800.f);
			}
			break;
		case TS_RELEASE_M187:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_3_5"), 1800.f);
			}
			break;
		case TS_MOVE_TO_EXIT:
			{
				CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_END_MESSAGE"), 1800.f);
			}
			break;
		case TS_END:
			{
				CHud::i()->SetEnableMissionNotice(false);
				m_pHudTutorial->OnOffTutorialHUD(TH_KEY_HELICOPTER, false);
				if(m_iEndCountInTutorial == 0 && m_iAppearChallengeClearEffectCount == 2)
				{
					CHud::i()->AddChallengeClear();
					m_iAppearChallengeClearEffectCount++;
				}

				GameEventSender::i()->SetEventOnce(EVENT_TUTORIAL_ROUND_END);
			}
			break;
		case TS_END_TIME_ATTACK:
			{
				CHud::i()->AddChallengeClear();
				GameEventSender::i()->SetEventOnce(EVENT_TUTORIAL_ROUND_END);
			}
			break;
		}
	}
}


void CTutorialMode::SetTutorialState(TUTORIAL_STATE eState)
{
	m_eTutorialState = eState;
	m_rStateTime = 0.f;
}	

void CTutorialMode::ChangeNextState(TUTORIAL_STATE eState)
{
	TUTORIAL_STATE eNextState = (TUTORIAL_STATE)(m_eTutorialState+1);

	if(eNextState == eState)
	{
		SetTutorialState(eState); 
	}
}

bool CTutorialMode::ChangeNextStateByArrowInTutorial(INT32 iIndex)
{
	// GlobalFunc::PB_TRACE("ChangeNextStateByArrowInTutorial %d", m_eTutorialState);

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	I3ASSERT( pPlayer != nullptr);

	switch(m_eTutorialState) // 튜토리얼 모드 : 게임내 화살표 밟았을시 처리
	{
	case TS_START:																			
		break;
	case TS_MOVE:							
		if(iIndex == 0) {
			SetTutorialState(TS_AVOID_HURDLE); 				
			return true;
		}
		break;
	case TS_AVOID_HURDLE:	
		if(iIndex == 1) {
			SetTutorialState(TS_JUMP);	
			return true;
		}
		break;
	case TS_JUMP:							
		if(iIndex == 2) {
			SetTutorialState(TS_PREPARE_SHOOT);				
			return true;
		}
		break;
	case TS_PREPARE_SHOOT:					
		if(iIndex == 3) {
			SetTutorialState(TS_SHOOT);						
			return true;
		}
		break;
	case TS_PREPARE_SHOOT_EXT:				
		if(iIndex == 4) {
			SetTutorialState(TS_TARGET_LOCKON);					
			return true;
		}
		break;
	case TS_PREPARE_GRENADE:				
		if(iIndex == 5) {
			{
				WeaponBase * pWeapon = pPlayer->GetCurrentCharaWeapon();
				if( pWeapon != nullptr && pWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_THROWING1)
					SetTutorialState(TS_THROW_GRENADE);
				else
					SetTutorialState(TS_SWAP_GRENADE);
			}
			return true;
		}
		break;
	case TS_PREPARE_C4:						
		if(iIndex == 6) {
			SetTutorialState(TS_SWAP_C4);				
			return true;
		}
		break;
	case TS_INSTALL_C4:						
		//if(iIndex == 0) {
		//	SetTutorialState(TS_REMOVE_C4);					
		//	return true;
		//}
		break;
	case TS_REMOVE_C4:						
		//if(iIndex == 0) {
		//	SetTutorialState(TS_MELEE);						
		//	return true;
		//}
		break;
	case TS_PREPARE_MELEE:							
		if(iIndex == 9) {
			SetTutorialState(TS_MELEE);
			return true;
		}
		break;
	case TS_MOVE_TO_HELICOPTER:				
		if(iIndex == 10){	
			SetTutorialState(TS_BOARD_HELICOPTER);
			return true;
		}
		break;
	case TS_BOARD_HELICOPTER:				
		if(iIndex == 7) {
			SetTutorialState(TS_SWAP_M187);
			return true;
		}
		break;
	case TS_SHOOT_M187:						
		if(iIndex == 0) {
			SetTutorialState(TS_MOVE_TO_EXIT);				
			return true;
		}
		break;
	case TS_MOVE_TO_EXIT:		
		if(m_iEndCountInTutorial == 1) 
		{
			if(iIndex == 8) {
				SetTutorialState(TS_END_TIME_ATTACK);						
				return true;
			}
		}
		else if(m_iEndCountInTutorial == 0) 
		{
			if(iIndex == 8) {
				SetTutorialState(TS_END);						
				return true;
			}
		}
		
		break;
	}
		
	return false;
}	

void CTutorialMode::AddTargetInTutorial(CTargetInTutorial* pTarget)
{
	// NetIndex로 타겟이 어느 지역의 타겟인지 구별한다.
	// 레벨 툴에서 Type를 지정해서 구별하는 방식으로 바꿀 필요성이 있음.
	
	if(pTarget->getNetIndex() == 0xa || pTarget->getNetIndex() == 0xb || pTarget->getNetIndex() == 0xc || pTarget->getNetIndex() == 0xd)
	{
		m_vecTarget[TTAT_SHOOT].push_back(pTarget);
	}
	else if(pTarget->getNetIndex() == 0xe || pTarget->getNetIndex() == 0xf || pTarget->getNetIndex() == 0x10 || pTarget->getNetIndex() == 0x11)
	{
		m_vecTarget[TTAT_SHOOT_EXT].push_back(pTarget);
	}
	else if(pTarget->getNetIndex() == 0x12 || pTarget->getNetIndex() == 0x13 || pTarget->getNetIndex() == 0x14 || pTarget->getNetIndex() == 0x15 || pTarget->getNetIndex() == 0x16)
	{
		m_vecTarget[TTAT_GRENADE].push_back(pTarget);
	}
	else if(pTarget->getNetIndex() == 0x17 || pTarget->getNetIndex() == 0x18 || pTarget->getNetIndex() == 0x19 || pTarget->getNetIndex() == 0x1a)
	{
		m_vecTarget[TTAT_MELEE].push_back(pTarget);
	}
	else
	{
		I3ASSERT_0;
	}
	
}

bool CTutorialMode::IsAllTargetDown(TUTORIAL_TARGET_AREA_TYPE eType)
{
	i3::vector<CTargetInTutorial*>::iterator itor;

	for(itor=m_vecTarget[eType].begin(); itor!=m_vecTarget[eType].end(); itor++)
	{
		CTargetInTutorial* pTarget = *itor;
		if(pTarget->GetTargetState() != TAS_DOWN_IDLE)
		{
			return false;
		}
	}

	return true;
}

bool CTutorialMode::IsAllTargetNotIdle(TUTORIAL_TARGET_AREA_TYPE eType)
{
	i3::vector<CTargetInTutorial*>::iterator itor;

	for(itor=m_vecTarget[eType].begin(); itor!=m_vecTarget[eType].end(); itor++)
	{
		CTargetInTutorial* pTarget = *itor;
		if(pTarget->GetTargetState() == TAS_IDLE)
		{
			return false;
		}
	}

	return true;
}
void CTutorialMode::CheckTargetState()
{
	switch(m_eTutorialState)
	{
	case TS_SHOOT:
		{
			if(IsAllTargetDown(TTAT_SHOOT)) {
				SetTutorialState(TS_PREPARE_SHOOT_EXT);
			}
		}
		break;
	case TS_SHOOT_EXT:
		{
			if(IsAllTargetDown(TTAT_SHOOT_EXT)) {
				SetTutorialState(TS_PREPARE_GRENADE);
			}
		}
		break;
	case TS_MELEE:
		{
			if(IsAllTargetDown(TTAT_MELEE)) {
				SetTutorialState(TS_MOVE_TO_HELICOPTER);
			}
		}
		break;
	case TS_SHOOT_M187:
		{
			i3::vector<i3Object*>::iterator itor;

			for(itor=m_vecDrumInTutorial.begin(); itor!=m_vecDrumInTutorial.end(); itor++)
			{
				i3Object* pTarget = *itor;
				if(pTarget->getCurrentHP() != 0.f)
				{
					return;
				}
			}
			SetTutorialState(TS_RELEASE_M187);
		}
		break;
	}
}

bool CTutorialMode::IsAvailableTargetOnTutorialState(TUTORIAL_TARGET_AREA_TYPE eType, INT32 idx)
{
	i3::vector<CTargetInTutorial*>::iterator itor;

	for(itor=m_vecTarget[eType].begin(); itor!=m_vecTarget[eType].end(); itor++)
	{
		CTargetInTutorial* pTarget = *itor;
		if(pTarget->getNetIndex() == idx)
		{
			return true;
		}
	}

	return false;
}

bool CTutorialMode::IsTutorialPrimaryWeapon(WeaponBase* pObj)
{
	CWeaponInfo* pInfo = pObj->getWeaponInfo();

	// ak 풀커스텀
	if(pInfo->GetNumber() == 58 && pInfo->GetTypeClass() == WEAPON_CLASS_ASSAULT)
	{
		return true;
	}

	return false;
}

bool CTutorialMode::IsAvailableTargetOnTutorialStateOnWeapon(INT32 idx, i3GameObj* pObj)
{
	WeaponBase* pWeapon = nullptr;

	if(i3::kind_of<WeaponBase*>(pObj))
	{
		pWeapon = (WeaponBase*)pObj;
	}
	else
	{
		return false;
	}
	switch(m_eTutorialState)
	{
	case TS_SHOOT:
		{
			if(IsAvailableTargetOnTutorialState(TTAT_SHOOT, idx) == true)
			{
				if(IsTutorialPrimaryWeapon(pWeapon)) {
					return true;
				}
			}
		}
		break;
	case TS_SHOOT_EXT:
		{
			if(IsAvailableTargetOnTutorialState(TTAT_SHOOT_EXT, idx) == true)
			{
				if(IsTutorialPrimaryWeapon(pWeapon)) {
					return true;
				}
			}
		}
		break;
	case TS_SWAP_GRENADE:
	case TS_THROW_GRENADE:
	case TS_PREPARE_GRENADE_LAUNCHER:
	case TS_SHOOT_GRENADE_LAUNCHER:
		{
			if(IsAvailableTargetOnTutorialState(TTAT_GRENADE, idx) == true)
			{
				CWeaponInfo* pInfo = pWeapon->getWeaponInfo();

				if(m_eTutorialState == TS_THROW_GRENADE || m_eTutorialState == TS_SWAP_GRENADE)
				{
					// 수류탄
					if(pInfo->GetNumber() == 1 && pInfo->GetTypeClass() == WEAPON_CLASS_THROWING_GRENADE)
					{
						return true;
					}
				}
				else if(m_eTutorialState == TS_SHOOT_GRENADE_LAUNCHER || m_eTutorialState == TS_PREPARE_GRENADE_LAUNCHER)
				{
					// 유탄
					if(pInfo->GetNumber() == 8 && pInfo->GetTypeClass() == WEAPON_CLASS_GRENADESHELL)
					{
						return true;
					}
				}
				else
				{
					I3ASSERT_0;
				}
			}
		}
		break;
	case TS_MELEE:
	case TS_SWAP_MELEE:
		{
			if(IsAvailableTargetOnTutorialState(TTAT_MELEE, idx) == true)
			{
				CWeaponInfo* pInfo = pWeapon->getWeaponInfo();
				if(pInfo->GetNumber() == 1 && pInfo->GetTypeClass() == WEAPON_CLASS_KNIFE)
				{
					return true;
				}
			}
		}
		break;
	case TS_SHOOT_M187:
		{
			i3::vector<i3Object*>::iterator itor;

			for(itor=m_vecDrumInTutorial.begin(); itor!=m_vecDrumInTutorial.end(); itor++)
			{
				i3Object* pObj2 = *itor;
				if(pObj2->getNetIndex() == idx)
				{
					CWeaponInfo* pInfo = pWeapon->getWeaponInfo();
					if(pInfo->GetNumber() == 1 && pInfo->GetTypeClass() == WEAPON_CLASS_OBJECT_GUN)
					{
						return true;
					}
				}
			}
		}
		break;
	}

	return false;
}

void CTutorialMode::CheckGrenadeInTutorial(VEC3D& v3GrenadePos, INT32 iWeaponNum)
{
	// 수류탄
	if(iWeaponNum == 1)
	{
		if(m_eTutorialState >= TS_PREPARE_GRENADE_LAUNCHER)
		{
			return;
		}

		if(IsAllTargetNotIdle(TTAT_GRENADE) == false)
		{
			RefillAmmo();
		}
		else
		{
			if(m_eTutorialState == TS_THROW_GRENADE || m_eTutorialState == TS_SWAP_GRENADE)
			{
				SetTutorialState(TS_PREPARE_GRENADE_LAUNCHER);
			}
			else
			{	
				RefillAmmo();
				//목표물이 전부 쓰러지지 않았다.
				SetTutorialState(TS_SWAP_GRENADE);
			}
		}
	}
	// 스모크 탄
	/*else if(iWeaponNum == 2)
	{
		if(m_eTutorialState >= TS_PREPARE_GRENADE_LAUNCHER)
		{
			return;
		}

		i3Vector::Set(&v3Pos, -28.84f, -10.14f, 6.725f);
		i3Vector::Sub(&v3Len, &v3GrenadePos, &v3Pos);
		REAL32 rLen = i3Vector::Length(&v3Len);

		if(rLen < 2.8f)
		{
			if(m_eTutorialState == TS_THROW_SMOKE)
			{
				SetTutorialState(TS_PREPARE_GRENADE_LAUNCHER);
			}
			else
			{
				RefillAmmo();
			}
		}
		else
		{
			RefillAmmo();

		}
	}*/
	// 유탄
	else if(iWeaponNum == 8)
	{
		if(m_eTutorialState >= TS_PREPARE_C4)
		{
			return;
		}

		if(IsAllTargetNotIdle(TTAT_GRENADE) == false)
		{
			RefillAmmo();
		}
		else
		{
			if(m_eTutorialState == TS_SHOOT_GRENADE_LAUNCHER || m_eTutorialState == TS_PREPARE_GRENADE_LAUNCHER)
			{
				SetTutorialState(TS_PREPARE_C4);
			}
			else
			{	
				RefillAmmo();
			}
		}
	}
}

void CTutorialMode::RefillAmmo()
{
	// 죽은 상태에서는 출력하지 않는다.
	// 죽는 상태의 메세지가 우선
	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

	if( pPlayer == nullptr || pPlayer->isCharaStateMask( CHARA_STATE_DEATH))
		return;

	const NET_CHARA_INFO * pNetCharaInfo = BattleSlotContext::i()->getNetCharaInfo( BattleSlotContext::i()->getMySlotIdx());
	if ( pNetCharaInfo == nullptr) return;

	if(HUD::instance())
		HUD::instance()->SetParameter(HUD::NOTICE, HUD::NMT_Tutorial, 27);

	if(CHud::i() != nullptr)
		CHud::i()->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_NEED_RELOAD_CENTER_DESC"), 1800.f);

	g_pCharaManager->SetCharaWeaponSet( pPlayer, pNetCharaInfo->getWeaponList(WEAPON_SLOT_PRIMARY));
	m_rNeedReloadStateTime = 0.f;
	m_bNeedReloadState = true;
	if( pPlayer->IsDino() == false)
	{
		//	처음 리스폰시 0번 무기로 교체하는 동작한다.
		pPlayer->getWeaponContext()->_Respawn();
	}
	WeaponBase * pBase = pPlayer->getWeaponContext()->getWeapon(WEAPON_SLOT_PRIMARY);
	if(i3::kind_of<WeaponGrenadeLauncher*>(pBase))
	{
		WeaponGrenadeLauncher * pLauncher  = (WeaponGrenadeLauncher*)pBase;
		pLauncher->setLoadedShellCount(1);
	}

	pBase = pPlayer->getWeaponContext()->getWeapon(WEAPON_SLOT_THROWING1);
	pBase->setLoadedBulletCount(1);
}


void CTutorialMode::UpdateTutorialState(REAL32 rDeltaTime)
{
	if(m_rStateTime == 0.f)
	{
		FirstTickInTutorialState();
	}

	CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());
	if( pPlayer == nullptr)
		return;
	if( pPlayer->getWeaponContext() == nullptr)
		return;

	WeaponBase * pWeapon = pPlayer->GetCurrentCharaWeapon();
	if( pWeapon != nullptr )
	{
		if(pWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_PRIMARY && pWeapon->getLoadedBulletCount() == 0 && pWeapon->getTotalBulletCount() == 0)
		{
			RefillAmmo();
		}
	}

	// 업데이트 순서 중요
	m_rStateTime += rDeltaTime;
	CheckTargetState(); // 각각의 스테이트에서 표적이 쓰러졌는지 확인.
	// 여기까지 // 업데이트 순서 중요
	
	if(m_bNeedReloadState == true)
	{
		if(m_rNeedReloadStateTime > 5.f)
		{
			// m_pHud->SetMessageInBoard(m_eTutorialState);
			FirstTickInTutorialState();
			m_bNeedReloadState = false;
		}
		m_rNeedReloadStateTime += rDeltaTime;
	}

	switch(m_eTutorialState)
	{
	case TS_START:
		{
			if(m_rStateTime > 6.f)
			{
				SetTutorialState(TS_MOVE);
				
			}
			if( pPlayer->isEnableStateMask(CHARA_STATE_DISABLE_MOVE) == false)
			{
				pPlayer->addEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH);
			}
		}
		break;
	case TS_MOVE:
		{
			if(m_rStateTime > 2.f)
			{
				m_pHudTutorial->OnOffTutorialHUD(TH_MISSION_START, false);
			}
		}
		break;

	case TS_TARGET_LOCKON:
	case TS_SHOOT_EXT:
		{
			if(pWeapon->isWeaponStateMask(WSM_ZOOM))
			{
				SetTutorialState(TS_SHOOT_EXT);
			}
			else
			{
				SetTutorialState(TS_TARGET_LOCKON);
			}
		}
		break;
	case TS_SWAP_GRENADE:
	case TS_THROW_GRENADE:
		{
			//if(pWeapon->getWeaponInfo()->GetTypeFire() == WEAPON::FIRE_THROWING)
			WeaponBase * pGrenade = pPlayer->getWeaponContext()->getWeapon(WEAPON_SLOT_THROWING1);

			if( pWeapon == pGrenade || pGrenade->getLoadedBulletCount() <= 0 )
			{
				SetTutorialState(TS_THROW_GRENADE);
			}
			else
			{
				SetTutorialState(TS_SWAP_GRENADE);
			}
		}
		break;
	case TS_PREPARE_GRENADE_LAUNCHER:
	case TS_SHOOT_GRENADE_LAUNCHER:
		{
			if(pWeapon->GetCurrentExtensionType() == WEAPON::EXT_SCOPE || pWeapon->getWeaponInfo()->GetTypeUsage() != WEAPON_SLOT_PRIMARY)
				SetTutorialState(TS_PREPARE_GRENADE_LAUNCHER);
			else
				SetTutorialState(TS_SHOOT_GRENADE_LAUNCHER);
		}
		break;
	case TS_SWAP_C4:
	case TS_INSTALL_C4:
		{
			if(pWeapon->getWeaponInfo()->isC4())
			{
				SetTutorialState(TS_INSTALL_C4);
			}
			else
			{
				SetTutorialState(TS_SWAP_C4);
			}
		}
		break;
	case TS_SWAP_MELEE:
	case TS_MELEE:
		{
			if(pWeapon->getWeaponInfo()->GetTypeUsage() == WEAPON_SLOT_MELEE)
			{
				SetTutorialState(TS_MELEE);
			}
			else
			{
				SetTutorialState(TS_SWAP_MELEE);
			}
		}
		break;
	case TS_SWAP_M187:
	case TS_SHOOT_M187:
		{
			if( pPlayer->IsOnObject())
				SetTutorialState(TS_SHOOT_M187);
			else
				SetTutorialState(TS_SWAP_M187);
		}
		break;
	case TS_RELEASE_M187:
		{
			if(! pPlayer->IsOnObject())
				SetTutorialState(TS_MOVE_TO_EXIT);
		}
		break;
	}

	i3InputDeviceManager	*	pMng	= g_pFramework->GetViewer()->GetInputDeviceManager();
	i3InputKeyboard			*	pKey	= pMng->GetKeyboard();

	if( pKey->GetStrokeState( I3I_KEY_TAB) && m_eTutorialState != TS_START)
	{
		if(m_bShowKeyDesc)
		{
			TurnOffKeyDescription();
			g_pSndMng->tellPlaySound(m_pTutorialSound[1]);
		}
		else
		{
			TurnOnKeyDescription(true);
			g_pSndMng->tellPlaySound(m_pTutorialSound[0]);
		}
		m_bShowKeyDesc = !m_bShowKeyDesc;
	}

}

void CTutorialMode::ReleaseTutorialSound()
{
	I3_SAFE_RELEASE( m_pTutorialSound[0]);
	I3_SAFE_RELEASE( m_pTutorialSound[1]);
}

void CTutorialMode::LoadTutorialSound()
{
	i3GameResSound * pRes = (i3GameResSound *) g_pFramework->QuaryResource( "UI_PopUp_Open.wav");
	I3ASSERT( pRes != nullptr);
	I3_REF_CHANGE( m_pTutorialSound[0], pRes->getSound());
	
	pRes = (i3GameResSound *) g_pFramework->QuaryResource( "UI_PopUp_Close.wav");
	I3ASSERT( pRes != nullptr);
	I3_REF_CHANGE( m_pTutorialSound[1], pRes->getSound());
}

/*
void CTutorialMode::ResetObjInTutorial()
{
	List	*	pObjectList = GetStageObjectList();

	if( pObjectList == nullptr )
		return;

	
	for(INT32 i = 0; i < pObjectList->GetCount(); i++ )
	{

		i3Object * pObj = (i3Object*) pObjectList->GetItem( i );
		INT32 iRet = i3String::Contain(pObj->GetName(), "BN_arrow_Green");
		if(iRet == 0)
		{
			pObj->setDynamicActiveState(true);
			pObj->VisibleScene();
		}
		if( i3::same_of<CTargetInTutorial*>(pObj))
		{
			//((CTargetInTutorial*)pObj)->ResetObjectOnTimeAttack();
		}
		if(pObj->getNetIndex() >= 0x1b && pObj->getNetIndex() <= 0x20)
		{
			pObj->setCurrentHP(pObj->getMaxHP());
			pObj->setCurState(0);
			pObj->setDynamicActiveState(true);
			pObj->VisibleScene();
		}
	}
}
*/

void CTutorialMode::TurnOffKeyDescription()
{
	m_pHudTutorial->OnOffTutorialHUD(TH_KEY_MOVE, false);
	m_pHudTutorial->OnOffTutorialHUD(TH_KEY_JUMP, false);
	m_pHudTutorial->OnOffTutorialHUD(TH_KEY_SHOOT, false);
	m_pHudTutorial->OnOffTutorialHUD(TH_KEY_EXT, false);
	m_pHudTutorial->OnOffTutorialHUD(TH_KEY_GRENADE_LAUNCHER, false);
	m_pHudTutorial->OnOffTutorialHUD(TH_KEY_GRENADE_SMOKE, false);
	m_pHudTutorial->OnOffTutorialHUD(TH_KEY_C4, false);
	m_pHudTutorial->OnOffTutorialHUD(TH_KEY_MELLE, false);
	m_pHudTutorial->OnOffTutorialHUD(TH_KEY_HELICOPTER, false);
}

void CTutorialMode::TurnOnKeyDescription(bool bForceTurnOn)
{
	TurnOffKeyDescription();

	if(m_bShowKeyDesc == false && bForceTurnOn == false)
	{
		return;
	}

	switch(m_eTutorialState)
	{
	case TS_MOVE:
	case TS_AVOID_HURDLE:
		{
			m_pHudTutorial->OnOffTutorialHUD(TH_KEY_MOVE, true);
		}
		break;
	case TS_JUMP:
		{
			m_pHudTutorial->OnOffTutorialHUD(TH_KEY_JUMP, true);
		}
		break;
	case TS_PREPARE_SHOOT:
	case TS_SHOOT:
		{
			m_pHudTutorial->OnOffTutorialHUD(TH_KEY_SHOOT, true);
		}
		break;
	case TS_PREPARE_SHOOT_EXT:
	case TS_TARGET_LOCKON:
	case TS_SHOOT_EXT:
		{
			m_pHudTutorial->OnOffTutorialHUD(TH_KEY_EXT, true);
		}
		break;
	case TS_PREPARE_GRENADE:
	case TS_SWAP_GRENADE:
	case TS_THROW_GRENADE:
	//case TS_THROW_SMOKE:
		{
			m_pHudTutorial->OnOffTutorialHUD(TH_KEY_GRENADE_SMOKE, true);
		}
		break;
	case TS_PREPARE_GRENADE_LAUNCHER:
	case TS_SHOOT_GRENADE_LAUNCHER:
		{
			m_pHudTutorial->OnOffTutorialHUD(TH_KEY_GRENADE_LAUNCHER, true);
		}
		break;
	
		
	case TS_PREPARE_C4:
	case TS_INSTALL_C4:
	case TS_REMOVE_C4:
		{
			//m_pHud->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_1"), 1800.f);
			m_pHudTutorial->OnOffTutorialHUD(TH_KEY_C4, true);
		}
		break;
	case TS_PREPARE_MELEE:
	case TS_SWAP_MELEE:
	case TS_MELEE:
		{
			//m_pHud->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_2"), 1800.f);
			m_pHudTutorial->OnOffTutorialHUD(TH_KEY_MELLE, true);
		}
		break;
	case TS_MOVE_TO_HELICOPTER:
	case TS_BOARD_HELICOPTER:
	case TS_SWAP_M187:
	case TS_SHOOT_M187:
	case TS_RELEASE_M187:
		{
			//m_pHud->SetMissionNoticeString( GAME_RCSTRING("STR_TBL_TUTORIAL_MISSION_DESC_SPECIAL_3"), 1800.f);
			m_pHudTutorial->OnOffTutorialHUD(TH_KEY_HELICOPTER, true);
		}
		break;
	}
}

INT32 CTutorialMode::GetMissionMaxCount(INT32 idx)
{
	switch(idx)
	{
	case 0:		return 3;
	case 1:		return 4;
	case 2:		return 3;
	case 3:		return 1;
	}
	return -1;
}

ChallengeBulletType CTutorialMode::IsMissionComplete(INT32 idx)
{
	if(GetMissionCompleteCount(idx) == GetMissionMaxCount(idx))
	{
		return CBT_COMPLATE;
	}
	else
	if (GetMissionCompleteCount(idx) == 0)
	{
		return CBT_START;
	}
	else
	{
		return CBT_CHALLENGING;
	}
}

INT32 CTutorialMode::GetMissionCompleteCount(INT32 idx)
{
	INT32 iReturn[4];

	if(idx>3 || idx<0)
	{
		I3ASSERT_0;
		return -1;
	}

	iReturn[0] = iReturn[1] = iReturn[2] = iReturn[3] = 0;

	if(m_iEndCountInTutorial == 0)
	{
		iReturn[0] = 0;
		iReturn[1] = 0;
		iReturn[2] = 0;
		iReturn[3] = 0;

		switch(m_eTutorialState)
		{
		case TS_AVOID_HURDLE:
			iReturn[0] = 1;
			break;
		case TS_JUMP:
			iReturn[0] = 2;
			break;
		case TS_PREPARE_SHOOT:
		case TS_SHOOT:
			iReturn[0] = 3;
			break;
		case TS_PREPARE_SHOOT_EXT:
		case TS_TARGET_LOCKON:
		case TS_SHOOT_EXT:
			iReturn[0] = 3;
			iReturn[1] = 1;
			break;
		case TS_PREPARE_GRENADE:
		case TS_SWAP_GRENADE:
		case TS_THROW_GRENADE:
			iReturn[0] = 3;
			iReturn[1] = 2;
			break;
		/*case TS_THROW_SMOKE:
			iReturn[0] = 3;
			iReturn[1] = 3;
			break;*/
		case TS_PREPARE_GRENADE_LAUNCHER:
		case TS_SHOOT_GRENADE_LAUNCHER:
			iReturn[0] = 3;
			iReturn[1] = 3;
			break;
		case TS_SWAP_C4:
		case TS_PREPARE_C4:
		case TS_INSTALL_C4:
		case TS_REMOVE_C4:
			iReturn[0] = 3;
			iReturn[1] = 4;
			break;
		case TS_PREPARE_MELEE:
		case TS_SWAP_MELEE:
		case TS_MELEE:
			iReturn[0] = 3;
			iReturn[1] = 4;
			iReturn[2] = 1;
			break;
		case TS_MOVE_TO_HELICOPTER:
		case TS_BOARD_HELICOPTER:
		case TS_SWAP_M187:
		case TS_SHOOT_M187: 
		case TS_RELEASE_M187:
			iReturn[0] = 3;
			iReturn[1] = 4;
			iReturn[2] = 2;
			break;
		case TS_MOVE_TO_EXIT:
		case TS_END:
			iReturn[0] = 3;
			iReturn[1] = 4;
			iReturn[2] = 3;
			break;
		}
	}
	else if(m_iEndCountInTutorial == 1)
	{
		iReturn[0] = 3;
		iReturn[1] = 4;
		iReturn[2] = 3;
		iReturn[3] = 0;
		if(m_eTutorialState == TS_END_TIME_ATTACK)
		{
			iReturn[3] = 1;
		}
	}
	
	
	return iReturn[idx];
}

void CTutorialMode::PushDrum(i3Object* pObj)
{
	m_vecDrumInTutorial.push_back(pObj);
}