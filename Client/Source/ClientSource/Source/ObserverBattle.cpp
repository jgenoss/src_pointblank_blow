#include "pch.h"
#include "ObserverBattle.h"

#include "UtilBattle.h"
#include "HackDefense.h"
#include "BattleHud.h"
#include "BattleGui.h"
#include "ui/UIHudManager.h"

#include "./StageBattle/UserContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "UserInfoContext.h"
#include "BattleServerContext.h"

#include "GameMaster.h"
#include "BattleHUD_Scope.h"
#include "GameStateMgr.h"
#include "GameMissionManager.h"
#include "UI/UIBattleFrame.h"
#include "UI/UIHudObserver.h"
#include "UI/UIHudDashBoard.h"
#include "UI/UIHudMissionBoard.h"
#include "UI/UIHUDIngameChat.h"


#define RESPAWN_COOL_TIME 3.0f
#define OBSERVER_CAMERA_INTERPOLATE_TIME 0.1f

namespace observer
{
	void ChangeObserverView(const INT32 idx)
	{
		UINT8 LockObserver = MyRoomInfoContext::i()->GetMyRoomLockObserver();

		//@eunil. 3인칭 시점 제한의 경우 3인칭 회전은 사용할 수 없다, 단 GM은 제한 없는 걸로...
		BOOL bObserverCamera1PVOnly = TESTBIT(LockObserver, LOCK_OBSERVER_1PVONLY);
		if(bObserverCamera1PVOnly && UserInfoContext::i()->IsGM_Observer() == false)
			CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_FPS, OBSERVER_CAMERA_INTERPOLATE_TIME);
		else
			CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_OBSERVER, OBSERVER_CAMERA_INTERPOLATE_TIME);

		observer::Context::i()->ChangeViewTarget(idx);	
	}

	bool ChangeViewToAnyoneAlive()
	{
		const INT32 idx = BattleSlotContext::i()->FindAnyoneAlive();
		if (idx != -1)
		{
			ChangeObserverView(idx);
			return true;
		}

		return false;
	}

	void ChangeViewTargetInTeam(const CHARACTER_TEAM_TYPE targetTeamType)
	{
		INT32 idx = observer::Context::i()->GetCurrIdx();
		CHARACTER_TEAM_TYPE currTeamType = CHARACTER_TEAM_NONE;

		if (idx == -1)
		{
			currTeamType = targetTeamType;
			idx = 0;
		}
		else
			currTeamType = BattleSlotContext::i()->getTeam(idx);

		if (targetTeamType != currTeamType)	// 타팀으로 변경.
			idx = BattleSlotContext::i()->FindAnyoneAlive(targetTeamType);
		else	// 현재 팀 내 변경.
			idx = BattleSlotContext::i()->FindNextPlayerAlive(targetTeamType, idx);

		if (idx != -1)
			observer::Context::i()->ChangeViewTarget(idx);
	}

	void ChangeViewToTopRanker(const TEAM_TYPE teamType)
	{
		INT32 idx = UserContext::i()->GetTopRankerIdx(teamType);
		if (idx != -1)
		{
			ChangeObserverView(idx);
			CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_FPS);

			if(g_pCamera->getCurTarget())
			{
				CHUD_Scope::instance()->SetEnableSinperScope(false);
				g_pCamera->getCurTarget()->Cmd_Observer_Zoom();
			}
		}
	}

	void ChangeViewToTopview()
	{
		util_battle::SetToOldCameraIfOnObjectCamera();

		if (MyRoomInfoContext::i()->getStage()->GetStageUID() == STAGE_UID_OUTPOST )
			g_pFramework->SetNightVisionEnable(false);

		ub::toggle_topview_camera(); 
	}

	void ToggleFlyingCamera()
	{
#if defined( DEF_OBSERVER_MODE)
		// GM만 가능
		if( BattleSlotContext::i()->isObserverGM())
			ub::toggle_flying_camera_mode(); 
#else
#if !defined( I3_DEBUG)
		if(UserInfoContext::i()->IsGM_Observer())
			ub::toggle_flying_camera_mode(); 
#endif
#endif
	}

	bool CheckTeamMembersAllDead()
	{
		if (UserInfoContext::i()->IsGM_Observer())	// GM옵저버의 경우 어느 한 쪽 팀이 전멸하면 관전화면을 킬러로 변경.
		{
			if (!BattleSlotContext::i()->CheckAnyoneAlive(CHARACTER_TEAM_RED) ||
				!BattleSlotContext::i()->CheckAnyoneAlive(CHARACTER_TEAM_BLUE))
				return true;
		}
		else
		{
			if (!BattleSlotContext::i()->CheckAnyoneAlive(BattleSlotContext::i()->getMyTeam()))	// 일반 유저의 경우 자신의 팀만 전멸시 관전화면을 킬러로 변경.
				return true;
		}

		return false;
	}

	void SetDotSight()
	{
		const INT32 currIdx = observer::Context::i()->GetCurrIdx();

		if (currIdx == -1) return;

		CGameCharaBase* pCharacter = g_pCharaManager->getCharaByNetIdx(currIdx);
		if (!pCharacter) return;

		WeaponBase* pWeapon = pCharacter->GetCurrentCharaWeapon();
		if(!pWeapon) return;

		if (pWeapon->GetCurrentExtensionType() == WEAPON::EXT_DOT_SIGHT ||
			pWeapon->GetCurrentExtensionType() == WEAPON::EXT_SCOPE )
		{
			if (pCharacter->getViewType() == CHARA_VIEW_1PV)
				CHUD_Scope::instance()->SetEnableSinperScope(true);
			else 
			if (pCharacter->getViewType() == CHARA_VIEW_3PV)
				CHUD_Scope::instance()->SetEnableSinperScope(false);
		}
	}

	void SelectViewTargetDirectly_GMCommand(const CHARACTER_TEAM_TYPE teamType, const INT32 keyNumber)
	{
		INT32 idx = keyNumber - 1;	// 키보드 넘버(1~8) => 슬롯 인덱스(0~7).

		if (teamType == CHARACTER_TEAM_RED)
			idx = idx * 2;
		else
			idx = (idx * 2) + 1;

		if (MyRoomInfoContext::i()->IsAutoTeamChanged())	// 공수교대가 실시되면 레드팀 블루팀 슬롯을 서로 변경한다.
			idx += (teamType == CHARACTER_TEAM_RED) ? 1 : -1;

		for (INT32 i=0; i<=idx; i++)	// GM 옵저버 스킵 처리.
		{
			if (BattleSlotContext::i()->getTeam(i) == teamType)
			{
				if (BattleSlotContext::i()->IsGMObserverForClient(i))
				{
					idx += 2;
					break;
				}
			}
		}

		//해당 팀에 GM이 있는 경우 인덱스에 +2가 되어 15를 넘어 크래시가 나는 것을 방지
		if (idx > 15)
			return;

		if (ub::is_live_character(idx))
			observer::Context::i()->ChangeViewTarget(idx);
	}

	bool IsPreparedToObserveEnemy()
	{
		UINT8 lock = MyRoomInfoContext::i()->GetMyRoomLockObserver();	// 설정창에서 체크된 상대방보기 유무에 따라.
		if (TESTBIT(lock, LOCK_OBSERVER_ENEMY))
			return true;

		// GM 일 경우 무조건 상대방 보기 허용, 나머지는 모드별로 허용.
		if (MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_ESCAPE) || MyRoomInfoContext::i()->getStage()->IsModeforModeType(STAGE_MODE_CROSSCOUNT) ||
#if defined( DEF_OBSERVER_MODE)
			BattleSlotContext::i()->isObserverMe())
#else
			UserInfoContext::i()->IsGM_Observer())
#endif
		{
			return true;
		}

		return false;
	}

	bool IsPreparedToObserveEnemyHP()
	{
		UINT8 lock = MyRoomInfoContext::i()->GetMyRoomLockObserver();	// 설정창에서 체크된 상대방보기 유무에 따라.
		if ( TESTBIT( lock, LOCK_OBSERVER_ENEMY_HP))
			return true;

#if defined( DEF_OBSERVER_MODE)
		if (BattleSlotContext::i()->isObserverMe())
#else
		if (UserInfoContext::i()->IsGM_Observer())
#endif
		{
			return true;
		}

		return false;
	}

	bool IsObserverState()
	{
		return (observer::Context::i()->GetCurrIdx() != -1) ? true : false;
	}

	bool UpdateObserverCamera(observer::Context* ctx, REAL32& tmAccumulated, const REAL32 tm)
	{
		if (i3::same_of<CGameCamera_FLY*>(g_pCamera))
		{
			if(CHud::i() && ctx->IsGMObserverHUDOff() == false)
				CHud::i()->SetHudMode(HUD_MODE_MISSION_OBSERVER);
				//CHud::i()->SetHudMode(HUD_MODE_MISSION_FLY);

			return false;	// 상태 전환 처리를 하지 않겠다.
		}

		g_pCamera->AdjustTargetByCamera();

		const INT32 currIdx = ctx->GetCurrIdx();
		if (currIdx == -1) return false;

		if (!ub::is_live_character(currIdx))
		{
			tmAccumulated += tm;

			if (tmAccumulated > RESPAWN_COOL_TIME)
			{
				INT32 idx = 0;
				if (UserInfoContext::i()->IsGM_Observer())	// 관전 중인 캐릭터의 팀 동료를 대상으로 변경한다.
				{	
					const CHARACTER_TEAM_TYPE currTeam = BattleSlotContext::i()->getTeam(currIdx);
					idx = BattleSlotContext::i()->FindNextPlayerAlive(currTeam, currIdx);
				}
				else
				{
					idx = BattleSlotContext::i()->FindNextPlayerAlive(BattleSlotContext::i()->getMyTeam(), currIdx);
				}

				if (idx == -1)
					idx = BattleSlotContext::i()->FindAnyoneAlive();

				if (idx != -1)
					ctx->ChangeViewTarget(idx);
			}
		}

		//PBBUG-17127 업데이트마다 왜 키는 걸까요
		if(CHud::i() && ctx->IsGMObserverHUDOff() == false)
			CHud::i()->SetHudMode(HUD_MODE_MISSION_OBSERVER);

		return true;
	}


	struct ViewState
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm) = 0;
	};

	struct ViewState_OnReady : ViewState				// 킬 카메라 또는 팀 전멸 상태로 전환. 
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm);
	} g_viewstate_ready;

	struct ViewState_OnTeamMembersAllDead : ViewState	// 팀 전멸 => 킬러 대상으로 타깃 변경.
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm);
	} g_viewstate_teamdead;

	struct ViewState_OnKillCamera : ViewState			// 킬 카메라 실행 중(로컬 또는 관전 캐릭터가 죽었을 때), 실행 종료 시 관전화면으로 전환.
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm);
	} g_viewstate_killcam;

	struct ViewState_OnObserverCamera : ViewState		// 관전 화면 실행 중 : 타겟 보여주기, 타겟 변경하기, 팀 전멸 처리.
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm);
		REAL32 m_tm;
	} g_viewstate_observercam;

	struct ViewState_OnRedTeamDeadAfterInstallingBomb : ViewState	// 레드팀이 폭탄 설치 후 전멸한 상태. 폭탄이 작동 중인 관계로 라운드는 종료 안 된 상태임.
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm);
		REAL32 m_tm;
	} g_viewstate_redteamdead_after_installbomb;

	struct ViewState_OnIntrude : ViewState	// 난입.
	{
		virtual void OnUpdate(observer::Context* owner, const REAL32 tm);
	} g_viewstate_intrude;



	void ViewState_OnReady::OnUpdate(observer::Context* owner, const REAL32 tm)
	{
		if(CHud::i())
		{
			if (CHud::i()->getHudMode() != HUD_MODE_DEFAULT && MyRoomInfoContext::i()->getStage()->IsHudOff() == false)
				CHud::i()->SetHudMode(HUD_MODE_DEFAULT);
		}
		if (CheckTeamMembersAllDead())
		{
			owner->SetViewState(&g_viewstate_teamdead);
			return;
		}

		if (i3::same_of<CGameCamera_Kill*>(g_pCamera))
		{
			CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_OBSERVER, OBSERVER_CAMERA_INTERPOLATE_TIME);
			owner->SetViewState(&g_viewstate_killcam);
			return;
		}

		if (BattleSlotContext::i()->IsLocalPlayerDead())
		{
			if (ChangeViewToAnyoneAlive())
			{
				g_viewstate_observercam.m_tm = 0.0f;
				owner->SetViewState(&g_viewstate_observercam);
			}
			return;
		}
	}

	void ViewState_OnTeamMembersAllDead::OnUpdate(observer::Context* owner, const REAL32 tm)
	{
		// 레드팀이 전멸하였으나 폭탄이 설치되었음. 폭탄이 터지거나 해체될 때까지 라운드 플레이 지속.
		const bool isRedTeamAllDead = 
			(BattleSlotContext::i()->FindAnyoneAlive(CHARACTER_TEAM_RED) == -1) ? true : false;

		const bool isBombInstalled = 
			(CGameMissionManager::i()->getMissionState() == GAME_MISSION_STATE_INSTALLED) ? true : false;

		//@eunil. 리스폰이 있으면! 다시 리셋시켜준다.즉, 폭미는 라운드 지속.
		CGameMissionManager * pMissionManager = CGameMissionManager::i();
		if(pMissionManager->getRespawnMode() != GAME_RESPAWN_MODE_1LIFE)
		{
			const bool isBlueTeamAllDead =
				(BattleSlotContext::i()->FindAnyoneAlive(CHARACTER_TEAM_BLUE) == -1) ? true : false;

			if (!isRedTeamAllDead && !isBlueTeamAllDead) {
				owner->SetViewState(&g_viewstate_ready);
				return;
			}
		}

		if (isRedTeamAllDead && isBombInstalled)
		{
			g_viewstate_redteamdead_after_installbomb.m_tm = 0.0f;
			owner->SetViewState(&g_viewstate_redteamdead_after_installbomb);
			return;
		}

		INT32 idx = BattleSlotContext::i()->GetKillerIdxMostRecent();

		if (idx == -1 || !ub::is_live_character(idx))	// 자살에 대한 처리는 유효하지 않으므로 아무 플레이어를 타겟으로.
			idx = BattleSlotContext::i()->FindAnyoneAlive();

		if (idx != -1)
		{
			if(CHud::i())
				CHud::i()->ResetKillMessage();

			if (IsObserverState())
				owner->ChangeViewTarget(idx);
			else
				ChangeObserverView(idx);
		}
	}

	void ViewState_OnKillCamera::OnUpdate(observer::Context* owner, const REAL32 tm)
	{
		if (CheckTeamMembersAllDead())	// 킬카메라 도중에 팀원 전원 사망 => 상대 플레이어를 타겟으로 변경.
		{
			owner->SetViewState(&g_viewstate_teamdead);
			return;
		}

		if (!i3::same_of<CGameCamera_Kill*>(g_pCamera))	// 킬카메라가 아니므로 타겟 변경 / 상태 전환.
		{
			if (ChangeViewToAnyoneAlive())
			{
				UINT8 LockObserver = MyRoomInfoContext::i()->GetMyRoomLockObserver();
				//@eunil. 3인칭 시점 제한의 경우 3인칭 회전은 사용할 수 없다, 단 GM은 제한 없는 걸로...
				BOOL bObserverCamera1PVOnly = TESTBIT(LockObserver, LOCK_OBSERVER_1PVONLY);
				if (bObserverCamera1PVOnly && UserInfoContext::i()->IsGM_Observer() == false)
					CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_FPS, OBSERVER_CAMERA_INTERPOLATE_TIME);

				g_viewstate_observercam.m_tm = 0.0f;
				owner->SetViewState(&g_viewstate_observercam);
			}
			return;
		}

		if (((CGameCamera_Kill*)g_pCamera)->isFreeze()) // 관전화면으로 전환. 킬카메라 종료 시점에서.
		{
			if (!IsObserverState())
			{
				if (ChangeViewToAnyoneAlive())
				{
					UINT8 LockObserver = MyRoomInfoContext::i()->GetMyRoomLockObserver();
					//@eunil. 3인칭 시점 제한의 경우 3인칭 회전은 사용할 수 없다, 단 GM은 제한 없는 걸로...
					BOOL bObserverCamera1PVOnly = TESTBIT(LockObserver, LOCK_OBSERVER_1PVONLY);
					if (bObserverCamera1PVOnly && UserInfoContext::i()->IsGM_Observer() == false)
						CGameCameraManager::i()->ChangeCamera(CAMERA_TYPE_FPS, OBSERVER_CAMERA_INTERPOLATE_TIME);

					g_viewstate_observercam.m_tm = 0.0f;
					owner->SetViewState(&g_viewstate_observercam);
				}
				return;
			}
		}
	}

	void ViewState_OnObserverCamera::OnUpdate(observer::Context* owner, const REAL32 tm)
	{
		if (UpdateObserverCamera(owner, m_tm, tm))
		{
			if (CheckTeamMembersAllDead())
				owner->SetViewState(&g_viewstate_teamdead);
		}
	}

	void ViewState_OnRedTeamDeadAfterInstallingBomb::OnUpdate(observer::Context* owner, const REAL32 tm)
	{
		const INT32 idx = BattleSlotContext::i()->FindAnyoneAlive(CHARACTER_TEAM_BLUE);
		if (idx != -1)
		{
			if (!IsObserverState())
				ChangeObserverView(idx);	

			UpdateObserverCamera(owner, m_tm, tm);
		}
	}

	void ViewState_OnIntrude::OnUpdate(observer::Context* owner, const REAL32 tm)
	{
		INT32 idx = BattleSlotContext::i()->FindAnyoneAlive(BattleSlotContext::i()->getMyTeam());

		I3_BOUNDCHK_RETURN( idx, SLOT_MAX_COUNT);

		if ( idx != -1 && BattleSlotContext::i()->IsGMObserverForClient(idx))	// GM옵저버는 플레이 시작 후 바로 죽기 때문에 선택되어서는 안 된다.
			idx = BattleSlotContext::i()->FindNextPlayerAlive(BattleSlotContext::i()->getMyTeam(), idx);

		if (idx == -1)
			idx = BattleSlotContext::i()->FindAnyoneAlive(BattleSlotContext::i()->getEnemyTeamType());

		if (idx != -1)
		{
			ChangeObserverView(idx);

			g_viewstate_observercam.m_tm = 0.0f;
			owner->SetViewState(&g_viewstate_observercam);
		}
	}

	void SetIntrudeViewState()
	{
		observer::Context::i()->SetViewState(&g_viewstate_intrude);
	}



	Context::Context()
	{
		Reset();
	}

	void Context::Reset()
	{
		m_currIdx = -1;
		m_currViewState = &g_viewstate_ready;
		m_bGMHUDOff = false;
	}

	bool Context::ChangeViewTarget(const INT32 idx)
	{
		hd::check_observer::i()->check();	// 해킹 방어 처리. SPION 핵이 ROOM_INFO_ADD::_LockObserver를 해킹하는 문제로 확인한다.

		if (idx < 0 || SLOT_MAX_COUNT <= idx) return false;
		if (!ub::is_live_character(idx)) return false;

		CGameCharaBase* pCharacter = g_pCharaManager->getCharaByNetIdx(idx);

		if (pCharacter == nullptr) return false;
		if (g_pCamera->getCurTarget() == pCharacter) return false;

		if (g_pCamera->getCurTarget())	// 이전 캐릭터를 3인칭뷰로 처리
			g_pCamera->getCurTarget()->Cmd_Observer(false);

		CGameCameraManager::i()->setTargetCharacter(pCharacter);	// 바뀌는 캐릭터를 카메라에 등록
		pCharacter->Cmd_Observer(true);

		m_currIdx = idx;

		CHud::i()->SetEnableObserver(true);
		observer_hud::Context::i()->Notify(observer_hud::EVT_TARGET_CHANGED, idx);

		for (INT32 i=0; i<g_pCharaManager->getCharaCount(); i++)	// TeamTag비활성화
		{
			CGameCharaBase * pChara = g_pCharaManager->getChara(i);
			if (pChara == nullptr)	continue;
			pChara->Cmd_Enable_TeamTag( false);

			if (pChara->same_of(Avatar::static_meta()))	// 팀 밴드 설정
#if defined (LOCALE_RUSSIA)		
				((Avatar*)pChara)->DisableTeamBand();	// PBRU-1064 러시아 요청사항 팀밴드 삭제
#else
				((Avatar*)pChara)->SetTeamBand(true);	// 관전 모드는 팀색상을 사용해야합니다.	
#endif
		}

		return true;
	}

	void Context::SetViewState(ViewState* state)
	{
		m_currViewState = state;
	}

	void Context::OnUpdate(REAL32 tm)
	{
		m_currViewState->OnUpdate(this, tm);

		if (IsObserverState())
		{
			_UpdateInput();
		}
	}

	INT32 Context::GetCurrIdx() const
	{
		return m_currIdx;
	}

	void Context::_UpdateInput()
	{
		if(CHud::i() == nullptr) return;

		i3InputDeviceManager* pInputMgr = g_pViewer->GetInputDeviceManager();
		i3InputKeyboard* pKey = pInputMgr->GetKeyboard();
		i3InputMouse* pMouse = pInputMgr->GetMouse();

		// revision 61410 [IngameUI] ESC 로 게임 메뉴 활성화 시 관전모드의 인풋 막음
		if (CBattleGui::i()->GetEnableGui(GAME_GUI_RESPAWN)) return;
		if (CHud::i()->GetEnableChatScroll()) return;
		if (CBattleGui::i()->GetEnablePopup()) return;

		if (!CHud::i()->GetChatEnable())
		{
			if (UserInfoContext::i()->IsGM_Observer())
				_UpdateInput_GMObserver(pKey, pMouse);

			if (pKey->GetStrokeState(I3I_KEY_SPACE))
				ub::toggle_normal_observe_mode();
			else 
			if (pKey->GetStrokeState(I3I_KEY_P))
				ToggleFlyingCamera();
		}

		if (!i3::same_of<CGameCamera_FLY*>(g_pCamera) && !CGameMaster::i()->IsPaused())
			_UpdateInput_ViewChange(pKey, pMouse);
	}

	void Context::_UpdateInput_GMObserver(i3InputKeyboard* pKey, i3InputMouse* pMouse)
	{
		CHud* pHUD = CHud::i();

		if (pKey->GetPressState(I3I_KEY_LSHIFT) || pKey->GetPressState(I3I_KEY_RSHIFT))
		{
			if (pKey->GetStrokeState(I3I_KEY_1))		
				ChangeViewToTopRanker(TEAM_RED);
			else 
			if (pKey->GetStrokeState(I3I_KEY_2))	
				ChangeViewToTopRanker(TEAM_BLUE);
			else 
			if (pKey->GetStrokeState(I3I_KEY_3))	
				ub::toggle_flying_camera_mode();
			else 
			if (pKey->GetStrokeState(I3I_KEY_4))
				ChangeViewToTopview();
		}
		else if (g_pFramework->getKeyboard()->GetPressState(I3I_KEY_LCTRL) || g_pFramework->getKeyboard()->GetPressState(I3I_KEY_RCTRL))
		{
			if (i3::same_of<CGameCamera_FLY*>(g_pCamera)) // 저장 기능. 플라잉 카메라일 때만 저장 가능.
			{
				for (INT32 i=0; i<=9; i++) // 0 ~ 9 number key
				{
					if (pKey->GetStrokeState(static_cast<I3I_KEY>(i)))
						FlyCameraUtil::SaveFlyCameraLocation(static_cast<I3I_KEY>(i));
				}
			}

			if (pKey->GetPressState(I3I_KEY_LALT))
			{
				if (pKey->GetStrokeState(I3I_KEY_P))
				{
					if (CGameMaster::i()->IsPaused())	//GM Observer Pause 기능
						GameEventSender::i()->SetEventOnce(EVENT_BATTLE_GM_RESUME);
					else
						GameEventSender::i()->SetEventOnce(EVENT_BATTLE_GM_PAUSE);
				}
			}
		}
		else if (pKey->GetStrokeState(I3I_KEY_NUMPADPLUS) || pKey->GetStrokeState(I3I_KEY_NUMPADMINUS))
		{
			if(pKey->GetStrokeState(I3I_KEY_NUMPADPLUS))
				pHUD->ChangeTextNameSize(true);
			else 
			if(pKey->GetStrokeState(I3I_KEY_NUMPADMINUS))
				pHUD->ChangeTextNameSize(false);
			else
				I3ASSERT_0;
		}
		else
		{
			if (pKey->GetStrokeState(I3I_KEY_F5))	// 인게임 HUD ON/OFF(디버그모드에서 채팅창 "/test 21"도 가능)
			{
				if (IsGMObserverHUDOff() == false) //꺼짐
				{
					SetGMObserverHUDOff(true);
					pHUD->SetHudMode(HUD_MODE_NOTHING);

					//스코어 보드는 항상 On
					UIHudDashBoard* pBoard = static_cast<UIHudDashBoard*>(HUD::instance()->GetHud(HUD::DASH_BOARD));
					if(pBoard) pBoard->SetVisible(true);

					UIHUDIngameChat* chat = (UIHUDIngameChat*)UIBattleFrame::i()->GetHUD(UIHUD_INGAMECHATING);
					if (chat) chat->SetVisible(false);
				}
				else
				{ // 켜짐
					SetGMObserverHUDOff(false);

					pHUD->SetHudMode(HUD_MODE_MISSION_OBSERVER);

					//스코어 보드는 항상 On
					UIHudDashBoard* pBoard = static_cast<UIHudDashBoard*>(HUD::instance()->GetHud(HUD::DASH_BOARD));
					pBoard->SetVisible(true);

				}
			}
			else 
				if (pKey->GetStrokeState(I3I_KEY_GRAVE))	// 닉네임 ON/OFF
					UserInfoContext::i()->ToggleViewHudNick();
				else 
				if (pKey->GetStrokeState(I3I_KEY_EQUAL))	// 닉네임 사이즈 변경 : I3I_KEY_NUMPADPLUS -> WHEEL UP, I3I_KEY_NUMPADMINUS -> WHEEL DOWN
					pHUD->ChangeTextNameSize(true);
				else 
				if (pKey->GetStrokeState(I3I_KEY_MINUS))
					pHUD->ChangeTextNameSize(false);

			for (INT32 i=0; i<=9; i++) // 0 ~ 9 number key, 카메라 위치 이동
			{
				if (pKey->GetStrokeState(static_cast<I3I_KEY>(i)))
					FlyCameraUtil::MoveFlyCameraLocation(static_cast<I3I_KEY>(i));
			}

			if (pKey->GetPressState(I3I_KEY_LBRACKET))	// 유저 수동 선택(슬롯 선택) 기능. RED TEAM [
			{
				for (INT32 i=1; i<=MAX_TEAM_SLOT_COUNT; i++)	// 1 ~ 8 number Key
				{
					if (pKey->GetStrokeState(static_cast<I3I_KEY>(i)))
						SelectViewTargetDirectly_GMCommand(CHARACTER_TEAM_RED, i);
				}
			}
			else if (pKey->GetPressState(I3I_KEY_RBRACKET))	// BLUE TEAM ] 
			{
				for (INT32 i=1; i<=MAX_TEAM_SLOT_COUNT; i++)	// 1 ~ 8 number Key
				{
					if (pKey->GetStrokeState(static_cast<I3I_KEY>(i)))
						SelectViewTargetDirectly_GMCommand(CHARACTER_TEAM_BLUE, i);
				}
			}
		}
	}

	void Context::_UpdateInput_ViewChange(i3InputKeyboard* pKey, i3InputMouse* pMouse)
	{
		if (pMouse->GetStrokeButtonState() & I3I_MOUSE_LBUTTON)	// 레드팀 타켓 변경.
		{
			if (IsPreparedToObserveEnemy())
			{
				ChangeViewTargetInTeam(CHARACTER_TEAM_RED);
			}
			else
			{
				if (BattleSlotContext::i()->getMyTeam() == CHARACTER_TEAM_RED)
					ChangeViewTargetInTeam(CHARACTER_TEAM_RED);
			}
		}
		else if (pMouse->GetStrokeButtonState() & I3I_MOUSE_RBUTTON)	// 블루팀 타겟 변경.
		{
			if (IsPreparedToObserveEnemy())
			{
				ChangeViewTargetInTeam(CHARACTER_TEAM_BLUE);
			}
			else
			{
				if (BattleSlotContext::i()->getMyTeam() == CHARACTER_TEAM_BLUE)
					ChangeViewTargetInTeam(CHARACTER_TEAM_BLUE);
			}
		}
	}
}