#include "pch.h"
#include "GlobalVariables.h"
#include "GameCharaBase.h"
#include "GameCamera.h"
#include "StageBattle.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"


// 네트워크 보정을 위해 값의 오차를 넣었습니다 by 정순구
//#define NET_MOVE_DIR_RANGE_VAL			0.3826834323650897717284599840304f
/*
void CGameCharaBase::ACTION_apply( BOOL bFixed3PV)
{	
	CHARA_ANIMSET_TYPE AnimSet = getAnimSet();

	char szAIUpperPath[256] = "Upper:";
	char szAILowPath[256] = "Lower:";

	char * pszPVState = "3PV:";
	char * pszWeaponClass = NULL;
	
	char * pszStandState = "Stand:";
	char * pszIdleState = "AttackIdle";	

	i3AIContext * UpperRoot = NULL;
	WeaponBase * pWeapon = NULL;

	if( getWeaponContext()->getCurrentSlotNum() != getWeaponContext()->getSwapSlotNum())		//	다음 프레임에 바뀔 무기일 경우
		pWeapon = getWeaponContext()->getWeapon( getWeaponContext()->getSwapSlotNum());
	else
		pWeapon = getWeaponContext()->getCurrent();

	if( pWeapon == NULL )
	{	// 무기가 없으면 스킵합니다. 네트워크 문제가 생길 수 있습니다. - by dfly
		return;
	}
	
	switch( pWeapon->getWeaponInfo()->m_TypeClass)
	{
	case WEAPON_CLASS_KNIFE:	pszWeaponClass = "Knife:";			break;
	case WEAPON_CLASS_HANDGUN:	pszWeaponClass = "HandGun:";		break;
	case WEAPON_CLASS_ASSAULT:	pszWeaponClass = "AssaultRifle:";	break;
	case WEAPON_CLASS_SMG:		pszWeaponClass = "SMG:";			break;
	case WEAPON_CLASS_SNIPER:	pszWeaponClass = "SniperRifle:";	break;
	case WEAPON_CLASS_SHOTGUN:	pszWeaponClass = "ShotGun:";		break;
	case WEAPON_CLASS_THROWING:	pszWeaponClass = "Throwing:";		break;
	case WEAPON_CLASS_ITEM:		pszWeaponClass = "Item:";			break;
	case WEAPON_CLASS_MISSION:	pszWeaponClass = "Mission:";		break;
	case WEAPON_CLASS_MG :		pszWeaponClass = "MG:";				break;
	default:	I3ASSERT_0;		break;
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	//	애니메이션 설정
	// ! praptor - 관전 카메라 관련으로 수정
	
	//if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
	{
	//	CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
	//	if( pCharacter == this ) bFixed3PV = FALSE;
	}
	

	if( bFixed3PV)
	{
		pszPVState = "3PV:";			//	시점에 관계없이 3인칭 고정	
		AnimSet = CHARA_ANIMSET_3PV;
	}
	else
	{
		if( AnimSet == CHARA_ANIMSET_1PV)			pszPVState = "1PV:";		//	1인칭 캐릭터이다.
		else if( AnimSet == CHARA_ANIMSET_3PV)		pszPVState = "3PV:";		//	3인칭 캐릭터이다.
		else I3ASSERT_0;	// ! praptor - 관전 카메라 관련 추가
	}
	
	switch( getActionContext()->getBody())
	{
	case CHARA_BODY_STAND:		pszStandState = "Stand:";		break;
	case CHARA_BODY_CROUCH:		pszStandState = "Sit:";			break;	
	default:					pszStandState = "Stand:";		break;
	}

	//	note. 현재 CHARA_BODY_NONE 상태는 Upper의 Dodge 또는 Death 동작에서의 상태입니다
	switch( getActionContext()->getBody2())
	{
	case CHARA_BODY_NONE:			pszIdleState = "";				break;		//	통합 애니메이션은 AttackIdle, Idle 구분이 없습니다.
	//case CHARA_BODY_IDLE:			pszIdleState = "Idle";			break;
	case CHARA_BODY_ATTACKIDLE:		pszIdleState = "AttackIdle";	break;
	case CHARA_BODY_ZOOM:			pszIdleState = "Zoom";			break;
	default:						pszIdleState = "AttackIdle";	break;
	}
		
	//	1인칭에서는 상하체 통합 애니메이션이 없으므로 강제로 3인칭용 애니메이션을 플레이한다.
	if( getActionContext()->isBody2( CHARA_BODY_NONE ))
		pszPVState = "3PV:";

	//	Upper AI Path
	i3String::Concat( szAIUpperPath, pszPVState);							//	시점
	i3String::Concat( szAIUpperPath, pszWeaponClass);						//	무기 클래스

	char szFindWeapon[ 256] = {0,};
	for( INT32 i = 0; i < i3String::Length( pWeapon->GetName()); i++)
	{
		if( pWeapon->GetName()[ i] == '_')
			break;
		szFindWeapon[ i] = pWeapon->GetName()[i];
	}

	i3String::Concat( szAIUpperPath, szFindWeapon);					//	무기 이름
	i3String::Concat( szAIUpperPath,":");
	//i3String::Concat( szAIUpperPath, pszStandState);						//	Stand or Sit

	if( getActionContext()->isBody2( CHARA_BODY_NONE))
	{
		szAIUpperPath[ strlen( szAIUpperPath) - 1] = '\0';					//	No AttackIdle or Idle or Zoom
	}
	else
	{
		i3String::Concat( szAIUpperPath, pszIdleState);						//	AttackIdle or Idle or Zoom
	}

	//	Lower AI Path
	i3String::Concat( szAILowPath, pszStandState);
	szAILowPath[ strlen( szAILowPath) - 1] = '\0';

	switch( AnimSet)
	{
	//	1인칭 일때
	case CHARA_ANIMSET_1PV:
		{
			#if 1
				if( m_pAnimContext->isAILower())		
					m_pAnimContext->getAILower()->SetEnable( false, true);

				m_pAnimContext->setAILower( NULL);
			#else
				m_pAnimContext->setAILower( FindAI( szAILowPath));
				I3ASSERT( m_pAnimContext->getAILower() != NULL);
			#endif

			m_pAnimContext->setAIUpper( FindAI( szAIUpperPath));
			I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

			//	UpperRoot를 기준으로 m_pAnimContext->getAIUpper()만 활성하고 관련없는 AIState는 Disable 시킨다.
			UpperRoot = FindAI( "Upper");
			UpperRoot->setGroupEnable( m_pAnimContext->getAIUpper());		
		}
		break;
	//	3인칭 일때
	case CHARA_ANIMSET_3PV:
		{
			m_pAnimContext->setAILower( FindAI( szAILowPath));
			I3ASSERT( m_pAnimContext->getAILower() != NULL);

			//	UpperRoot를 기준으로 m_pAnimContext->getAILower()만 활성하고 관련없는 AIState는 Disable 시킨다.
			UpperRoot = FindAI( "Lower");
			UpperRoot->setGroupEnable( m_pAnimContext->getAILower());

			m_pAnimContext->setAIUpper( FindAI( szAIUpperPath));
			I3ASSERT( m_pAnimContext->getAIUpper() != NULL);

			//	UpperRoot를 기준으로 m_pAnimContext->getAIUpper()만 활성하고 관련없는 AIState는 Disable 시킨다.
			UpperRoot = FindAI( "Upper");
			UpperRoot->setGroupEnable( m_pAnimContext->getAIUpper());
		}
		break;
	default: I3ASSERT_0;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	ANIM
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CGameCharaBase::ChangeAnimSetLowerSync( CHARA_ANIMSET_TYPE type, BOOL isFixed3PV)
{
	BOOL bNeedStopAnim = (m_AnimSet != type);
	
	m_AnimSet = type;
	
	//ACTION_apply( isFixed3PV);
	getAnimContext()->getControl()->AnimSetApply();
	
	REAL32 fLowerSyncTime = 0.0f;

	//	반드시 ANIM_Stop하기 전에 플레이 시간을 받아 놓아야 한다. stop을 하면 Local Time이 0 이 되므로...주의
	fLowerSyncTime = GetSyncTimeLower();

	//	1인칭으로 바뀐 경우는 진행중인 3인칭 하체 애니메이션에 영향을 받으면 안된다.
	//BOOL bNetFPSView = FALSE;
	//if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
	{	
	//	CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
	//	if( pCharacter != g_pPlayer && pCharacter == this ) bNetFPSView = TRUE;
	}
	

	if( bNeedStopAnim )
	{
		_ANIM_stop();
	}

	//	시점이 변경될 경우 애니메이션 루트 자체가 변경되기 때문에 새로 싱크를 맞춰 플레이해야 한다.
	//	똑같은 동작으로 보이지만 1인칭이냐 3인칭이냐에 따라 전혀 다른 애니메이션 파일이기 때문이다.
	ANIM_playLower( getActionContext()->getLower(), (ID_LOWER_AI)m_pAnimContext->getOldLowerID(), fLowerSyncTime);
}

void CGameCharaBase::ChangeAnimSet( CHARA_ANIMSET_TYPE type, BOOL bAnimSync, BOOL isFixed3PV)
{
	BOOL bNeedStopAnim = (m_AnimSet != type);
	
	setAnimSet( type);
	
	//ACTION_apply( isFixed3PV);
	getAnimContext()->getControl()->AnimSetApply();
		
	if( bAnimSync)
	{
		REAL32 fLowerSyncTime = 0.0f;
		REAL32 fUpperSyncTime = 0.0f;

		//	반드시 ANIM_Stop하기 전에 플레이 시간을 받아 놓아야 한다. stop을 하면 Local Time이 0 이 되므로...주의
		fLowerSyncTime = GetSyncTimeLower();
		fUpperSyncTime = GetSyncTimeUpper();

		//	1인칭으로 바뀐 경우는 진행중인 3인칭 하체 애니메이션에 영향을 받으면 안된다.
		
		//BOOL bNetFPSView = FALSE;
		//if( g_pCamera && g_pCamera->IsExactTypeOf(CGameCamera_FPS::GetClassMeta()) )
		{	
		//	CGameCharaPlayer* pCharacter = g_pCamera->getCurTarget();
		//	if( pCharacter != g_pPlayer && pCharacter == this ) bNetFPSView = TRUE;
		}
		
		// || bNetFPSView

		if( bNeedStopAnim  )			_ANIM_stop();
		

		//	시점이 변경될 경우 애니메이션 루트 자체가 변경되기 때문에 새로 싱크를 맞춰 플레이해야 한다.
		//	똑같은 동작으로 보이지만 1인칭이냐 3인칭이냐에 따라 전혀 다른 애니메이션 파일이기 때문이다.
		ANIM_playLower( getActionContext()->getLower(), (ID_LOWER_AI)m_pAnimContext->getOldLowerID(), fLowerSyncTime);
		ANIM_playUpper( getActionContext()->getUpper(), (ID_UPPER_AI)m_pAnimContext->getOldUpperID(), fUpperSyncTime);
		
	}
}

/*

ID_LOWER_AI CGameCharaBase::_ANIM_getMoveId( VEC3D * vDir)
{
	//char * pszAIName = "Idle";	
	
	// 네트워크 보정을 위해 값의 오차를 넣었습니다 by 정순구
	if( vDir->z > NET_MOVE_DIR_RANGE_VAL)	// 0.f)
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_MOVE_FRONT_LEFT;	//pszAIName = "FrontLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_MOVE_FRONT_RIGHT;	//pszAIName = "FrontRight";
		else											return ID_LOWER_MOVE_FRONT;	//pszAIName = "Front";
	}
	else if( vDir->z < -NET_MOVE_DIR_RANGE_VAL) //0.f) 
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_MOVE_BACK_LEFT;	//pszAIName = "BackLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_MOVE_BACK_RIGHT;	//pszAIName = "BackRight";
		else											return ID_LOWER_MOVE_BACK;	//pszAIName = "Back";
	}
	else
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_MOVE_LEFT;	//pszAIName = "Left";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_MOVE_RIGHT;	//pszAIName = "Right";
	}
	
	//return pszAIName;
	return ID_LOWER_IDLE;
}

ID_LOWER_AI CGameCharaBase::_ANIM_getWalkId( VEC3D * vDir)
{
	//char * pszAIName = ID_LOWER_IDLE;	
	
	// 네트워크 보정을 위해 값의 오차를 넣었습니다 by 정순구
	if( vDir->z > NET_MOVE_DIR_RANGE_VAL)	// 0.f)
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_WALK_FRONT_LEFT;	//pszAIName = "FrontLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_WALK_FRONT_RIGHT;	//pszAIName = "FrontRight";
		else											return ID_LOWER_WALK_FRONT;	//pszAIName = "Front";
	}
	else if( vDir->z < -NET_MOVE_DIR_RANGE_VAL) //0.f) 
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_WALK_BACK_LEFT;	//pszAIName = "BackLeft";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_WALK_BACK_RIGHT;	//pszAIName = "BackRight";
		else											return ID_LOWER_WALK_BACK;	//pszAIName = "Back";
	}
	else
	{
		if( vDir->x > NET_MOVE_DIR_RANGE_VAL)			return ID_LOWER_WALK_LEFT;	//pszAIName = "Left";
		else if( vDir->x < -NET_MOVE_DIR_RANGE_VAL)		return ID_LOWER_WALK_RIGHT;	//pszAIName = "Right";
	}
	
	//return pszAIName;
	return ID_LOWER_IDLE;
}


BOOL CGameCharaBase::_ANIM_playBodyLowerSync( CHARA_ACTION_BODY state, BOOL bIs3PV)
{
	if( getActionContext()->isBody( state))	return FALSE;

	getActionContext()->setBody( state);

	//	하체는 싱크 맞춰 플레이를 계속한다.
	ChangeAnimSetLowerSync( getAnimSet(), bIs3PV);

	return FALSE;
}

BOOL CGameCharaBase::_ANIM_playBody( CHARA_ACTION_BODY state, BOOL bIs3PV)
{
	if( getActionContext()->isBody( state))	return FALSE;

	getActionContext()->setBody( state);

	ChangeAnimSet( getAnimSet(), TRUE, bIs3PV);

	return FALSE;
}

BOOL CGameCharaBase::ANIM_playBody( CHARA_ACTION_BODY state, BOOL bIs3PV)
{
	return _ANIM_playBody( state, bIs3PV );
}

BOOL CGameCharaBase::_ANIM_playBody2( CHARA_ACTION_BODY2 state, BOOL bIs3PV)
{
	if( getActionContext()->isBody2( state))	return FALSE;

	//	일단 AttackIdle로 고정
	if( state == CHARA_BODY_IDLE)	state = CHARA_BODY_ATTACKIDLE;

	getActionContext()->setBody2( state);

	//ACTION_apply( bIs3PV);
	getAnimContext()->getControl()->AnimSetApply();

	if( getActionContext()->isUpper( CHARA_UPPER_IDLE) || getActionContext()->isUpper( CHARA_UPPER_MOVE) || 
		getActionContext()->isUpper( CHARA_UPPER_JUMP) || getActionContext()->isUpper( CHARA_UPPER_FIRE) ||
		getActionContext()->isUpper( CHARA_UPPER_SECONDARY_FIRE) || getActionContext()->isUpper(CHARA_UPPER_DAMAGE) )
	{
		switch( getActionContext()->getBody2())
		{
		case CHARA_BODY_ATTACKIDLE:
			{	//	애니메이션 타임 싱크가 필요없는 경우
				if( !getActionContext()->isLower( CHARA_LOWER_IDLE) && !getActionContext()->isLower( CHARA_LOWER_MOVE) && 
					!getActionContext()->isLower( CHARA_LOWER_WALK) && !getActionContext()->isLower( CHARA_LOWER_JUMP))
					return TRUE;
			}
			//	아래 동작도 합니다. No break!!
		case CHARA_BODY_ZOOM:
		case CHARA_BODY_IDLE:
			{
				//	애니메이션을 계속 합니다.
				{
					REAL32 fSyncTime = 0.0f;

					//	하체 애니메이션과 맞는 타임 싱크해야 합니다.
					if( m_pAnimContext->isAILower())
					{
						fSyncTime = GetSyncTimeLower();
					}
					else
					{	//	상체에 타임 싱크하기 위해
						fSyncTime = GetSyncTimeUpper();
					}

					//	애니메이션 타임 싱크해야 합니다.			
					ANIM_playLower( getActionContext()->getLower(), (ID_LOWER_AI)m_pAnimContext->getOldLowerID(), fSyncTime);	
					ANIM_playUpper( getActionContext()->getUpper(), (ID_UPPER_AI)m_pAnimContext->getOldUpperID(), fSyncTime);				
				}
			}
			break;	
		}
	}
	
	return TRUE;
}

BOOL CGameCharaBase::ANIM_playBody2( CHARA_ACTION_BODY2 state, BOOL bIs3PV )
{
	return _ANIM_playBody2( state, bIs3PV );
}

void CGameCharaBase::_ANIM_Debug( i3AIContext * pCtx, REAL32 timeStart)
{	//	i3AIContext가 NULL일 경우도 있다. 스킵하도록
	if( pCtx == NULL)		return;

	i3AIState * pAIState = NULL;
	char * pszAIName = "(null)";

	//	아직 Anim이 현 프레임에 적용되지 않았다.
	if( pCtx->getNextAIStateIndex() != -1)
		pAIState = pCtx->getNextAIState();
	else
		pAIState = pCtx->getCurrentAIState();
	
	if( pAIState != NULL)
	{
		if( pAIState->getAnim())	
			pszAIName = (char *) pAIState->getAnim()->GetName();
		
		I3TRACE("- AI (%s) Anim (%s) (start %.4f, scale %.2f)\n", pAIState->GetName(), pszAIName, timeStart, pAIState->getTimeScale());
	}
	else
	{
		I3TRACE("- AI (null)\n");
	}
}

void CGameCharaBase::_ANIM_stop( void)
{
	GetSceneObject()->GetBody()->StopAnim();
}

BOOL CGameCharaBase::_ANIM_playLower( CHARA_ACTION_LOWER state, ID_LOWER_AI id, REAL32 timeStart, REAL32 timeScale)
{
	if( getActionContext()->isLower( CHARA_LOWER_DEATH))	return FALSE;

	//	상태가 허가되면 하채 애니메이션을 한다.
	getActionContext()->setLower( state);

	//	Combined  Anim은 따로 상체 동작을 없앤다.
	if( state == CHARA_LOWER_DEATH)
		m_pAnimContext->setOldLowerID( ID_LOWER_UNKNOWN);
		
	if( m_pAnimContext->isAILower() && (id > ID_LOWER_UNKNOWN) && (id < ID_LOWER_AI_MAX))
	{
		//	현재 동작이 무엇인지 저장한다.
		m_pAnimContext->setOldLowerID( id);

		INT32 nAIIdx = m_pAnimContext->getAILowerID( getActionContext()->getBody(), id);
		I3ASSERT( nAIIdx != -1);

		m_pAnimContext->getAILower()->setCurrentAIState( nAIIdx, timeStart, timeScale);		//	하체 AI 상태 변경

		return TRUE;
	}	

	return FALSE;
}

BOOL CGameCharaBase::ANIM_playLower( CHARA_ACTION_LOWER state, ID_LOWER_AI id, REAL32 timeStart, REAL32 timeScale )
{
#if defined( ENABLE_ANIM_OUTPUT )
	BOOL bRv = _ANIM_playLower(state, id, timeStart, timeScale );
	if( bRv )
	{
		if(m_pAnimContext->getAILower() != NULL)
		{
			TRACE_FUNC;
			_ANIM_Debug( m_pAnimContext->getAILower(), timeStart );
		}
		
	}
	return bRv;
#else
	return _ANIM_playLower(state, id, timeStart, timeScale );
#endif
}

BOOL CGameCharaBase::_ANIM_playUpper( CHARA_ACTION_UPPER state, ID_UPPER_AI id, REAL32 timeStart, REAL32 timeScale)
{
	if( getActionContext()->isLower( CHARA_LOWER_DEATH))	return FALSE;
	
	if( m_pAnimContext->isAIUpper() && (id > ID_UPPER_UNKNOWN) && (id < ID_UPPER_AI_MAX))
	{
		CWeaponInfo * pWeaponInfo = NULL;
		WeaponBase * pWeapon = getWeaponContext()->getCurrent();
		if( pWeapon != NULL )
		{
			pWeaponInfo = pWeapon->getWeaponInfo();
		}

		//	무기 교환은 예외로 현재는 아직 바뀌지 않은 상태이며 예약된 상태이다.
		//	다음 프레임에 무기 노드가 바뀌도록 되어 있으므로 캐릭터는 예약된 무기로 인식해야 한다.	komet
		if( getWeaponContext()->getSwapSlotNum() != getWeaponContext()->getCurrentSlotNum())	
		{
			WeaponBase * pWeapon = getWeaponContext()->getWeapon( getWeaponContext()->getSwapSlotNum());		
			if( pWeapon == NULL )
				return FALSE;

			pWeaponInfo = pWeapon->getWeaponInfo();
		}

		if( pWeaponInfo == NULL )
			return FALSE;

		INT32 nAIIdx = m_pAnimContext->getAIUpperID( getAnimSet(), pWeaponInfo->m_TypeClass, pWeaponInfo->m_nNumber, getActionContext()->getBody2(), id);
		if( nAIIdx == -1)
		{
			I3TRACE( "Chara Animset no animation.\n" );
			return FALSE;
		}

		//	상태가 허가되면 상체 애니메이션을 한다.
		//	주의! Combined 애니메이션은 하체에 속한다.
		getActionContext()->setUpper( state);

		//	(현재 동작이 무엇인지 저장한다.)
		m_pAnimContext->setOldUpperID( id);

		m_pAnimContext->getAIUpper()->setCurrentAIState( nAIIdx, timeStart, timeScale);		//	상체 AI 상태 변경
		
		return TRUE;
	}

	return FALSE;
}

BOOL CGameCharaBase::ANIM_playUpper( CHARA_ACTION_UPPER state, ID_UPPER_AI id, REAL32 timeStart, REAL32 timeScale )
{
#if defined( ENABLE_ANIM_OUTPUT )
	BOOL bRv = _ANIM_playUpper( state, id, timeStart, timeScale );
	if( bRv )
	{
		if(m_pAnimContext->getAIUpper() != NULL)
		{
			TRACE_FUNC;
			_ANIM_Debug( m_pAnimContext->getAIUpper(), timeStart);
		}
	}
	return bRv;
#else
	return _ANIM_playUpper( state, id, timeStart, timeScale );
#endif
}


BOOL CGameCharaBase::_ANIM_playUpperReturn( BOOL bIsNetwork)
{	//	이전에 동작중인 Upper 애니메이션으로 돌아간다.

	if( getActionContext()->isBody2( CHARA_BODY_IDLE) )
		_ANIM_playBody2( CHARA_BODY_ATTACKIDLE, bIsNetwork);

	//	하체 애니메이션과 맞는 타임 싱크해야 합니다.
	REAL32 fSyncTime = GetSyncTimeLower();
	
	if( getActionContext()->isLower( CHARA_LOWER_MOVE) )//|| getActionContext()->isLower( CHARA_LOWER_WALK) )
	{
		ANIM_playUpper( CHARA_UPPER_MOVE, ID_UPPER_MOVE, fSyncTime);				//	이전 Move 동작으로
	}
	else
	{		
		ANIM_playUpper( CHARA_UPPER_IDLE, ID_UPPER_IDLE, fSyncTime);		//	이전 Idle 동작으로
	}
	
	return TRUE;
}

BOOL CGameCharaBase::ANIM_playUpperReturn( BOOL bIs3PV )
{
	TRACE_FUNC;
	return _ANIM_playUpperReturn( bIs3PV );
}

REAL32 CGameCharaBase::_ANIM_getUpperDuration(void)
{
	if( !m_pAnimContext->isAIUpper())	return 0.0f;
	i3AIState * pAIState = m_pAnimContext->getAIUpper()->getCurrentAIState();
	return (pAIState != NULL ? pAIState->getDuration() : 0.0f);								
}

REAL32 CGameCharaBase::_ANIM_getUpperLocalTime( void)
{
	if( !m_pAnimContext->isAIUpper())	return 0.0f;
	return m_pAnimContext->getAIUpper()->getLocalTime();
}

REAL32 CGameCharaBase::_ANIM_getLowerDuration( void)		
{	
	if( !m_pAnimContext->isAILower())	return 0.0f;

	i3AIState * pAIState = m_pAnimContext->getAILower()->getCurrentAIState();
	return (pAIState != NULL ? pAIState->getDuration() : 0.0f);
}

REAL32 CGameCharaBase::_ANIM_getLowerLocalTime( void)
{
	if( !m_pAnimContext->isAILower())	return 0.0f;
	return m_pAnimContext->getAILower()->getLocalTime();
}*/

