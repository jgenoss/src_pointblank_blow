#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaAnimControl.h"
#include "GameCharaBase.h"
#include "GameCharaContexts.h"
#include "Character/GameDinoAnimContext.h"
#include "Weapon.h"
#include "StageBattle.h"
#include "GameCharaDino.h"
#include "i3Base/string/ext/snprintf.h"
#include "i3Base/string/ext/generic_string_cat.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "HackContext.h"
#include "GameMissionManager.h"
#include "GameCharaMultiWeaponContext.h"

#include "MainWeapon_DualCIC.h"

#include "i3Base/string/ext/utf16_to_mb.h"

#include "ClientStageInfoUtil.h"


I3_CLASS_INSTANCE( CGameCharaAnimControl);

void CGameCharaAnimControl::_ProcessJumpEnd()
{
	VEC3D * pVecDir = getMoveContext()->getMoveDir();

	CHARA_ACTION_LOWER lower_state = CHARA_LOWER_RUN;
		
	//	착지후 다시 한번 방향키 입력을 체크합니다. (그냥 착지 할지 바로 이동할지)
	UINT64 keyPress = getInputContext()->getKeyPressInput();

	if( keyPress & GAME_KEY_MOVE_FORWARD )				setZ( pVecDir, 1.f);
	else if( keyPress & GAME_KEY_MOVE_BACKWARD )		setZ( pVecDir, -1.f);
	else												setZ( pVecDir, 0.f);

	if( keyPress & GAME_KEY_MOVE_LEFT )					setX( pVecDir, 1.f);
	else if( keyPress & GAME_KEY_MOVE_RIGHT)			setX( pVecDir, -1.f);
	else												setX( pVecDir, 0.f);

	if( (getX( pVecDir) == 0.0f) && (getY( pVecDir) == 0.0f) && (getZ( pVecDir) == 0.0f))
		lower_state = CHARA_LOWER_IDLE;

	if(m_pOwner->IsDino() == false)
	{
		WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
		if( pWeapon != nullptr )
		{
			WEAPON::eSTATE state = pWeapon->getWeaponState();

			if( state == WEAPON::STATE_READY || state == WEAPON::STATE_WAIT )
				pWeapon->PlayAnimation_JumpEnd();
		}
	}
	
	bool bIsHackUser = HackContext::i()->isHackUserFrezen( BattleSlotContext::i()->getMySlotIdx());

	//	제자리 점프다. -> 점프 끝 애니메이션으로
	//렙터스팅
	if( lower_state == CHARA_LOWER_IDLE || m_pOwner->IsRaptorBase() || bIsHackUser)			
	{
		m_pOwner->Cmd_JumpEnd();
	}
	else
	{	//	이동중 점프다. -> 이동 애니메이션으로
		if( keyPress & GAME_KEY_ACTION_FOOTSTEP)
			m_pOwner->Cmd_Walk( pVecDir);	//	걷기
		else
			m_pOwner->Cmd_Run( pVecDir);	//	뛰기
	}

	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_MOVE | CHARA_STATE_DISABLE_CROUCH );

	//점프 종료후 앉기 버튼이 눌려져 있으면 앉아버린돠.
	if( keyPress & GAME_KEY_ACTION_CROUCH)
	{
		getAnimContext()->setOldLowerID( ID_LOWER_JUMP_END);
		m_pOwner->Cmd_Crouch();
	}

	//	점프 딜레이 시간 설정
	//m_fJumpDelayElapsedTime = getDBInfo()->m_rJumpDelay;
	m_pOwner->setJumpDelayElapsedTime( 0.f);

	getAnimContext()->setEnableRunJump( false);
}

REAL32 CGameCharaAnimControl::_PlayFootStepSound( REAL32 rDuration, REAL32 rElapseTime, REAL32 tm)
{
	if( rDuration > 0.0f)
	{
		if( rElapseTime >= rDuration || rElapseTime <= tm)
		{	
			if( rElapseTime >= rDuration)
				rElapseTime = 0.f;

			m_pOwner->removeCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT);
			INT32 nAccessory = ( getAnimContext()->getAccessorySoundIdx() > 0)	? 0 : 1;
			getAnimContext()->setAccessorySoundIdx( nAccessory);

			if( m_pOwner->IsDino() == false)
			{
				if( !getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))
				{
					if( getActionContext()->isLower( CHARA_LOWER_WALK))
					{
						g_pSndMng->StartCharacterActionSnd( GTBCA_ON_WALK_EQUIPMENT, nAccessory, m_pOwner->GetPos(), false);
					}
					else if( getActionContext()->isLower( CHARA_LOWER_RUN))
					{
						g_pSndMng->StartCharacterActionSnd( GTBCA_ON_RUN_EQUIPMENT, nAccessory, m_pOwner->GetPos(), false);
					}
				}
			}
		}
		else if( rElapseTime >= getAnimContext()->getLeftStepTime())
		{	//	왼발 소리 플레이	
			if( m_pOwner->isCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT))
			{
				if( !getActionContext()->isBody( CHARA_BODYLOWER_CROUCH) &&
					!getActionContext()->isLower( CHARA_LOWER_WALK))
				{
					getSoundContext()->PlayFootStep( getMoveContext()->getFloorTerrainType(), true, false);
				}

				//g_pEffectManager->AddEnvironmentEffect( ENV_EFEECT_TREX_WALK, m_pOwner->GetPos(), &I3_YAXIS);

				m_pOwner->removeCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT);
			}				
		}
		else if( rElapseTime >= getAnimContext()->getRightStepTime())
		{	//	오른발 소리 플레이
			if( !m_pOwner->isCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT))
			{
				if( !getActionContext()->isBody( CHARA_BODYLOWER_CROUCH) &&
					!getActionContext()->isLower( CHARA_LOWER_WALK))
				{
					getSoundContext()->PlayFootStep( getMoveContext()->getFloorTerrainType(), false, false);
				}
				m_pOwner->addCharaStateMask( CHARA_STATE_FOOTSTEPRIGHT);

				//g_pEffectManager->AddEnvironmentEffect( ENV_EFEECT_TREX_WALK, m_pOwner->GetPos(), &I3_YAXIS);
			}
		}
	}

	return rElapseTime;
}

bool CGameCharaAnimControl::PlayLower( ID_LOWER_AI id, REAL32 timeStart, REAL32 timeScale)
{
	I3ASSERT( m_pOwner->IsDino() == false);

	bool bRv = false;
	i3AIContext * pAILower = getAnimContext()->getAILower();
	
	// 처음에는 초기화 상태로 UNKNOWN상태가 들어올 수 있습니다.
	// 기본적으로 IDLE의 동작을 해줍니다.
	if( (id <= ID_LOWER_UNKNOWN) || (id >= ID_LOWER_AI_MAX) )
		id = ID_LOWER_IDLE;

	//	현재 동작이 무엇인지 저장한다.
	getAnimContext()->setOldLowerID( id);
		
	if( (pAILower != nullptr) )
	{
		INT32 nAIIdx = 0;

		if(m_pOwner->IsDino())
			nAIIdx = ((CGameDinoAnimContext*)getAnimContext())->getDinoAILowerID( (ID_LOWER_DINO_AI)id);
		else
			nAIIdx = getAnimContext()->getAILowerID( getActionContext()->getBodyLower(), id);

		I3ASSERT( nAIIdx >= 0);

		//GlobalFunc::PB_TRACE("nAIIdx %d", nAIIdx);
		pAILower->setCurrentAIState( nAIIdx, timeStart, timeScale);		//	하체 AI 상태 변경

		bRv = true;

#if defined( ENABLE_CHARA_ANIM_OUTPUT )
		{
			TRACE_FUNC;
			AnimDebug( pAILower, timeStart );
		}
#endif
	}

	return bRv;
}

bool CGameCharaAnimControl::PlayUpper( ID_UPPER_AI id, REAL32 timeStart, REAL32 timeScale)
{
	if( (id <= ID_UPPER_UNKNOWN) || (id >= ID_UPPER_AI_MAX))
	{
		I3PRINTLOG(I3LOG_NOTICE, "잘못된 ID입니다.");
		return false;			// 초기화 안된 애니메이션이 들어오는 경우가 있습니다. 2008. 8.29
	}

	//GlobalFunc::PB_TRACE("PlayUpper id %d", id);

	if( getActionContext()->isLower( CHARA_LOWER_DEATH))	return false;

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	if( pAIUpper == nullptr) return false;

	i3AIContext * pAIUpper_Left = getAnimContext()->getAIUpper_Left();

	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();

	//	무기 교환은 예외로 현재는 아직 바뀌지 않은 상태이며 예약된 상태이다.
	//	다음 프레임에 무기 노드가 바뀌도록 되어 있으므로 캐릭터는 예약된 무기로 인식해야 한다.	komet
	WEAPON_SLOT_TYPE nSwapWeaponSlotNum = getWeaponContext()->getSwapSlotNum();
	WEAPON_SLOT_TYPE nCurWeaponSlotNum = getWeaponContext()->getCurrentSlotNum();
	if( nSwapWeaponSlotNum != nCurWeaponSlotNum )//&& ID_UPPER_SWAP_WEAPON == id)
		pWeapon = getWeaponContext()->getWeapon( nSwapWeaponSlotNum);

	if (getWeaponContext()->getSwapSlotNum() == WEAPON_SLOT_PRIMARY && MyRoomInfoContext::i()->getStage()->IsMultiWeapon())
	{
		pWeapon = m_pOwner->getMultiWeaponContext()->GetSwapMultiWeapon(false);
	}
	
	if( pWeapon == nullptr )	return false;

	pWeapon = pWeapon->getUsedWeapon();
	I3ASSERT( pWeapon != nullptr);
	//	WeaponInfo는 swap등으로 전환될수 있으므로 메인 무기의 애니메이션으로 사용
	CWeaponInfo * pWeaponInfo = pWeapon->getAbsoluteWeaponInfo();	

	// KEYWORD : WEAPON_C5
	if( i3::same_of<WeaponBombTrigger*>(pWeapon))
	{
		WeaponBase * pLinkWeapon = ((WeaponBombTrigger*)pWeapon)->getBomb();
		if( pLinkWeapon != nullptr)
			pWeaponInfo = pLinkWeapon->getWeaponInfo();
	}

	I3ASSERT( pWeaponInfo != nullptr );

#if defined( AI_MOVE_TO_WEAPON)
	INT32 nAIIdx = pWeapon->getCharaAIIndex( m_pOwner->getAnimSet(), pActionCtx->getBodyUpper(), id);
#else
	INT32 nAIIdx = getAnimContext()->getAIUpperID( pWeaponInfo->GetTypeClass(), pWeaponInfo->GetNumber(), id);
#endif

	if( nAIIdx < 0 || nAIIdx >= pAIUpper->getAIStateCount() )
	{
//#if defined( I3_DEBUG) || defined( BUILD_RELEASE_QA_VERSION)
//		//m_pOwner->DumpChara();
//		I3PRINTLOG(I3LOG_WARN,  "Chara Animset no animation. ANIMSET(%d) W_CLASS(%d) W_NUMBER(%d), BODY2(%d), ID(%d)",
//			m_pOwner->getViewType(), pWeaponInfo->GetTypeClass(), pWeaponInfo->GetNumber(), getActionContext()->getBodyUpper(), id);
//#endif
		return false;
	}

	//	(현재 동작이 무엇인지 저장한다.)
	getAnimContext()->setOldUpperID( id);
	getAnimContext()->setOldUpperDualLeftID( ID_UPPER_DUAL_UNKNOWN);
	getAnimContext()->setOldUpperDualRightID( ID_UPPER_DUAL_UNKNOWN);

#if defined( AI_MOVE_TO_WEAPON)
	if( id == ID_UPPER_SWAP_WEAPON)
	{
		// 무기 변경 순간, 1 Frame 오차로 인해 잠깐 무기가 잘못 나타나는 현상을
		// 방지하기 위해, Delay가 발생하지 않는 setCurrentAIStateDirect() 함수로
		// 변경.
		pAIUpper->setCurrentAIStateDirect( nAIIdx, timeStart, timeScale);		//	상체 AI 상태 변경

		if( (pAIUpper_Left != nullptr) && g_pFramework->IsBattleStage() )	// UI 애니메이션은 하지 않도록 합니다.
		{
			pAIUpper_Left->setCurrentAIStateDirect( nAIIdx, timeStart, timeScale);
		}
	}
	else
#endif
	{
		pAIUpper->setCurrentAIState( nAIIdx, timeStart, timeScale);		//	상체 AI 상태 변경

		if( (pAIUpper_Left != nullptr) && g_pFramework->IsBattleStage() )	// UI 애니메이션은 하지 않도록 합니다.
		{
			pAIUpper_Left->setCurrentAIState( nAIIdx, timeStart, timeScale);
		}
	}

#if defined( ENABLE_CHARA_ANIM_OUTPUT )
	TRACE_FUNC;
	AnimDebug( pAIUpper, timeStart);
#endif

	return true;
}

bool CGameCharaAnimControl::PlayDinoUpper( ID_UPPER_DINO_AI id, REAL32 timeStart, REAL32 timeScale)
{
#ifdef ENABLE_CHARA_ANIM_OUTPUT
	GlobalFunc::PB_TRACE("PlayDinoUpper id %d", id);
#endif

	if( (id <= ID_UPPER_DINO_UNKNOWN) &&  (id >= ID_UPPER_DINO_AI_MAX))
	{
		I3PRINTLOG(I3LOG_NOTICE, "잘못된 ID입니다.");
		return false;			// 초기화 안된 애니메이션이 들어오는 경우가 있습니다. 2008. 8.29
	}

	if( m_pOwner->IsRaptorBase())
	{
		if( id == ID_UPPER_DINO_DAMAGE_MOVE_LEFT)		{	id = ID_UPPER_DINO_DAMAGE_LEFT;}
		else if( id == ID_UPPER_DINO_DAMAGE_MOVE_RIGHT) {	id = ID_UPPER_DINO_DAMAGE_RIGHT;}
	}

	//GlobalFunc::PB_TRACE("ID_UPPER_DINO_AI %d \n", id);

	if( getActionContext()->isLower( CHARA_LOWER_DEATH))	return false;

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	if( pAIUpper == nullptr) return false;
	i3AIContext * pAIUpper_Left = getAnimContext()->getAIUpper_Left();


	INT32 nAIIdx = ((CGameDinoAnimContext*)getAnimContext())->getDinoAIUpperID( id);
	if( nAIIdx < 0 || nAIIdx >= pAIUpper->getAIStateCount() )
	{
		I3PRINTLOG(I3LOG_WARN,  "Dino Animset no animation. ANIMSET(%d) ID(%d)",	m_pOwner->getViewType(), id);
		return false;
	}

	//	(현재 동작이 무엇인지 저장한다.)
	((CGameDinoAnimContext*)getAnimContext())->setOldDinoUpperID( id);
	getAnimContext()->setOldUpperDualLeftID( ID_UPPER_DUAL_UNKNOWN);
	getAnimContext()->setOldUpperDualRightID( ID_UPPER_DUAL_UNKNOWN);
	pAIUpper->setCurrentAIState( nAIIdx, timeStart, timeScale);		//	상체 AI 상태 변경

	if( pAIUpper_Left != nullptr)
		pAIUpper_Left->setCurrentAIState( nAIIdx, timeStart, timeScale);

#if defined( ENABLE_CHARA_ANIM_OUTPUT )
	TRACE_FUNC;
	AnimDebug( pAIUpper, timeStart);
#endif

	return true;
}

/*virtual*/bool CGameCharaAnimControl::PlayUpperDual( ID_UPPER_DUAL_AI id, REAL32 timeStart, REAL32 timeScale)
{	
	INT32 nAIIdx;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return false;
	if( id <= ID_UPPER_DUAL_UNKNOWN || id >= ID_UPPER_DUAL_AI_MAX) return false;
	
	WeaponBase * pWeapon = getUsedWeapon();
	if( pWeapon == nullptr )	return false;
	
	CWeaponInfo * pWeaponInfo = pWeapon->getWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr );

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	if( pAIUpper == nullptr)	return false;
	i3AIContext * pAIUpperLeft = getAnimContext()->getAIUpper_Left();
	if( pAIUpperLeft == nullptr)	return false;
	
#if defined( AI_MOVE_TO_WEAPON)
	// KEYWORD : AI_MOVE_TO_WEAPON
	nAIIdx = pWeapon->getCharaDualAIIndex( m_pOwner->getAnimSet(), id);
#else
	nAIIdx = getAnimContext()->getAIUpperDualID( pWeaponInfo->GetNumber(), id);
#endif

	if( nAIIdx == -1)
	{
		I3PRINTLOG(I3LOG_WARN,  "Chara Animset no animation. ANIMSET(%d) W_CLASS(WEAPON_CLASS_DUALHANDGUN) W_NUMBER(%d), ID(%d)",
			m_pOwner->getViewType(),	pWeaponInfo->GetNumber(), id);
		return false;
	}

	if( id < ID_UPPER_DUAL_IDLE_RIGHT)
	{
		//	(현재 동작이 무엇인지 저장한다.)
		getAnimContext()->setOldUpperDualLeftID( id);
		pAIUpperLeft->setCurrentAIState( nAIIdx, timeStart, timeScale);
	}
	else
	{
		//	(현재 동작이 무엇인지 저장한다.)
		getAnimContext()->setOldUpperDualRightID( id);
		pAIUpper->setCurrentAIState( nAIIdx, timeStart, timeScale);
	}

	getAnimContext()->setOldUpperID( ID_UPPER_UNKNOWN);

	return true;
}

void CGameCharaAnimControl::PlayStop( void)
{
	m_pOwner->getSceneObject()->GetBody()->StopAnim();
}

void CGameCharaAnimControl::AnimDebug( i3AIContext * pCtx, REAL32 timeStart)
{//	i3AIContext가 nullptr일 경우도 있다. 스킵하도록
	if( pCtx == nullptr)		return;

	i3AIState * pAIState = nullptr;
	char * pszAIName = "(null)";

	//	아직 Anim이 현 프레임에 적용되지 않았다.
	if( pCtx->getNextAIStateIndex() != -1)		pAIState = pCtx->getNextAIState();
	else										pAIState = pCtx->getCurrentAIState();
	
	if( pAIState != nullptr)
	{
		if( pAIState->getAnim())	
			pszAIName = (char *) pAIState->getAnim()->GetName();
		
		// 이걸 막지 마시고 ENABLE_CHARA_ANIM_OUTPUT 디파인을 막으면 여기에 안들어옵니다. komet
		I3TRACE("- AI (%s) Anim (%s) (start %.4f, scale %.2f)\n", pAIState->GetName(), pszAIName, timeStart, pAIState->getTimeScale());
	}
	else
	{
		I3TRACE("- AI (null)\n");
	}
}

#if defined( AI_MOVE_TO_WEAPON)
void CGameCharaAnimControl::_CharaChangeAIContext( const char * pszPVState )
{
	char szAIUpperPath[ MAX_PATH] = "CharaAI:";
	char szAIUpperDual[ MAX_PATH] = {0,};

	//char * pszWeaponClass = nullptr;
	
	char * pszIdleState = "AttackIdle";	

	INT32 nSwapWeaponSlotNum = getWeaponContext()->getSwapSlotNum();
	INT32 nCurWeaponSlotNum = getWeaponContext()->getCurrentSlotNum();

	i3AIContext * UpperRoot = nullptr;
	WeaponBase * pWeapon = nullptr;

	INT32 nSwapWeaponSlotNum = pWeaponCtx->getSwapSlotNum();
	INT32 nCurWeaponSlotNum = pWeaponCtx->getCurrentSlotNum();

	if( nCurWeaponSlotNum != nSwapWeaponSlotNum)		//	다음 프레임에 바뀔 무기일 경우
		pWeapon = getWeaponContext()->getWeapon( nSwapWeaponSlotNum);
	else
		pWeapon = m_pOwner->GetCurrentCharaWeapon();

	if( pWeapon == nullptr)
		return;

	// KEYWORD : DUALSMG
	pWeapon = pWeapon->getUsedWeapon();
	I3ASSERT( pWeapon != nullptr);

	CWeaponInfo * pWeaponInfo = pWeapon->getAbsoluteWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);

	//	note. 현재 CHARA_BODYUPPER_NONE 상태는 Upper의 Dodge 또는 Death 동작에서의 상태입니다
	switch( getActionContext()->getBodyUpper())
	{
	case CHARA_BODYUPPER_NONE:			//	통합 애니메이션은 AttackIdle, Idle 구분이 없습니다.
	case CHARA_BODYUPPER_ATTACKIDLE:		pszIdleState = "AttackIdle";	break;
	case CHARA_BODYUPPER_ZOOM:
		// 줌이 없는 무기가 들어오는 경우가 있습니다. 실제로는 들어오면 안되지만 싱크 문제로 들어오는 경우가 발생합니다.
		if( !pWeaponInfo->isAnimZoomType())
			pszIdleState = "AttackIdle";
		else
			pszIdleState = "Zoom";
		break;
	case CHARA_BODYUPPER_GRENADE :
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER)
			pszIdleState = "Grenade";
		else
			pszIdleState = "AttackIdle";
		break;
	case CHARA_BODYUPPER_SHOTGUN :		//	LSS (보조 샷건)
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER)
			pszIdleState = "ShotGun";
		else
			pszIdleState = "AttackIdle";
		break;
	case CHARA_BODYUPPER_KNIFE :		//	총검
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_BAYONET)
			pszIdleState = "Knife";
		else
			pszIdleState = "AttackIdle";
		break;
	case CHARA_BODYUPPER_CROSSFIRE :	//	교차사격
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_CROSSFIRE)
			pszIdleState = "CrossIdle";
		else
			pszIdleState = "AttackIdle";
		break;
	default:						pszIdleState = "AttackIdle";	break;
	}
	
	//	Upper AI Path
	i3::generic_string_cat( szAIUpperPath, pszPVState);							//	시점
	i3::safe_string_copy( szAIUpperDual, szAIUpperPath, MAX_PATH);

	if( getActionContext()->isBody2( CHARA_BODYUPPER_NONE))
	{
		szAIUpperPath[ strlen( szAIUpperPath) - 1] = '\0';					//	No AttackIdle or Idle or Zoom
	}
	else
	{
		i3::generic_string_cat( szAIUpperPath, pszIdleState);						//	AttackIdle or Idle or Zoom
	}


	i3AIContext * pAIUpper;
	//if( pAIUpper != nullptr)	pAIUpper->SetExclusiveEnable( false, false);				// 이전 AI_Upper를 Disable 시키는 코드.

	pAIUpper = m_pOwner->FindAI( szAIUpperPath);

	I3ASSERT( pAIUpper != nullptr);
	if( pAIUpper == nullptr)
	{
		char szMsg[MAX_STRING_COUNT] = "";
		pWeaponInfo->GetWeaponName( szMsg);

		I3PRINTLOG(I3LOG_NOTICE, "Cannot find a animation node (%s) with weapon (%s)", szAIUpperPath, szMsg);
		return;
	}

	getAnimContext()->setAIUpper( pAIUpper);

	{
		i3AI * pAI = pAIUpper->getAI();
		i3AIDriver * pDriver = pAI->getAIDriver();

		I3TRACE( "AI Driver : %s\n", pDriver->GetMeta()->GetClassName());
	}

	//	UpperRoot를 기준으로 m_pAIUpper만 활성하고 관련없는 AIState는 Disable 시킨다.
	UpperRoot = m_pOwner->FindAI( "CharaAI");
	I3ASSERT(UpperRoot != nullptr);
	
	UpperRoot->setGroupEnable( pAIUpper);

	if( pWeapon->isDualGun())
	{
		i3::generic_string_cat( szAIUpperDual, "AttackIdle_Left");
		i3AIContext * pAIUpperDual = m_pOwner->FindAI( szAIUpperDual);
		if( pAIUpperDual != nullptr)
		{
			getAnimContext()->setAIUpper_Left( pAIUpperDual);
			pAIUpperDual->SetExclusiveEnable( true, false);
		}
	}
	else
	{
		getAnimContext()->setAIUpper_Left( nullptr);
	}
}

#else

void CGameCharaAnimControl::_CharaChangeAIContext( const char * pszPVState )
{
	char szAIUpperPath[ MAX_PATH] = "Upper:";
	char szAIUpperDual[ MAX_PATH] = {0,};

	char * pszWeaponClass = nullptr;
	
	char * pszIdleState = "AttackIdle";	

	i3AIContext * UpperRoot = nullptr;
	WeaponBase * pWeapon = nullptr;

	if (!getWeaponContext())
	{
		TRACE("CGameCharaAnimControl::_CharaChangeAIContext getWeaponContext() = nullptr\n");
		return;
	}
	WEAPON_SLOT_TYPE nSwapWeaponSlotNum = getWeaponContext()->getSwapSlotNum();
	WEAPON_SLOT_TYPE nCurWeaponSlotNum = getWeaponContext()->getCurrentSlotNum();

	//	다음 프레임에 바뀔 무기일 경우 - 정리 필요
	if ( !g_pFramework->IsBattleStage())
	{
		if (nCurWeaponSlotNum != nSwapWeaponSlotNum)		//	다음 프레임에 바뀔 무기일 경우
			pWeapon = getWeaponContext()->getWeapon(nSwapWeaponSlotNum);
		else
			pWeapon = m_pOwner->GetCurrentCharaWeapon();
	}
	else
	{
		if (nCurWeaponSlotNum != nSwapWeaponSlotNum && !(m_pOwner->getMultiWeaponContext()->IsPickupChange() && MyRoomInfoContext::i()->getStage()->IsMultiWeapon()))
		{
			pWeapon = getWeaponContext()->getWeapon(nSwapWeaponSlotNum);
		}
		else if ( (nSwapWeaponSlotNum == WEAPON_SLOT_PRIMARY && nCurWeaponSlotNum == WEAPON_SLOT_PRIMARY &&	MyRoomInfoContext::i()->getStage()->IsMultiWeapon()) ||
			( m_pOwner->getMultiWeaponContext()->IsPickupChange() && nSwapWeaponSlotNum == WEAPON_SLOT_PRIMARY && MyRoomInfoContext::i()->getStage()->IsMultiWeapon()))
		{
			pWeapon = m_pOwner->getMultiWeaponContext()->GetSwapMultiWeapon(false);
		}
		else
			pWeapon = m_pOwner->GetCurrentCharaWeapon();
	}

	if ( pWeapon == nullptr)
	{
		return;
	}
	else
	{
		if ( pWeapon->getWeaponInfo()->GetTypeUsage() != nSwapWeaponSlotNum)
		{
			m_pOwner->getWeaponContext()->getWeapon( nSwapWeaponSlotNum);
		}
	}

	// KEYWORD : DUALSMG
	pWeapon = pWeapon->getUsedWeapon();
	I3ASSERT( pWeapon != nullptr);

	CWeaponInfo * pWeaponInfo = pWeapon->getAbsoluteWeaponInfo();
	I3ASSERT( pWeaponInfo != nullptr);
	
	switch( pWeaponInfo->GetTypeClass())
	{
	case WEAPON_CLASS_KNIFE:			pszWeaponClass = "Knife:";			break;
	case WEAPON_CLASS_HANDGUN:			pszWeaponClass = "HandGun:";		break;
	case WEAPON_SUBCLASS_ASSAULT :
	case WEAPON_CLASS_ASSAULT:			pszWeaponClass = "AssaultRifle:";	break;
	case WEAPON_SUBCLASS_SMG :
	case WEAPON_CLASS_SMG:				pszWeaponClass = "SMG:";			break;
	case WEAPON_SUBCLASS_SNIPER :
	case WEAPON_CLASS_SNIPER:			pszWeaponClass = "SniperRifle:";	break;
	case WEAPON_SUBCLASS_SHOTGUN :
	case WEAPON_CLASS_SHOTGUN:			pszWeaponClass = "ShotGun:";		break;
	case WEAPON_CLASS_BOMBTRIGGER :
	case WEAPON_CLASS_THROWING_GRENADE:
	case WEAPON_CLASS_THROWING_CONTAIN:
	case WEAPON_CLASS_THROWING_ATTACK:
	case WEAPON_CLASS_THROWING_HEAL:	pszWeaponClass = "Throwing:";		break;
	case WEAPON_CLASS_MISSION:			pszWeaponClass = "Mission:";		break;
	case WEAPON_CLASS_MG :				pszWeaponClass = "MG:";				break;
	case WEAPON_CLASS_CIC :				pszWeaponClass = "CIC:";			break;
	case WEAPON_CLASS_DUALHANDGUN :		pszWeaponClass = "DualHandGun:";	break;
	case WEAPON_CLASS_DUALKNIFE :		pszWeaponClass = "DualKnife:";	break;
	case WEAPON_CLASS_ROCKET_LAUNCHER :	pszWeaponClass = "RocketLauncher:";	break;
	case WEAPON_CLASS_DUALSMG :			pszWeaponClass = "DualSMG:";	break;
	case WEAPON_CLASS_KNUCKLE:			pszWeaponClass = "Knife:";			break;
	case WEAPON_SUBCLASS_BOW :
	case WEAPON_CLASS_BOW:				pszWeaponClass = "Bow:";			break;
	case WEAPON_CLASS_DUALSHOTGUN :		pszWeaponClass = "DualShotGun:";	break;

	default:
		I3PRINTLOG(I3LOG_FATAL,  "잘못된 인자입니다. 무기 타입 %d", pWeaponInfo->GetTypeClass() );
		return;
	}

	//	note. 현재 CHARA_BODYUPPER_NONE 상태는 Upper의 Dodge 또는 Death 동작에서의 상태입니다
	switch( getActionContext()->getBodyUpper())
	{
	case CHARA_BODYUPPER_NONE:			//	통합 애니메이션은 AttackIdle, Idle 구분이 없습니다.
	case CHARA_BODYUPPER_ATTACKIDLE:		pszIdleState = "AttackIdle";	break;
	case CHARA_BODYUPPER_ZOOM:
		// 줌이 없는 무기가 들어오는 경우가 있습니다. 실제로는 들어오면 안되지만 싱크 문제로 들어오는 경우가 발생합니다.
		if( !pWeaponInfo->isAnimZoomType())
		{
			pszIdleState = "AttackIdle";
		}
		else
		{
			pszIdleState = "Zoom";
		}
		break;
	case CHARA_BODYUPPER_GRENADE :
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER)
			pszIdleState = "Grenade";
		else
			pszIdleState = "AttackIdle";
		break;
	case CHARA_BODYUPPER_SHOTGUN :		//	LSS (보조 샷건)
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER)
			pszIdleState = "ShotGun";
		else
			pszIdleState = "AttackIdle";
		break;
	case CHARA_BODYUPPER_KNIFE :		//	총검
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_BAYONET)
			pszIdleState = "Knife";
		else
			pszIdleState = "AttackIdle";
		break;
	case	CHARA_BODYUPPER_CROSSFIRE :
		if( pWeapon->GetCurrentExtensionType() == WEAPON::EXT_CROSSFIRE )
			pszIdleState = "CrossIdle";
		else
			pszIdleState = "AttackIdle";
		break;
	default:						pszIdleState = "AttackIdle";	break;
	}
	
	//	Upper AI Path
	i3::safe_string_cat( szAIUpperPath, pszPVState, _countof(szAIUpperPath));							//	시점
	i3::safe_string_cat( szAIUpperPath, pszWeaponClass, _countof(szAIUpperPath));						//	무기 클래스

	i3::string strFindWeapon;

	if( i3::same_of<WeaponBombTrigger*>(pWeapon))
	{
		WeaponBase * pLinkWeapon = ((WeaponBombTrigger*)pWeapon)->getBomb();
		if (pLinkWeapon)
			i3::safe_string_copy( strFindWeapon, "C5", 256 );
	}
	else
	{
		//	GetLinkedToCharaAI 항목을 무기 정보에 추가하여 FindWeapon을 대신합니다. komet
		i3::rc_wstring wstrFindWeapon;
		pWeaponInfo->GetLinkedToCharaAI( wstrFindWeapon);
		i3::utf16_to_mb(wstrFindWeapon, strFindWeapon);
	}

	i3::generic_string_cat( szAIUpperPath, strFindWeapon);					//	무기 이름
	i3::generic_string_cat( szAIUpperPath,":");

	i3::safe_string_copy( szAIUpperDual, szAIUpperPath, MAX_PATH );

	if( getActionContext()->isBody2( CHARA_BODYUPPER_NONE))
	{
		szAIUpperPath[ strlen( szAIUpperPath) - 1] = '\0';					//	No AttackIdle or Idle or Zoom
	}
	else
	{
		i3::generic_string_cat( szAIUpperPath, pszIdleState);						//	AttackIdle or Idle or Zoom
	}

	//GlobalFunc::PB_TRACE("_CharaChangeAIContext %s", szAIUpperPath);

	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	if( pAIUpper != nullptr)	pAIUpper->SetEnable( false, false);

	pAIUpper = m_pOwner->FindAI( szAIUpperPath);

	if (pAIUpper == nullptr)
	{
		TRACE("CGameCharaAnimControl::_CharaChangeAIContext pAIUpper = %s\n",szAIUpperPath);

		i3::rc_wstring wstrMsg;
		pWeaponInfo->GetWeaponName(wstrMsg);
		I3PRINTLOG(I3LOG_NOTICE, "Cannot find a animation node (%s) with weapon (%s)", szAIUpperPath, wstrMsg);
		I3ASSERT(pAIUpper != nullptr);
		return;
	}

	getAnimContext()->setAIUpper( pAIUpper);

	//	UpperRoot를 기준으로 m_pAIUpper만 활성하고 관련없는 AIState는 Disable 시킨다.
	UpperRoot = m_pOwner->FindAI( "Upper");
	I3ASSERT(UpperRoot != nullptr);
	
	UpperRoot->setGroupEnable( pAIUpper);

	if( pWeapon->isDualGun())
	{
		if( i3::generic_is_iequal( pszIdleState, "CrossIdle" )  )
		{
			i3::generic_string_cat( szAIUpperDual, "CrossIdle_Left");
		}
		else
		{
			i3::generic_string_cat( szAIUpperDual, "AttackIdle_Left");
		}

		i3AIContext * pAIUpperDual = m_pOwner->FindAI( szAIUpperDual);
		if( pAIUpperDual != nullptr)
		{
			// next AI 가 존재할 경우 초기화를 해버리는 버그가 있다.
			INT32 nextai = pAIUpperDual->getNextAIStateIndex();
			// CurAI를 진행 해야 하는데 중간에 초기화 해버려서 안넘어가는 버그 존재.
			INT32 curAI = pAIUpperDual->getCurrnetAIStateIndex();
			getAnimContext()->setAIUpper_Left( pAIUpperDual);

			// 이게 문제다
			// 해제된 Current를 접근하는 문제가 있어 임시로 nullptr로 초기화 합니다.
			pAIUpperDual->ResetCurrentAI();
			pAIUpperDual->SetEnable( true, false);

			// 다음 AI State가 존재할 경우 초기화를 해서 없애지 말고 진짜 진행 해야 합니다.
			if (nextai != -1)
			{
				REAL32 nextai_Timescale = pAIUpperDual->getAIState(nextai)->getTimeScale();
				pAIUpperDual->setCurrentAIState(nextai, 0.f, nextai_Timescale);
			}
			// 둘 중 하나만 선택해야 할듯.
			else if (curAI != -1)
			{
				REAL32 curAI_Timescale = pAIUpperDual->getAIState(curAI)->getTimeScale();
				REAL32 curAI_LocalTime = pAIUpperDual->getLocalTime();
				pAIUpperDual->setCurrentAIStateDirect(curAI, curAI_LocalTime, curAI_Timescale);
			}
		}
	}
	else
	{
		getAnimContext()->setAIUpper_Left( nullptr);
	}
}
#endif

void CGameCharaAnimControl::_DinoChangeAIContext( const char * pszPVState )
{
	char szAIUpperPath[ MAX_PATH] = "Upper:";

	i3AIContext * UpperRoot = nullptr;

	//	Upper AI Path
	i3::generic_string_cat( szAIUpperPath, pszPVState);							//	시점

	i3AIContext * pAIUpper = m_pOwner->FindAI( szAIUpperPath);
	I3ASSERT( pAIUpper != nullptr);
	
	getAnimContext()->setAIUpper( pAIUpper);

	//	UpperRoot를 기준으로 m_pAIUpper만 활성하고 관련없는 AIState는 Disable 시킨다.
	UpperRoot = m_pOwner->FindAI( "Upper");
	I3ASSERT(UpperRoot != nullptr);

	UpperRoot->setGroupEnable( pAIUpper);

	getAnimContext()->setAIUpper_Left( nullptr);
}

void CGameCharaAnimControl::OnChangeAIContext( const char * pszPVState )
{
	if(m_pOwner->IsDino())
	{
		_DinoChangeAIContext(pszPVState);
	}
	else
	{
		_CharaChangeAIContext(pszPVState);
	}
}

bool CGameCharaAnimControl::PlayUpperReturn( void)
{	//	이전에 동작중인 Upper 애니메이션으로 돌아간다.

	TRACE_FUNC;

	//	하체 애니메이션과 맞는 타임 싱크해야 합니다.
	if (getAnimContext())
	{
		REAL32 fSyncTime = getAnimContext()->GetLowerSyncTime();

		if( getActionContext()->isLower( CHARA_LOWER_RUN) )		PlayUpper( ID_UPPER_MOVE, fSyncTime);		//	이전 Move 동작으로
		else													PlayUpper( ID_UPPER_IDLE, fSyncTime);		//	이전 Idle 동작으로
	}
	else
		TRACE("CGameCharaAnimControl::PlayUpperReturn getActionContext = nullptr\n");
	
	return true;
}

bool CGameCharaAnimControl::PlayBodyLowerSync( void)
{	
	//	하체는 싱크 맞춰 플레이를 계속한다.
	getAnimContext()->ChangeAnimSetLowerSync( m_pOwner->getViewType());

	return false;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Anim Callback
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHSND_ACTRATE_FACTER 10	//발생빈도계수

#define CHSND_ACTRATE_ACCESSORY 4	//장비 달그락 거리는 소리
#define CHSND_ACTRATE_RUSTLE	8	//옷이 부시럭 거리는 소리

void CGameCharaAnimControl::OnIdleRun( i3AIContext * pCtx, REAL32 tm)
{	
	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_WEAPONCHANGE | CHARA_STATE_DISABLE_DROPWEAPON);
	PlayAutoReload();
}

void CGameCharaAnimControl::PlayAutoReload()
{

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) 
		|| m_pOwner->IsDino() == true) return;

	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();

	if( m_pOwner->isLocal() 
		&& pWeapon != nullptr
		&& pWeapon->getWeaponInfo()->GetGaugeReload() == 1)
	{
		WEAPON::eSTATE		nState = pWeapon->getWeaponState();
		//	일반 무기
		switch( nState)
		{
			// 발사 후, 총알이 없어 탄창을 갈아 끼워야 하는 상태
			//	일반 총은 Reload 동작( LoadMagazine) 하나로 끝납니다.
		case  WEAPON::STATE_NEEDTO_LOADMAG:		
			{
				ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

				if( pWeapon->getWeaponInfo()->IsLoadMagazineReady() && pWeapon->isReadyToLoadMagazine() == false &&
					getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
					nLoadType = ACTION_LOAD_READY_MAG;

				bool enable_loadmag = true;
				if( i3::same_of<WeaponDualCIC*>(getWeaponContext()->getCurrent()))
					enable_loadmag = pWeapon->getFireOrder() != WEAPON::SECONDARY_FIRE;

				if( enable_loadmag)
					m_pOwner->Cmd_Reload(nLoadType);	

				return;
			}
			break;
			// 발사 후, 장전을 해주어야 하는 상태
		case WEAPON::STATE_NEEDTO_LOADBULLET :
			{
				if( pWeapon->IsFireBulletWeapon())
				{
					m_pOwner->Cmd_Reload( ACTION_LOAD_FIRE_BULLET);	
					return;
				}
				else
				{
					m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_BULLET);	
					return;
				}
			}
			break;
		}
	}

}

void CGameCharaAnimControl::OnMoveRun( i3AIContext * pCtx, REAL32 tm)
{
	if( getAnimContext()->isEnableRunJump() == false)
	{
		REAL32 rRunElapseTime = getAnimContext()->getRunElapseTime();
		rRunElapseTime += tm;
		getAnimContext()->setRunElapseTime( rRunElapseTime);
	}
	else
	{
		
		
			REAL32 rRunJumpScale = getAnimContext()->getRunJumpAnimScale();
			REAL32 rJumpTime = 1.f;
			if( m_pOwner->getDBInfo()->GetJumpTime() > I3_EPS)
				rJumpTime = 1.f / m_pOwner->getDBInfo()->GetJumpTime();

			rRunJumpScale += tm * rJumpTime * 2.f;

			if( rRunJumpScale > 1.f)
				rRunJumpScale = 1.f;

			getAnimContext()->setRunJumpAnimScale( rRunJumpScale);

			REAL32 fScaleTime = 1.f;

			if( (getAnimContext()->getOldUpperID() == ID_UPPER_MOVE) && (rRunJumpScale > -0.65f) )
			{
				getAnimContext()->_Play_Anim_Upper( ID_UPPER_JUMP_START);
			}

			switch( getAnimContext()->getOldLowerID() )
			{
			case ID_LOWER_MOVE_FRONT :
				if( rRunJumpScale < -0.6f)		fScaleTime = 1.1f;
				else if( rRunJumpScale < 0.85f)	fScaleTime = 0.7f;
				else							fScaleTime = 0.7f;
				break;
			case ID_LOWER_MOVE_RIGHT :
				if( rRunJumpScale < -0.6f)			fScaleTime = 1.6f;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.6f;
				else								fScaleTime = 1.4f;
				break;
			case ID_LOWER_MOVE_FRONT_LEFT :
				if( rRunJumpScale < -0.6f)			fScaleTime = 1.6f;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.6f;
				else								fScaleTime = 1.4f;
				break;
			case ID_LOWER_MOVE_LEFT :
				if( rRunJumpScale < -0.6f)			fScaleTime = 1.8f;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.6f;
				else								fScaleTime = 1.3f;
				break;
			case ID_LOWER_MOVE_BACK :
				if( rRunJumpScale < -0.6f)			fScaleTime = 2.;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.6f;
				else								fScaleTime = 1.5f;
				break;
			case ID_LOWER_MOVE_FRONT_RIGHT :
				if( rRunJumpScale < -0.6f)			fScaleTime = 1.7f;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.55f;
				else								fScaleTime = 1.5f;
				break;
			case ID_LOWER_MOVE_BACK_LEFT :
				if( rRunJumpScale < -0.6f)			fScaleTime = 2.f;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.8f;
				else								fScaleTime = 1.8f;
				break;
			case ID_LOWER_MOVE_BACK_RIGHT :
				if( rRunJumpScale < -0.6f)			fScaleTime = 2.f;
				else if( rRunJumpScale < 0.85f)		fScaleTime = 0.8f;
				else								fScaleTime = 1.8f;
				break;
			}

			if( getAnimContext()->getLowerTimeScale() != fScaleTime )
				getAnimContext()->SetLowerTimeScale( fScaleTime );
		
		
		if( m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR) ) //	땅에 닿으면
			_ProcessJumpEnd();
			
	}
	PlayAutoReload();	
}

void CGameCharaAnimControl::OnAttackRun( i3AIContext * pCtx, REAL32 tm)
{
	if(m_pOwner->IsDino())
	{
		((CGameCharaDino*)((CGameCharaBase*)(m_pOwner)))->OnDinoAttackRun(pCtx, tm);

	//	if( m_pOwner->IsControlledDino())
		{
			m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);

			/*GlobalFunc::PB_TRACE("getLocalTimeMulSclae() %f tm %f getUpperActualDuration() %f getUpperDuration %f", pCtx->getLocalTimeMulSclae(), tm, getAnimContext()->getUpperActualDuration(), getAnimContext()->getUpperDuration());*/
			//	이전 행동으로 돌아옵니다.. (Idle 이나 Move)
			if( (pCtx->getLocalTimeMulSclae() + tm > getAnimContext()->getUpperDuration()) )
			{
				//GlobalFunc::PB_TRACE("Cmd_Return_Upper");
				m_pOwner->Cmd_Return_Upper();
				m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_WEAPONCHANGE);
			}
		}
	}
	else
	{
		//if( !m_pOwner->isLocal() && !m_pOwner->isAI())	return;
		if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return;
		WeaponBase * pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
		if( pCurWeapon == nullptr)		return;
		// KEYWORD : WEAPON_CIC
		if( i3::same_of<WeaponDualCIC*>(pCurWeapon))
		{
			MainWeapon_DualCIC* mainWeapon_DualCIC = static_cast<MainWeapon_DualCIC*>(pCurWeapon->GetMainWeapon());

			if( mainWeapon_DualCIC->getCurrentUseWeapon() == WEAPON_CIC_CURRENTUSE_LEFT)
			{	// 현재 왼손무기에 대한 핸들링중이라면 왼손무기에 대해서 처리
				pCurWeapon = mainWeapon_DualCIC->getLeftWeapon();
				I3ASSERT( pCurWeapon != nullptr);
			}
		}
		WEAPON::eSTATE		nState = pCurWeapon->getWeaponState();

		CWeaponInfo * pCurWeaponInfo = pCurWeapon->getWeaponInfo();
		I3ASSERT( pCurWeaponInfo != nullptr);

		if( i3::kind_of<WeaponMiniAxe*>(pCurWeapon) && pCurWeapon->getUsedWeapon() )
		{
			nState	= pCurWeapon->getUsedWeapon()->getWeaponState();
		}
		//	발사중 끝나면 Idle 상태로 돌아온다.
		if( nState == WEAPON::STATE_FIRE )
			return;

		if( i3::kind_of<WeaponMiniAxe*>(pCurWeapon))
		{
			if( i3::kind_of<WeaponMiniAxe*>(pCurWeapon->getUsedWeapon()))
			{
				if( pCurWeapon->getUsedWeapon()->getLoadedBulletCount() <= 0)
				{
					pCurWeapon->ExtensionActivate( false);
					m_pOwner->Cmd_Only_ChangeWeaponAction();
					return;
				}
			}	
		}

		//WEAPON_CLASS_TYPE	nType = pCurWeaponInfo->GetTypeClass();

		// KEYWORD : WEAPON_CIC
		if( i3::same_of<WeaponDualCIC*>( pCurWeapon->getPairWeapon()))
		{
			// revision 59544 P99 & HAK RMB 공격중 (자동)장전 안되도록 수정. hansoft.3655
			WeaponDualCIC* pCIC = static_cast<WeaponDualCIC*>(pCurWeapon->getPairWeapon());
			INT32 bullet_count = pCIC->getLoadedBulletCount() + pCIC->getTotalBulletCount();
			if( bullet_count > 0)
			{
				MainWeapon_DualCIC* mainWeapon_DualCIC = static_cast<MainWeapon_DualCIC*>(pCIC->GetMainWeapon());
				mainWeapon_DualCIC->setCurrentUseWeapon( WEAPON_CIC_CURRENTUSE_RIGHT);
			}
		}

		if( m_pOwner->isLocal() || m_pOwner->is1PVProfile() )
		{
			// 무기에 대한 사용을 가능하도록 한다.
			m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_WEAPONCHANGE | CHARA_STATE_DISABLE_DROPWEAPON);

			//	일반 무기
			switch( nState)
			{
				// 발사 후, 총알이 없어 탄창을 갈아 끼워야 하는 상태
				//	일반 총은 Reload 동작( LoadMagazine) 하나로 끝납니다.
				case  WEAPON::STATE_NEEDTO_LOADMAG:	
					{
						ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

						if( pCurWeapon->getWeaponInfo()->IsLoadMagazineReady() && pCurWeapon->isReadyToLoadMagazine() == false &&
							getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
							nLoadType = ACTION_LOAD_READY_MAG;
						
						// revision 59589 P99 & HAK RMB 공격 후 자동 장전 되지 않도록 수정.
						bool enable_loadmag = true;
						if( i3::same_of<WeaponDualCIC*>(pCurWeapon))
							enable_loadmag = pCurWeapon->getFireOrder() != WEAPON::SECONDARY_FIRE;
						
						m_pOwner->SetLoadMagChg(false);
						if( enable_loadmag)
							m_pOwner->Cmd_Reload(nLoadType);
						return;
					}
					break;
					// 발사 후, 장전을 해주어야 하는 상태
				case WEAPON::STATE_NEEDTO_LOADBULLET :
					{
						if( pCurWeapon->IsFireBulletWeapon())
						{
							m_pOwner->Cmd_Reload( ACTION_LOAD_FIRE_BULLET);	
							return;
						}
						else
						{
							m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_BULLET);	
							return;
						}
					}
					break;
	
					//	연발 발사합니다.
				case WEAPON::STATE_NEEDTO_FIRE:	
						m_pOwner->Cmd_Attack(); return;
						break;
					// 탄약이 없는 경우
				case WEAPON::STATE_EMPTY_MAGAZINE:
					{
						// 다중무기를 사용 중이면 무조건 무기를 버린 후 교체해야하기 때문에 이곳에서 처리하면 안됩니다. 주무기가 한 개 남았을 경우는 예외...
						if ( MyRoomInfoContext::i()->getStage()->IsMultiWeapon() && pCurWeaponInfo->GetTypeUsage() == WEAPON_SLOT_PRIMARY )
						{
							if ( m_pOwner->getMultiWeaponContext()->GetMultiWeaponCount() > 1)
							{
								GAMEEVENT::Write_Chara_DropWeapon(m_pOwner, pCurWeaponInfo->GetTypeUsage(), pCurWeapon->getLoadedBulletCount(), pCurWeapon->getLoadedBulletDualCount(), pCurWeapon->getTotalBulletCount());

								return;
							}
						}

						if( i3::same_of<WeaponKrissSuperV*>(pCurWeapon))
						{
							I3ASSERT( ((WeaponKrissSuperV*)pCurWeapon)->getDualWeapon() );
							if( !((WeaponKrissSuperV*)pCurWeapon)->isExtensionActivate() )
							{
								
	#if defined( USE_AUTO_DROPWEAPON)
								GAMEEVENT::Write_Chara_DropWeapon( m_pOwner, pCurWeaponInfo->GetTypeUsage(), pCurWeapon->getLoadedBulletCount(), pCurWeapon->getLoadedBulletDualCount(), pCurWeapon->getTotalBulletCount() );
	#else
								m_pOwner->Cmd_ChangeWeapon( getWeaponContext()->getNextUsableWeaponSlot());
	#endif
							}
						}
						else if( i3::same_of<WeaponShotGunGenderWithDual*>(pCurWeapon) )
						{
							I3ASSERT( ((WeaponShotGunGenderWithDual*)pCurWeapon)->getDualWeapon() );
							if( !((WeaponShotGunGenderWithDual*)pCurWeapon)->isExtensionActivate() )
							{

#if defined( USE_AUTO_DROPWEAPON)
								GAMEEVENT::Write_Chara_DropWeapon( m_pOwner, pCurWeaponInfo->GetTypeUsage(), pCurWeapon->getLoadedBulletCount(), pCurWeapon->getLoadedBulletDualCount(), pCurWeapon->getTotalBulletCount() );
#else
								m_pOwner->Cmd_ChangeWeapon( getWeaponContext()->getNextUsableWeaponSlot());
#endif
							}
						}
						else
						{
							//	수류탄이나 아이템일 경우 EMPTY면 0번 무기로 교체
							if( (pCurWeaponInfo->GetTypeUsage() >= WEAPON_SLOT_MELEE) )
							{
								INT32 nIdx = 0;
								
								// 0번 무기가 없으면 다음 번호의 무기로 교체
								while( !getWeaponContext()->checkUsableWeaponSlot( static_cast<WEAPON_SLOT_TYPE>(nIdx), false) )
								{
									if( nIdx < MAX_WEAPON_CHECK_SLOT_COUNT )	nIdx++;
									else										nIdx = 0;

									if( nIdx == m_pOwner->getWeaponContext()->getCurrentSlotNum())	//	한바퀴 돌면( 바꿀 무기가 없다면)
									{
										nIdx = -1;
										break;
									}
								}

								m_pOwner->Cmd_ChangeWeapon( static_cast<WEAPON_SLOT_TYPE>(nIdx));
	#if defined( USE_AUTO_DROPWEAPON)
							}
							else
							{
								GAMEEVENT::Write_Chara_DropWeapon( m_pOwner, pCurWeaponInfo->GetTypeUsage(), pCurWeapon->getLoadedBulletCount(), pCurWeapon->getLoadedBulletDualCount(), pCurWeapon->getTotalBulletCount() );
							}
	#else
							}
							else
								m_pOwner->Cmd_ChangeWeapon( getWeaponContext()->getNextUsableWeaponSlot());
	#endif
						}
						return;
					}
					break;

				case WEAPON::STATE_NEEDTO_QUICKLOAD :	m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_QUICK);	return;
				case WEAPON::STATE_NEEDTO_REMOVESHELL :	m_pOwner->Cmd_Reload( ACTION_LOAD_REMOVE_SHELL);return;

				case WEAPON::STATE_NEEDTO_DROPWEAPON:
					{
						GAMEEVENT::Write_Chara_DropWeapon( m_pOwner, pCurWeaponInfo->GetTypeUsage(), pCurWeapon->getLoadedBulletCount(), pCurWeapon->getLoadedBulletDualCount(), pCurWeapon->getTotalBulletCount() );
						
						WeaponRocketLauncher * pLauncher = i3::same_cast<WeaponRocketLauncher*>(pCurWeapon);
						if(pLauncher != nullptr)
							pLauncher->SetDropLauncher(true);
					}
					return;
			}
		}

		//2015_03_18 ENG_김대영 연발이고 마우스에서 손을 안뗏다면 무기 발사 완료 처리 하면 안됩니다. (단, WAIT 상태에서는 완료처리 할 수 있도록 합니다. 3점샷 같은 무기 때문에 )
		UINT64 keyPress = getInputContext()->getKeyPressInput();
		if( (nState !=  WEAPON::STATE_WAIT && keyPress & GAME_KEY_ACTION_ATTACK) != 0 && pCurWeapon->getWeaponInfo()->GetAutoFireCount() == 0 )
			return;

		// 무기 발사 완료
		//	이전 행동으로 돌아옵니다.. (Idle 이나 Move)
		if( ( getAnimContext()->getUpperLocalTime() + tm > getAnimContext()->getUpperActualDuration()) )
		{
			switch(pCurWeaponInfo->GetTypeClass())
			{
			case WEAPON_CLASS_THROWING_GRENADE:
			case WEAPON_CLASS_THROWING_CONTAIN:
			case WEAPON_CLASS_THROWING_ATTACK:
			case WEAPON_CLASS_THROWING_HEAL:
				{
					pCurWeapon->ReturnToReady();
					pCurWeapon->SetEnable( true);
					getActionContext()->setUpper( CHARA_UPPER_SWAPWEAPON);
					getAnimContext()->_Play_Anim_Upper( ID_UPPER_SWAP_WEAPON);
				}
				break;
			default:
				{
					if( m_pOwner->is1PV() && i3::same_of<CGameCamera_FPS*>(g_pCamera))
					{
						((CGameCamera_FPS*)g_pCamera)->SetNeckAnim(false);
					}

					bool returnupper = false;
					if (m_pOwner->getCharaNetIndex() == BattleSlotContext::i()->getMySlotIdx())
					{
						if (nState != WEAPON::STATE_FIRING) returnupper = true;
					}
					else
					{
						if (nState != WEAPON::STATE_FIRING)
						{
							if (m_pOwner->IsNetworkAttackReturnUpper())
							{
								returnupper = true;
								m_pOwner->ResetNetworkAttackCount();
							}
							else
							{
								m_pOwner->NetworkAttackCountPlus(tm);
							}
						}
					}

					if(returnupper)
						m_pOwner->Cmd_Return_Upper();
				}
				break;
			}
		}
	}
}

void CGameCharaAnimControl::OnAttackFinish( i3AIContext * pCtx, REAL32 tm)
{
}

void CGameCharaAnimControl::OnAttackReadyFinish( i3AIContext * pCtx, REAL32 tm)
{
	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);		//	준비 끝난후 바로 발사 가능
}

void CGameCharaAnimControl::OnJumpStartRun( i3AIContext * pCtx, REAL32 tm)
{
	//if( !getActionContext()->isLower( CHARA_LOWER_JUMP))
	//	return;
	
	if( !m_pOwner->isLocal() && getAnimContext()->isEnableRunJump() )
	{
		ID_LOWER_AI nId = getAnimContext()->getMoveIdByDir( getMoveContext()->getMoveDir());
		REAL32 rSyncTime = 0.f;
		if( getAnimContext()->getRunJumpStepTime() > 0.1f && getAnimContext()->getRunJumpStepTime() < 0.4f)
		{
			rSyncTime = 0.31f;
		}

		getAnimContext()->_Play_Anim_Lower( nId, rSyncTime);

		if( (getAnimContext()->getOldUpperID() == ID_UPPER_JUMP_START) )
		{
			getAnimContext()->_Play_Anim_Upper( ID_UPPER_MOVE);
		}
	}

		if( m_pOwner->isCharaStateMask( CHARA_STATE_FLOOR))	//	땅에 닿으면
		_ProcessJumpEnd();
}

void CGameCharaAnimControl::OnJumpEndRun( i3AIContext * pCtx, REAL32 tm)
{
	if( getAnimContext()->getUpperLocalTime() + tm >= getAnimContext()->getUpperDuration())
	{
		// revision 33110
		if(m_pOwner->IsDino())
		{
			m_pOwner->Cmd_Return_Upper();
			m_pOwner->Cmd_Return_Lower();
		}
		else
		{
			//	이전 동작이었던 Upper의 Idle 또는 Move로 돌아온다.
			// revision 31983
			WeaponBase * pCurWeapon = m_pOwner->GetCurrentCharaWeapon();
			// revision 32656
			//pumpaction weapons는 재장전 방식이 다른총기류와 달라 점프애니메이션후 상체를 초기화 시킬경우 다음 장전동작을 하지 않아 예외처리합니다. 
			if( pCurWeapon != nullptr && pCurWeapon->getWeaponInfo()->GetPumpAction())
			{
				m_pOwner->Cmd_Return_Lower();
			}
			else
			{
				m_pOwner->Cmd_Return_Upper();
				m_pOwner->Cmd_Return_Lower();
			}
		}
	}
}

void CGameCharaAnimControl::OnJumpEndFinish( i3AIContext * pCtx, REAL32 tm)
{
	//	다른 시점으로 변환중 애니메이션이 다르더라고 같은 동작이면 이전 동작으로 돌아가지 않는다.		
	if( getAnimContext()->getUpperLocalTime() + tm >= getAnimContext()->getUpperDuration())
	{
		// revision 33110
		if(m_pOwner->IsDino())
		{
			m_pOwner->Cmd_Return_Upper();
			m_pOwner->Cmd_Return_Lower();
		}
		else
		{
			//	이전 동작이었던 Upper의 Idle 또는 Move로 돌아온다.
			// revision 31983
			//M1887류의 총기는 재장전 방식이 다른총기류와 달라 점프애니메이션후 상체를 초기화 시킬경우 다음 장전동작을 하지 않아 예외처리합니다. 
			WeaponBase * pCurWeapon = getWeaponContext()->getCurrent();
			if( pCurWeapon != nullptr
				&& pCurWeapon->getWeaponInfo()->GetPumpAction() )
			{
				m_pOwner->Cmd_Return_Lower();
			}
			else
			{
				m_pOwner->Cmd_Return_Upper();
				m_pOwner->Cmd_Return_Lower();
			}
		}
	}
}

void CGameCharaAnimControl::OnLoadMagazineRun( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pWeapon == nullptr)
		return;

	if( i3::same_of<WeaponKrissSuperV*>(pWeapon))
	{
		if( ((WeaponKrissSuperV*)pWeapon)->isUsedDualWeapon())
		{	
			if( ((WeaponKrissSuperV*)pWeapon)->getDualWeapon()->getWeaponState() == WEAPON::STATE_NEEDTO_LOADBULLET )
			{
				((WeaponKrissSuperV*)pWeapon)->SetEnableDualWeapon( false);
			}
		}
	}

	switch( pWeapon->getWeaponState())
	{
		case  WEAPON::STATE_NEEDTO_LOADMAG:
			{
				ACTION_LOAD_MAG_TYPE nLoadType = ACTION_LOAD_MAG;

				if( pWeapon->getWeaponInfo()->IsLoadMagazineReady() && pWeapon->isReadyToLoadMagazine() == false && 
					getActionContext()->isUpper(CHARA_UPPER_LOADMAGAZINE) == false)
					nLoadType = ACTION_LOAD_READY_MAG;

				m_pOwner->Cmd_Reload(nLoadType);
			}
			break;

		case WEAPON::STATE_NEEDTO_LOADBULLET :
			m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_BULLET);
			break;

		case WEAPON::STATE_NEEDTO_REMOVESHELL :
			break;

		case WEAPON::STATE_READY:
			{	
				//	샷건 외에는 일반 무기는 Idle 상태로 돌아가야 하기 때문에
				//	OnLoadBullet을 호출하지 않기 때문에 (AIState가 없으므로) 호출해야 합니다.
				OnLoadBulletRun( pCtx, tm);	
			}
			break;
		case WEAPON::STATE_NEEDTO_LOAD_B_MAG:
			//GlobalFunc::PB_TRACE("CGameCharaAnimControl::OnLoadMagazineRun WEAPON::STATE_NEEDTO_LOAD_B_MAG\n");
			m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_B);
			break;
		default :
			break;
	}
}

void CGameCharaAnimControl::OnLoadBulletRun( i3AIContext * pCtx, REAL32 tm)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return;

	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pWeapon == nullptr)		return;

	//	장전이 끝나면
	if( pWeapon->getWeaponState() == WEAPON::STATE_READY )
	{
		// KEYWORD : WEAPON_GRENADELAUNCHER
		if( i3::kind_of<WeaponGrenadeLauncher*>(pWeapon)
			&& pWeapon->GetCurrentExtensionType() == WEAPON::EXT_GLAUNCHER
			&& pWeapon->isExtensionActivate() )
			getActionContext()->ApplyActionBody2( CHARA_BODYUPPER_GRENADE);	
		// KEYWORD : WEAPON_SHOTGUNLAUNCHER
		else if( i3::kind_of<WeaponShotGunLauncher*>(pWeapon)
			&& pWeapon->GetCurrentExtensionType() == WEAPON::EXT_SHOTGUN_LAUNCHER
			&& pWeapon->isExtensionActivate() )
			getActionContext()->ApplyActionBody2( CHARA_BODYUPPER_SHOTGUN);	
		else
			getActionContext()->ApplyActionBody2( CHARA_BODYUPPER_ATTACKIDLE);		//	Idle 상태는 AttackIdle 상태로 한다.

		if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM ) )
			m_pOwner->Cmd_Extension( false);
	
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE 
			| CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_ZOOM | CHARA_STATE_DISABLE_WEAPONCHANGE);

		//	이전 행동으로 돌아옵니다.. (Idle 이나 Move)
		m_pOwner->Cmd_Return_Upper();
	}
}

void CGameCharaAnimControl::OnFireBulletRun( i3AIContext * pCtx, REAL32 tm)
{
	OnLoadBulletRun( pCtx, tm);
}

void CGameCharaAnimControl::OnWeaponSwapRun( i3AIContext * pCtx, REAL32 tm)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) ) return;

	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pWeapon == nullptr)		return;

	//	다른 시점으로 변환중 애니메이션이 다르더라고 같은 동작이면 이전 동작으로 돌아가지 않는다.		
	//	동작이 끝났다.
	if( getAnimContext()->getUpperLocalTime() > getAnimContext()->getUpperActualDuration())
	{
		//	이전 동작이었던 Upper의 Idle 또는 Move로 돌아온다.
		m_pOwner->Cmd_Return_Upper();
		
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);
	}
	else	//	진행중
		m_pOwner->addEnableStateMask( CHARA_STATE_DISABLE_SHOT);

	m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_WEAPONCHANGE | CHARA_STATE_DISABLE_DROPWEAPON);
}

void CGameCharaAnimControl::OnDamageRun( i3AIContext * pCtx, REAL32 tm)
{
	if( !getActionContext()->isUpper( CHARA_UPPER_DAMAGE) ||	m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return;

	//	다른 시점으로 변환중 애니메이션이 다르더라고 같은 동작이면 이전 동작으로 돌아가지 않는다.
	if( getAnimContext()->getUpperLocalTime() >= getAnimContext()->getUpperDuration())
	{
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT | CHARA_STATE_DISABLE_MOVE
			| CHARA_STATE_DISABLE_CROUCH | CHARA_STATE_DISABLE_WEAPONCHANGE | CHARA_STATE_DISABLE_DROPWEAPON);

		//	이전 행동으로 돌아옵니다.. (Idle 이나 Move)
		m_pOwner->Cmd_Return_Upper();
	}
}

void CGameCharaAnimControl::OnDeathFinish( i3AIContext * pCtx, REAL32 tm)
{
	CGameMissionManager * pMissionMng = CGameMissionManager::i();
	I3ASSERT( pMissionMng != nullptr);

	if( g_pFramework->IsBattleStage() == false)
		return;

	//	리스폰 타이머 설정
	if( CStageInfoUtil::IsEternerlRespawn(*(MyRoomInfoContext::i()->getStage())) )
	{
		REAL32 rRespawnTime = 0.0f;
			
		if( m_pOwner->isAI())	//	AI일 경우 따로 지정된 값을 사용
		{
			rRespawnTime = m_pOwner->getDBInfo()->GetNPCRespawnTime();
		}
		else
		{
			rRespawnTime = MyRoomInfoContext::i()->GetRespawnTime( m_pOwner->getCharaNetIndex());
			
			//	자신 또는 유저가 조작하는 캐릭터일 경우 리스폰시 StageBattle에서 RespawnDelayTime이 존재하기 때문에
			//if( m_pOwner->isLocal() || m_pOwner->isNetwork())
			{
				const REAL32 fRespawnDelayTime = 2.0f;

				rRespawnTime += fRespawnDelayTime;
			}
		}

		m_pOwner->setRespawnElapseTime( rRespawnTime);
	}
}

void CGameCharaAnimControl::OnExtensionActivateRun( i3AIContext * pCtx, REAL32 tm)
{
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return;
	
	if( getAnimContext()->getUpperLocalTime() + tm > getAnimContext()->getUpperDuration())
	{
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		m_pOwner->Cmd_Return_Upper();
		m_pOwner->Cmd_ApplyCrosshair();
	}
}

void CGameCharaAnimControl::OnExtensionDeactivateRun( i3AIContext * pCtx, REAL32 tm)
{ 
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH))	return;

	if( getAnimContext()->getUpperLocalTime() + tm > getAnimContext()->getUpperDuration())
	{
		m_pOwner->removeEnableStateMask( CHARA_STATE_DISABLE_SHOT);
		m_pOwner->Cmd_Return_Upper();
		m_pOwner->Cmd_ApplyCrosshair();
	}
}

void CGameCharaAnimControl::OnUI_IdleBRun( i3AIContext * pCtx, REAL32 tm)
{
	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	if( pAIUpper == nullptr)
		return;

	i3AIState * pAIState = pAIUpper->getCurrentAIState();

	if( pAIState == nullptr)
		return;

	if( getAnimContext()->getUpperAniTimer() + tm > pAIState->getDuration())
	{
		m_pOwner->Cmd_Profile_Idle_Stand();
		getAnimContext()->setUpperAniTimer( 0.f);
	}
}

void CGameCharaAnimControl::OnUI_IdleBFinish( i3AIContext * pCtx, REAL32 tm)
{

	// 기본 Idle로 값니다
	if (false == g_pFramework->IsBattleStage())	
	{
		m_pOwner->Cmd_Profile_Idle_Stand();
		getAnimContext()->setUpperAniTimer( 0.f);
	}
	else
	{
		if (g_pFramework->GetCurrentStage())
		{
			INT32 idx = m_pOwner->getCharaNetIndex();
			char szTrace[ 256] = { 0,};
			i3::snprintf( szTrace, _countof(szTrace) ,"잘못된 애니메니이션. 유저:%s(%d), ", BattleSlotContext::i()->getNickForSlot( idx), idx );
			WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
			if( pWeapon != nullptr)
			{
				i3::rc_wstring wstrWeaponName;
				pWeapon->getWeaponInfo()->GetWeaponName( wstrWeaponName);
				i3::snprintf( szTrace, 256, "무기:%s", wstrWeaponName);	// 자동형변환을 기대한다.
			}
			I3TRACE( szTrace);
		}
		else
		{
			// CurrentStage가 nullptr이다. 스테이지 점프 중 호출되었다.
			I3TRACE("[WARNING] CGameCharaAnimControl::OnUI_IdleAFinish() - currentStage is nullptr\n");			
		}
	}
}

void CGameCharaAnimControl::OnUI_IdleChangeRun( i3AIContext * pCtx, REAL32 tm)
{
	i3AIContext * pAIUpper = getAnimContext()->getAIUpper();
	if( pAIUpper == nullptr)
		return;

	i3AIState * pAIState = pAIUpper->getCurrentAIState();

	if( pAIState == nullptr)
		return;

	if( getAnimContext()->getUpperAniTimer() + tm > pAIState->getDuration())
	{
		m_pOwner->Cmd_Profile_Idle_Stand();
		getAnimContext()->setUpperAniTimer( 0.f);
	}
}

bool CGameCharaAnimControl::PlayDinoLower( ID_LOWER_DINO_AI id, REAL32 timeStart, REAL32 timeScale)
{
	bool bRv = false;
	i3AIContext * pAILower = getAnimContext()->getAILower();

	// 처음에는 초기화 상태로 UNKNOWN상태가 들어올 수 있습니다.
	// 기본적으로 IDLE의 동작을 해줍니다.
	if( (id <= ID_LOWER_DINO_UNKNOWN) || (id >= ID_LOWER_DINO_AI_MAX) )
		id = ID_LOWER_DINO_IDLE;

	//	현재 동작이 무엇인지 저장한다.
	((CGameDinoAnimContext*)getAnimContext())->setOldDinoLowerID( id);

	if( pAILower != nullptr)
	{
		INT32 nAIIdx = 0;

		nAIIdx = ((CGameDinoAnimContext*)getAnimContext())->getDinoAILowerID( id);

		if (nAIIdx >= 0)
		{
			//GlobalFunc::PB_TRACE("nAIIdx %d", nAIIdx);
			pAILower->setCurrentAIState( nAIIdx, timeStart, timeScale);		//	하체 AI 상태 변경

			bRv = true;

#if defined( ENABLE_CHARA_ANIM_OUTPUT )
			{
				TRACE_FUNC;
				AnimDebug( pAILower, timeStart );
			}
#endif
		}
	}

	return bRv;
}