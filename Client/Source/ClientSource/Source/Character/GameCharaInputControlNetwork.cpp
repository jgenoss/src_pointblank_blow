#include "pch.h"
#include "GameCharaInputControlNetwork.h"
#include "GlobalVariables.h"
#include "GameCharaContexts.h"
#include "GameObject.h"
#include "StageBattle.h"
#include "GameStateMgr.h"

#include "./StageBattle/UserContext.h"
#include "BattleServerContext.h"
#include "MyRoomInfoContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"

#define NET_ASSIST_MINTIME			0.5f			// 최소 보정 타임 이 시간을 초과하면 보정하지 않는다.
#define NET_ASSIST_MAXTIME			0.1f			//0.1f			//0.16f			// 이 시간 단위로 위치 이동이 이루어진다.
#define NET_ASSIST_MAXTIMEINV		10.f			//10.f			//6.25f
#define NET_ASSIST_ROTATETIME		0.1f
#define NET_ASSIST_ROTATETIMEINV	10.f

#define NET_DIRECTION_ASSIST_VALUE	0.3826834323650897717284599840304f		// 0.7142857142857142857142857142857f
#define NET_ASSIST_MOVE_MINLENGTH	0.01f	//I3_EPS

I3_CLASS_INSTANCE( CGameCharaInputControlNetwork);

void CGameCharaInputControlNetwork::_ProcessMoveByKey( UINT8 key, VEC3D * pPos)
{
	CGameCharaActionContext * pActionCtx = getActionContext();
	CHARA_ACTION_LOWER lowerAction = pActionCtx->getLower();

	if( (lowerAction == CHARA_LOWER_RUN) || (lowerAction == CHARA_LOWER_WALK) )
	{
		VEC3D vTemp;
		
		bool bCheckKey = false;
		REAL32 rSpeed = 1.f;
		VEC3D * pMoveDir = getMoveContext()->getMoveDir();
		VEC3D * pAccel = getMoveContext()->getAccel();

		if( (key & GAME_KEY_MOVE_LEFT) && (getX( pMoveDir ) == 1.f) )
		{
			rSpeed = m_pOwner->GetMoveSpeedAddOffset(MST_RUN, MDT_SIDE) * g_pViewer->getDeltaTime();
			addX( pAccel, rSpeed * 0.5f);
			if( getX( pAccel) > rSpeed * NET_ASSIST_MAXTIME)
				setX( pAccel, rSpeed * NET_ASSIST_MAXTIME);
			bCheckKey = true;
		}
		else if( key & GAME_KEY_MOVE_RIGHT && (getX( pMoveDir ) == -1.f))
		{
			rSpeed = -m_pOwner->GetMoveSpeedAddOffset(MST_RUN, MDT_SIDE) * g_pViewer->getDeltaTime();
			addX( pAccel, rSpeed * 0.5f);
			if( getX( pAccel) < rSpeed * NET_ASSIST_MAXTIME)
				setX( pAccel, rSpeed * NET_ASSIST_MAXTIME);
			bCheckKey = true;
		}
		else
			setX( pAccel, 0.f);

		if( key & GAME_KEY_MOVE_FORWARD && (getZ( pMoveDir ) == -1.f))
		{
			rSpeed = m_pOwner->GetMoveSpeedAddOffset(MST_RUN, MDT_FRONT) * g_pViewer->getDeltaTime();
			addZ( pAccel, rSpeed * 0.5f);
			if( getZ( pAccel) > rSpeed * NET_ASSIST_MAXTIME)
				setZ( pAccel, rSpeed * NET_ASSIST_MAXTIME);
			bCheckKey = true;
		}
		else if( key & GAME_KEY_MOVE_BACKWARD && (getZ( pMoveDir ) == 1.f))
		{
			rSpeed = -m_pOwner->GetMoveSpeedAddOffset(MST_RUN, MDT_BACK) * g_pViewer->getDeltaTime();
			addZ( pAccel, rSpeed * 0.5f );
			if( getZ( pAccel) < rSpeed * NET_ASSIST_MAXTIME)
				setZ( pAccel, rSpeed * NET_ASSIST_MAXTIME);
			bCheckKey = true;
		}
		else
			setZ( pAccel, 0.f);

		if( bCheckKey )
		{
			MATRIX mat;
			i3Matrix::SetRotateY( &mat, getBoneContext()->getTheta() );
			i3Vector::TransformCoord( &vTemp, pAccel, &mat );
			i3Vector::Add( pPos, pPos, &vTemp );
		}
	}
}

void CGameCharaInputControlNetwork::_InterpolateThetaRho(void)
{
	REAL32	flerp = getMoveContext()->getRecvThetaRhoTime();

	flerp += g_pViewer->getDeltaTime();
	getMoveContext()->setRecvThetaRhoTime( flerp);

	if( flerp > NET_ASSIST_ROTATETIME )
	{
		getMoveContext()->setPostTheta( getMoveContext()->getRecvTheta());
		getMoveContext()->setPostRho( getMoveContext()->getRecvRho());
		flerp = NET_ASSIST_ROTATETIME;
	}
	flerp *= NET_ASSIST_ROTATETIMEINV;

	//	Theta
	getBoneContext()->setTheta( getMoveContext()->getPostTheta() + ((getMoveContext()->getRecvTheta() - getMoveContext()->getPostTheta()) * flerp));
	//	Rho
	getBoneContext()->setRho( getMoveContext()->getPostRho() + ((getMoveContext()->getRecvRho() - getMoveContext()->getPostRho()) * flerp));
}

void CGameCharaInputControlNetwork::_InterpolatePosition( VEC3D * pPos)
{
	REAL32	flerp = getMoveContext()->getRecvPosTime();
	flerp += g_pViewer->getDeltaTime();
	getMoveContext()->setRecvPosTime( flerp);

	if( flerp > NET_ASSIST_MAXTIME )
		flerp = NET_ASSIST_MAXTIME;
	flerp *= NET_ASSIST_MAXTIMEINV;
	//i3Vector::Lerp( pPos, &m_pOwner->getCharaAssistInfo()->_vPostPosition, &m_pOwner->getCharaAssistInfo()->_vRecvPosition, flerp );
	i3Vector::Lerp( pPos, getMoveContext()->getPostPosition(), getMoveContext()->getRecvPosition(), flerp );
}

///////////////////////////////////////////////////////////////////////////////////////////
//
//	게임 프레임 업데이트 상의 업데이트 입니다.
//  네트워크로 최종적으로 받은 상태에 대해서만 처리합니다.
void CGameCharaInputControlNetwork::OnFirstUpdate( REAL32 rDeltatime)
{
	// 네트워크 입력
	VEC3D	vLerp;

	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) ) return;
	ISNETWORKRETURN(false)
	if( !BattleServerContext::i()->IsP2PActivate() )			return;
	if( m_pOwner->getCharaNetIndex() == -1 )		return;

	if( !g_pFramework->IsBattleStage())				return;
	
	// 게임 종료거나 라운드가 종료되면 처리하지 않습니다.
	if( (gstatemgr::i()->getStageState() != GAMESTAGE_STATE_BATTLE) )
		return;

	GAMEINFO_CHARACTER *	pGameInfo = BattleSlotContext::i()->setGameInfo_Chara( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());
	I3ASSERT( pGameInfo != nullptr);

	m_pOwner->removeCharaStateMask( CHARA_STATE_CHANGEDIR);
	pGameInfo->_bRecvFlag = false;
	
	if( pGameInfo->_tNetData._tTakingObject.getOnLoadObject() )
	{	// 애니메이션하는 오브젝트만 검사합니다.
		if( pGameInfo->_tNetData._tTakingObject.getOnLoadObjectIndex() < OBJ_TOTAL_COUNT)
		{
			const GAMEINFO_OBJECT * pObjInfo = BattleObjContext::i()->getGameInfo_Object( pGameInfo->_tNetData._tTakingObject.getOnLoadObjectIndex() );
			if( pObjInfo->_pObjectRef != nullptr)
			{
				VEC3D vObjectMove;
				MATRIX matObject;
				i3Object * pObj = const_cast<i3Object*>(pObjInfo->_pObjectRef);

				getMoveContext()->SetRideObject( pObj);
				getMoveContext()->UpdateRideObject( &matObject, rDeltatime, false);
				VEC3D * pNewPos = getMoveContext()->TransformByRideObject( &matObject, m_pOwner->GetPos(), false);

				// 이동량을 구한다.
				i3Vector::Sub( &vObjectMove, pNewPos, m_pOwner->GetPos());

				// 속도 계산
			//	i3Vector::Sub( getMoveContext()->getVelocity(), getMoveContext()->getPostPosition(), getMoveContext()->getRecvPosition());
			//	i3Vector::Scale( getMoveContext()->getVelocity(), getMoveContext()->getVelocity(), 0.16f);

				VEC3D vTemp;
				i3Vector::Add( &vTemp, getMoveContext()->getRecvPosition(), &vObjectMove);
				getMoveContext()->setRecvPosition( &vTemp);
				i3Vector::Add( &vTemp, getMoveContext()->getPostPosition(), &vObjectMove);
				getMoveContext()->setPostPosition( &vTemp);
				
				m_pOwner->AddOnObjectState();
				
			}
		}
	}
	else
	{
		getMoveContext()->ResetRideObject();
	}
	
	// 캐릭터의 Theta, Rho를 계산합니다.
	_InterpolateThetaRho();
	// 캐릭터 위치 설정(인터폴레이션합니다.)
	_InterpolatePosition( &vLerp);
	// 키값을 바탕으로 캐릭터를 약간 더 움직여줍니다
	// Dedi에서 Move키는 버리므로 처리 하지 않습니다...
	//_ProcessMoveByKey( pGameInfo->_tNetData._tActionKey.getMoveKey(0), &vLerp);
	// 캐릭터 스피드 계산(하체 애니메이션에 사용됩니다)
//	if( !pGameInfo->_tNetData._tTakingObject.getOnLoadObject() )
		i3Vector::Sub( getMoveContext()->getVelocity(), &vLerp, m_pOwner->GetPos() );
	i3Vector::Copy( getMoveContext()->getOldVelocity(), getMoveContext()->getVelocity() );
	getMoveContext()->setReceivedLength( i3Vector::Length( getMoveContext()->getVelocity() ));
	
	// Set Position
	m_pOwner->forceSetPos( &vLerp );
	
	m_rReloadElapesedTime += rDeltatime;
	m_rReloadReadyElapesedTime += rDeltatime;
	m_rLoadBulletElapesedTime += rDeltatime;

	// 자동으로 처리하는 액션입니다.
	_ProcessAutoActionUpper();
	ProcessNetworkActionLower( (CHARA_ACTION_LOWER) pGameInfo->_tNetData._tCharaState.getLower(), getMoveContext()->getReceivedLength());
}

void CGameCharaInputControlNetwork::OnUpdate( REAL32 rDeltatime)
{
	CGameCharaInputControl::OnUpdate( rDeltatime);
	// Add Code here
}

GAME_CHARA_NETWORK_MOVE_TYPE CGameCharaInputControlNetwork::_ProcessRecv_MoveDir( GAMEINFO_CHARA_NETDATA * pRecvInfo )
{	
	bool	bCheckMove = false;
	bool	bCheckXAxisMove = false;

	WeaponBase * pWeapon = nullptr;
	GAME_CHARA_NETWORK_MOVE_TYPE	nCheckMove = GAME_CHARA_NETWORK_STAND;

	if( m_pOwner->getCharaNetIndex() == -1 )									return nCheckMove;
	if( m_pOwner->getCharaInfo()->GetAIToClient() == false)
	{
		if( UserContext::i()->Death[m_pOwner->getCharaNetIndex()])
			return nCheckMove;
	}
	
	m_pOwner->removeCharaStateMask( CHARA_STATE_CHANGEDIR );

	
	const GAMEINFO_CHARACTER * pRecvGameInfo = BattleSlotContext::i()->getGameInfo_Chara( m_pOwner->getCharaNetIndex(), m_pOwner->getBufferType());

	if( pRecvGameInfo->_tNetData._tTakingObject.getOnLoadObject() )
		_ProcessRecv_PositionOnLoadObject( pRecvInfo);
	else
		_ProcessRecv_Position( pRecvInfo);
	
	i3Vector::Copy( getMoveContext()->getVelocity(), getMoveContext()->getRecvDirection() );
	if( i3Vector::LengthSq( getMoveContext()->getRecvDirection()) > I3_EPS)
	{
		REAL32 length = i3Vector::Normalize( getMoveContext()->getRecvDirection(), getMoveContext()->getRecvDirection());

		length *= NET_ASSIST_MAXTIMEINV;
		getMoveContext()->setReceivedLength( length );
	}
	else
		getMoveContext()->setReceivedLength( 0.f);

	if( getMoveContext()->getReceivedLength() > 50.f)
	{	// 순간 이동 시켜줍니다.
		getMoveContext()->setPostPosition( getMoveContext()->getRecvPosition() );

		getMoveContext()->setReceivedLength( 0.f);
		m_pOwner->forceSetPos( getMoveContext()->getPostPosition() );
		return nCheckMove;
	}
	else if( getMoveContext()->getReceivedLength() > NET_ASSIST_MOVE_MINLENGTH )
	{	// 모션 및 방향을 결정합니다.
		MATRIX	mat;
		
		i3Matrix::SetRotateY( &mat, -getMoveContext()->getRecvTheta() );
		i3Vector::TransformNormal( getMoveContext()->getPostDirection(), getMoveContext()->getRecvDirection(), &mat );

		REAL32	rDestVel = 0.f;

		if( (getZ( getMoveContext()->getPostDirection() ) > NET_DIRECTION_ASSIST_VALUE))
		{
			if(getZ( getMoveContext()->getMoveDir() ) != 1.f)	m_pOwner->addCharaStateMask( CHARA_STATE_CHANGEDIR);
			i3Vector::SetZ( getMoveContext()->getMoveDir(), 1.f );
			rDestVel = m_pOwner->GetMoveSpeedAddOffset(MST_WALK, MDT_FRONT);
			bCheckMove =true;
		}
		else if( (getZ( getMoveContext()->getPostDirection() ) < -NET_DIRECTION_ASSIST_VALUE))
		{
			if(getZ( getMoveContext()->getMoveDir()) != -1.f)	m_pOwner->addCharaStateMask( CHARA_STATE_CHANGEDIR);
			i3Vector::SetZ( getMoveContext()->getMoveDir(), -1.f );
			rDestVel = m_pOwner->GetMoveSpeedAddOffset(MST_WALK, MDT_BACK);
			bCheckMove = true;
		}
		else
		{
			if( getZ( getMoveContext()->getMoveDir()) != 0.f )	m_pOwner->addCharaStateMask( CHARA_STATE_CHANGEDIR);
			i3Vector::SetZ( getMoveContext()->getMoveDir(), 0.f );
		}

		if( (getX( getMoveContext()->getPostDirection() ) > NET_DIRECTION_ASSIST_VALUE))
		{
			if(getX( getMoveContext()->getMoveDir()) != 1.f)	m_pOwner->addCharaStateMask( CHARA_STATE_CHANGEDIR);
			i3Vector::SetX( getMoveContext()->getMoveDir(), 1.f );
			if( bCheckMove )	rDestVel = m_pOwner->GetMoveSpeedAddOffset(MST_WALK, MDT_DIAGONAL);
			else				rDestVel = m_pOwner->GetMoveSpeedAddOffset(MST_WALK, MDT_SIDE);
			bCheckMove = true;
			bCheckXAxisMove = true;
		}
		else if( (getX( getMoveContext()->getPostDirection() ) < -NET_DIRECTION_ASSIST_VALUE))
		{
			if(getX( getMoveContext()->getMoveDir()) != -1.f)	m_pOwner->addCharaStateMask( CHARA_STATE_CHANGEDIR);
			i3Vector::SetX( getMoveContext()->getMoveDir(), -1.f );
			if( bCheckMove )	rDestVel = m_pOwner->GetMoveSpeedAddOffset(MST_WALK, MDT_DIAGONAL);
			else				rDestVel = m_pOwner->GetMoveSpeedAddOffset(MST_WALK, MDT_SIDE);
			bCheckMove = true;
			bCheckXAxisMove = true;
		}
		else
		{
			if( getX( getMoveContext()->getMoveDir()) != 0.f )	m_pOwner->addCharaStateMask( CHARA_STATE_CHANGEDIR);
			i3Vector::SetX( getMoveContext()->getMoveDir(), 0.f );
		}
		
//		if (m_pOwner->isCharaStateMask(CHARA_STATE_JUMP) == false)
//		{
			if ((getY(getMoveContext()->getPostDirection()) > NET_DIRECTION_ASSIST_VALUE))
			{
				if (getY(getMoveContext()->getMoveDir()) != 1.f)	m_pOwner->addCharaStateMask(CHARA_STATE_CHANGEDIR);
				i3Vector::SetY(getMoveContext()->getMoveDir(), 1.f);
				if (bCheckXAxisMove)	rDestVel = m_pOwner->GetMoveSpeedAddOffset(MST_WALK, MDT_DIAGONAL);
				else				rDestVel = m_pOwner->GetMoveSpeedAddOffset(MST_WALK, MDT_SIDE);
				bCheckMove = true;
			}
			else if ((getY(getMoveContext()->getPostDirection()) < -NET_DIRECTION_ASSIST_VALUE))
			{
				if (getY(getMoveContext()->getMoveDir()) != -1.f)	m_pOwner->addCharaStateMask(CHARA_STATE_CHANGEDIR);
				i3Vector::SetY(getMoveContext()->getMoveDir(), -1.f);
				if (bCheckXAxisMove)	rDestVel = m_pOwner->GetMoveSpeedAddOffset(MST_WALK, MDT_DIAGONAL);
				else				rDestVel = m_pOwner->GetMoveSpeedAddOffset(MST_WALK, MDT_SIDE);
				bCheckMove = true;
			}
			else
			{
				if (getY(getMoveContext()->getMoveDir()) != 0.f)	m_pOwner->addCharaStateMask(CHARA_STATE_CHANGEDIR);
				i3Vector::SetY(getMoveContext()->getMoveDir(), 0.f);
			}
//		}

		if(m_pOwner->IsDino() == false)
		{
			pWeapon = m_pOwner->GetCurrentCharaWeapon();
		}
		else
		{
			pWeapon = nullptr;
		}

		if( bCheckMove )
		{
			if( pWeapon != nullptr)
			{
				if( getActionContext()->isBody( CHARA_BODYLOWER_STAND ))		rDestVel *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_NORMAL) * 0.01f;
				else if( getActionContext()->isBody( CHARA_BODYLOWER_CROUCH))	rDestVel *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_SIT) * 0.01f;

				if( m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM))
				{
					//	줌인 중이지만 장전시에는 적용안되게...
					if( !getActionContext()->isUpper( CHARA_UPPER_LOADMAGAZINE) && !getActionContext()->isUpper( CHARA_UPPER_LOADBULLET) &&
						!getActionContext()->isUpper( CHARA_UPPER_FIREBULLET))	
						rDestVel *= pWeapon->GetMoveSpeed( WEAPON_MOVE_SPEED_ZOOM) * 0.01f;
				}
			}
			
			switch( pRecvGameInfo->_tNetData._tCharaState.getLower())
			{
			case CHARA_LOWER_WALK:	nCheckMove = GAME_CHARA_NETWORK_WALK;		break;
			case CHARA_LOWER_RUN :	nCheckMove = GAME_CHARA_NETWORK_RUN;		break;
			default :	if( getMoveContext()->getReceivedLength() > I3_EPS)	nCheckMove = GAME_CHARA_NETWORK_WALK;	break;
			}
		}
	}
	else
	{
		i3Vector::Zero( getMoveContext()->getMoveDir() );
		nCheckMove = GAME_CHARA_NETWORK_STAND;
	}

	return nCheckMove;
}

// 앉아/일어서
void CGameCharaInputControlNetwork::ProcessNetworkActionBody( CHARA_ACTION_BODYLOWER body)
{
	CGameCharaActionContext * pActionCtx = getActionContext();

	switch( body )
	{
	case CHARA_BODYLOWER_STAND :	m_pOwner->Cmd_StandUp();		break;
	case CHARA_BODYLOWER_CROUCH :
		if( !pActionCtx->isBody( CHARA_BODYLOWER_CROUCH ) )
		{
			if( getAnimContext()->getOldLowerID() == ID_LOWER_JUMP_END )
			{
				m_pOwner->Cmd_Return_Upper();
				m_pOwner->Cmd_Return_Lower();
			}
			m_pOwner->Cmd_Crouch();
		}
		break;
	default :
		I3TRACE( "CGameCharaInputControlNetwork::ProcessNetworkActionBody() - invalid action. %d\n", body );
		break;
	}
}

void CGameCharaInputControlNetwork::ProcessNetworkActionBody2( CHARA_ACTION_BODYUPPER body2)
{
	CGameCharaActionContext * pActionCtx = getActionContext();
	// Zoom

	switch( body2 )
	{
	case CHARA_BODYUPPER_NONE :		break;
	/*case CHARA_BODYUPPER_IDLE :
		if( !pActionCtx->isBody2( CHARA_BODYUPPER_IDLE ) )
			m_pOwner->Cmd_ToggleBodyState();
		break;*/
	case CHARA_BODYUPPER_ATTACKIDLE :
		{
			if( !pActionCtx->isBody2( CHARA_BODYUPPER_ATTACKIDLE) ||
				m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM) )
			{
				// revision 35022	DSR 익스텐션재장전 모션 버그 수정
				WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
				if( pWeapon == nullptr) return;
				if( i3::same_of<WeaponSniperDualMagazine*>( pWeapon ) )
					m_pOwner->Cmd_ToggleZoomForDSR();
				else
					m_pOwner->Cmd_Extension( false);
			}
		}
		break;
	case CHARA_BODYUPPER_ZOOM :
		if( !pActionCtx->isBody2( CHARA_BODYUPPER_ZOOM) )
		{	
			if( m_pOwner->IsDino()) return;

			WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
			if( pWeapon == nullptr) return;

			if(i3::same_of<WeaponSniperDualMagazine*>(pWeapon))
			{
				m_pOwner->Cmd_ToggleZoomForDSR();
			}
			else if(i3::same_of<WeaponGunWithKnife*>(pWeapon))
			{
				if( pWeapon->GetCurrentExtensionType() != WEAPON::EXT_ZOOM && 
					pWeapon->GetCurrentExtensionType() != WEAPON::EXT_SCOPE)
					pWeapon->SwapExtensionType();

				m_pOwner->Cmd_Extension( true);
			}
			else
				m_pOwner->Cmd_Extension( true);
		}
		break;
	case CHARA_BODYUPPER_GRENADE :
		if( !pActionCtx->isBody2( CHARA_BODYUPPER_GRENADE) || pActionCtx->isBody2( CHARA_BODYUPPER_ZOOM) )
		{
			//3PV 경우 GRENADE -> ATTACKIDLE
			if( !pActionCtx->isBody2( CHARA_BODYUPPER_ATTACKIDLE) || 
				m_pOwner->isCharaStateMask( CHARA_STATE_ZOOM) )
				m_pOwner->Cmd_Extension( false);
		}
		break;
	case CHARA_BODYUPPER_SHOTGUN :
		if( !pActionCtx->isBody2( CHARA_BODYUPPER_SHOTGUN) )
			m_pOwner->Cmd_Extension( true);
			break;
	case CHARA_BODYUPPER_KNIFE :
		//if( !pActionCtx->isBody2( CHARA_BODYUPPER_KNIFE) )
		//	m_pOwner->Cmd_Extension( true);
		break;

	case CHARA_BODYUPPER_CROSSFIRE :
		if( !pActionCtx->isBody2( CHARA_BODYUPPER_CROSSFIRE) )
		{
			m_pOwner->Cmd_Extension( true);
		}
		break;

	default :
		I3TRACE( "CGameCharaInputControlNetwork::ProcessNetworkActionBody2() - invalid action. %d\n", body2 );
		break;
	}
}

void CGameCharaInputControlNetwork::ProcessNetworkActionUpper( CHARA_ACTION_UPPER upper)
{
	if(m_pOwner->IsDino())
	{
		ProcessNetworkActionUpperDino(upper);
	}
	else
	{
		ProcessNetworkActionUpperHuman(upper);
	}
}

void CGameCharaInputControlNetwork::ProcessNetworkActionUpperDino( CHARA_ACTION_UPPER upper)
{
	CGameCharaActionContext * pActionCtx = getActionContext();
	if( pActionCtx->isLower( CHARA_LOWER_STUN ) )
		return;

	switch( upper )
	{
		case CHARA_UPPER_MOVE :
			m_pOwner->getActionContext()->setUpper( CHARA_UPPER_MOVE);
			break;
	}
}

void CGameCharaInputControlNetwork::ProcessNetworkActionUpperHuman( CHARA_ACTION_UPPER upper)
{
	WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
	if( pWeapon == nullptr) return;

	CHARA_ACTION_UPPER nActionUpper = getActionContext()->getUpper();

	switch( upper )
	{
	case CHARA_UPPER_NONE :
	case CHARA_UPPER_IDLE :
		{
			if( pWeapon->getWeaponInfo()->GetTypeFire() == WEAPON::FIRE_THROWING)
			{
				if( pWeapon->getWeaponState() != WEAPON::STATE_READY)
					pWeapon->ReturnToReady();
			}
		}
		break;
	case CHARA_UPPER_MOVE :
	case CHARA_UPPER_FIRE :					break;
	case CHARA_UPPER_FIRE_READY :
		{
			WEAPON_CLASS_TYPE weaponClass = pWeapon->getWeaponInfo()->GetTypeClass();
			if( pWeapon->getWeaponInfo()->GetTypeFire() == WEAPON::FIRE_THROWING
				|| (weaponClass ==WEAPON_CLASS_BOW || weaponClass == WEAPON_SUBCLASS_BOW) )
			{
				if( (nActionUpper != CHARA_UPPER_DAMAGE) && (nActionUpper != CHARA_UPPER_FIRE_READY) )
					m_pOwner->Cmd_Attack( WEAPON::READY_FIRE);
			}
		}
		break;
	case CHARA_UPPER_SECONDARY_FIRE :		break;
	case CHARA_UPPER_LOADMAGAZINE :
		if( (nActionUpper != CHARA_UPPER_DAMAGE) && (nActionUpper != CHARA_UPPER_LOADMAGAZINE) &&
			(nActionUpper != CHARA_UPPER_SWAPWEAPON))
		{
			if( m_rReloadElapesedTime > m_rReloadTime)
			{				
				m_pOwner->Cmd_Reload();
				m_rReloadTime = pWeapon->GetReloadTime() + 0.1f;
				m_rReloadElapesedTime = 0.f;
			}
		}
		break;
	case CHARA_UPPER_LOADMAG_READY:
		if( (nActionUpper != CHARA_UPPER_DAMAGE) && (nActionUpper != CHARA_UPPER_LOADMAG_READY) &&
			(nActionUpper != CHARA_UPPER_SWAPWEAPON))
		{
			if( m_rReloadReadyElapesedTime > m_rReloadReadyTime)
			{
				m_pOwner->Cmd_Reload(ACTION_LOAD_READY_MAG);
				m_rReloadReadyTime = getAnimContext()->getUpperDuration();
				m_rReloadReadyElapesedTime = 0.f;
			}
		}
		break;
	case CHARA_UPPER_LOADBULLET :
		if( (nActionUpper != CHARA_UPPER_DAMAGE) && (nActionUpper != CHARA_UPPER_LOADBULLET) )
		{
			if( m_rLoadBulletElapesedTime > m_rLoadBulletTime )
			{
				m_pOwner->Cmd_Reload( ACTION_LOAD_MAG_BULLET);
				m_rLoadBulletTime = pWeapon->getWeaponInfo()->GetLoadBulletTime() + 0.1f;
				m_rLoadBulletElapesedTime = 0.f;
			}
		}
		break;
	case CHARA_UPPER_FIREBULLET :
		if( (nActionUpper != CHARA_UPPER_DAMAGE) && (nActionUpper != CHARA_UPPER_FIREBULLET) )
		{
			if( m_rLoadBulletElapesedTime > m_rLoadBulletTime )
			{
				m_pOwner->Cmd_Reload( ACTION_LOAD_FIRE_BULLET);
				m_rLoadBulletTime = pWeapon->getWeaponInfo()->GetLoadBulletTime() + 0.1f;
				m_rLoadBulletElapesedTime = 0.f;
			}
		}
		break;

	case CHARA_UPPER_DAMAGE :
	case CHARA_UPPER_SWAPWEAPON :
		break;
	case CHARA_UPPER_JUMP :
		m_pOwner->Cmd_Idle_Upper();
		break;
	case CHARA_UPPER_EXTENSION_ACTIVATE :
	case CHARA_UPPER_EXTENSION_DEACTIVATE :
		if( nActionUpper != CHARA_UPPER_DAMAGE )
		{
			if( nActionUpper == CHARA_UPPER_JUMP )
			{
				m_pOwner->Cmd_JumpEnd();
			}
			else if( (nActionUpper != CHARA_UPPER_IDLE) &&
				(getInputContext()->getMoveType() == GAME_CHARA_NETWORK_STAND) )
			{
				m_pOwner->Cmd_Idle_Upper();
			}
			if( i3::same_of<WeaponGunWithKnife*>(pWeapon))
			{
				if( upper == CHARA_UPPER_EXTENSION_ACTIVATE )
				{
					if( pWeapon->GetCurrentExtensionType() !=  WEAPON::EXT_BAYONET)
					{
						pWeapon->ExtensionActivate(false);
						pWeapon->SwapExtensionType();
					}
					m_pOwner->Cmd_Extension( true);
				}
				else
				{
					if( pWeapon->GetCurrentExtensionType() !=  WEAPON::EXT_BAYONET)
					{
						pWeapon->SwapExtensionType();
					}
					m_pOwner->Cmd_Extension( false);
					//pWeapon->setExtensionActivate(false);
				}
			}
		}
		break;
	
	default :
		I3TRACE( "CGameCharaInputControlNetwork::ProcessNetworkActionUpper() - invalid action. %d\n", upper );
		break;
	}
}

void CGameCharaInputControlNetwork::ProcessNetworkActionLower( CHARA_ACTION_LOWER lower, REAL32 rLength)
{
	CGameCharaActionContext * pActionCtx = getActionContext();
	CGameCharaMoveContext * pMoveCtx = getMoveContext();

	if( pActionCtx->isLower( CHARA_LOWER_STUN ) )
		return;

	switch( lower )
	{
	case CHARA_LOWER_NONE :					break;
	case CHARA_LOWER_JUMP :			// jump
		m_pOwner->Cmd_JumpStart();
		break;

	case CHARA_LOWER_IDLE :
		{
			if( pActionCtx->isLower( CHARA_LOWER_JUMP ) )
			{	// 이전 상태가 점프이면
				m_pOwner->Cmd_JumpEnd();
			}
		}
		break;
	case CHARA_LOWER_RUN :
	case CHARA_LOWER_WALK :
		if( pActionCtx->isLower( CHARA_LOWER_JUMP ) )
		{	// 이전 상태가 점프이면
			m_pOwner->Cmd_JumpEnd();
		}
		break;
	
	case CHARA_LOWER_DEATH :
		break;

	case CHARA_LOWER_ROAR :
		m_pOwner->Cmd_Roar();
		break;
	case CHARA_LOWER_WAIT :
		m_pOwner->Cmd_Dino_Idle();
		break;

	case CHARA_LOWER_STUN :
		m_pOwner->Cmd_Dino_Stun();
		break;

	default :
		I3TRACE( "CGameCharaInputControlNetwork::ProcessNetworkActionLower() - invalid action. %d\n", lower );
		break;
	}

	// 이동 하체
	if( !pActionCtx->isLower( CHARA_LOWER_JUMP ) )
	{	// 점프 중이면 스킵
		switch( getInputContext()->getMoveType() )
		{
		case GAME_CHARA_NETWORK_RUN:
			if( m_pOwner->isCharaStateMask( CHARA_STATE_CHANGEDIR) ||
				(pMoveCtx->getSpeed() != rLength) )
			{	
				if( i3Vector::IsSame( pMoveCtx->getMoveDir(), &I3_ZEROVECTOR) )
				{
					m_pOwner->Cmd_Idle_Lower();
				}
				else
				{
					// 방향이 바뀌거나 이동 속도가 다르면 애니메이션을 다시 합니다.( 내부에서 스케일링 함)
					if(m_pOwner->IsDino())
					{
						// TRex는 달릴 때 상체 애니메이션이 있다.
						m_pOwner->Cmd_Run( pMoveCtx->getMoveDir(), true);
					}
					else
					{
						m_pOwner->Cmd_Run( pMoveCtx->getMoveDir(), false);
					}
					
				}

				getMoveContext()->setSpeed( rLength);
			}
			break;

		case GAME_CHARA_NETWORK_WALK:
			if( m_pOwner->isCharaStateMask( CHARA_STATE_CHANGEDIR) || ( pMoveCtx->getSpeed() != rLength ) )
			{	
				if( i3Vector::IsSame( pMoveCtx->getMoveDir(), &I3_ZEROVECTOR) )
				{	
					m_pOwner->Cmd_Idle_Lower();
				}
				else
				{
					// 방향이 바뀌거나 이동 속도가 다르면 애니메이션을 다시 합니다.( 내부에서 스케일링 함)
					m_pOwner->Cmd_WalkLower( pMoveCtx->getMoveDir());

					getMoveContext()->setSpeed( rLength);
				}
			}
			else if( pMoveCtx->getSpeed() < I3_EPS)
			{
				m_pOwner->Cmd_Idle_Lower();
			}
			break;

		default :
			switch( pActionCtx->getLower() )
			{
			case	CHARA_LOWER_ROAR :
				m_pOwner->Cmd_Roar();
				break;
			case	CHARA_LOWER_WAIT :
				m_pOwner->Cmd_Dino_Idle();
				break;
			case	CHARA_LOWER_STUN :
				m_pOwner->Cmd_Dino_Stun();
				break;
			default :
				m_pOwner->Cmd_Idle_Lower();
				break;
			}
			break;
		}
	}
}

void CGameCharaInputControlNetwork::_ProcessRecv_Position( GAMEINFO_CHARA_NETDATA * pInfo)
{
	I3ASSERT( m_pOwner->getCharaNetIndex() != -1);
	i3Vector::Sub( getMoveContext()->getRecvDirection(), pInfo->_tPosRotData.getPos(), getMoveContext()->getRecvPosition() );
	getMoveContext()->setRecvPosition( pInfo->_tPosRotData.getPos() );
	getMoveContext()->setPostPosition( m_pOwner->GetPos() );
}

void CGameCharaInputControlNetwork::_ProcessRecv_PositionOnLoadObject( GAMEINFO_CHARA_NETDATA * pRecvInfo)
{	
	// local to world matrix
	const GAMEINFO_CHARACTER * pRecvGameInfo = BattleSlotContext::i()->getGameInfo_Chara( m_pOwner->getCharaNetIndex() );

	if( pRecvGameInfo->_tNetData._tTakingObject.getOnLoadObjectIndex() >= OBJ_TOTAL_COUNT)
	{	// Invalid Object Index....
		return;
	}

	const GAMEINFO_OBJECT * pObjInfo = BattleObjContext::i()->getGameInfo_Object( pRecvGameInfo->_tNetData._tTakingObject.getOnLoadObjectIndex() );
	I3ASSERT( pObjInfo->_pObjectRef != nullptr);
		
	i3Object * pObject = const_cast<i3Object*>(pObjInfo->_pObjectRef);
	i3Transform * pTrans = GlobalFunc::FindTransformFromPhysXSceneObj( pObject->getSceneObject() );

	if( pTrans != nullptr)
	{
		VEC3D vTemp;
		i3Vector::TransformCoord( &vTemp, pRecvInfo->_tPosRotData.getPos(), pTrans->GetCacheMatrix() );
		getMoveContext()->setRecvPosition( &vTemp);
		getMoveContext()->setPostPosition( m_pOwner->GetPos() );
		i3Vector::Sub( getMoveContext()->getRecvDirection(), getMoveContext()->getRecvPosition(), m_pOwner->GetPos() );
	}
}

void CGameCharaInputControlNetwork::ProcessNetworkPosition( GAMEINFO_CHARA_NETDATA * pRecvInfo)
{
	I3ASSERT( m_pOwner->getCharaNetIndex() != -1);
	if( m_pOwner->isCharaStateMask( CHARA_STATE_DEATH) == true ) return;

	getMoveContext()->setRecvPosTime( 0.f);

	GAME_CHARA_NETWORK_MOVE_TYPE type = _ProcessRecv_MoveDir( pRecvInfo);
	getInputContext()->setMoveType( type );	
}

void CGameCharaInputControlNetwork::ProcessNetworkRotationTheta( GAMEINFO_CHARA_NETDATA * pRecvInfo )
{
	getMoveContext()->setRecvThetaRhoTime( 0.f);

	getMoveContext()->setPostTheta( getBoneContext()->getTheta());
	getMoveContext()->setRecvTheta( pRecvInfo->_tPosRotData.getTheta());

	if( getMoveContext()->getPostTheta() < -I3_PI2 &&
		getMoveContext()->getRecvTheta() > I3_PI2 )
	{
		getMoveContext()->setPostTheta( getMoveContext()->getPostTheta() + I3_2PI);
	}
	else if( getMoveContext()->getPostTheta() > I3_PI2 &&
		getMoveContext()->getRecvTheta() < -I3_PI2 )
	{
		getMoveContext()->setPostTheta( getMoveContext()->getPostTheta() - I3_2PI);
	}

}

void CGameCharaInputControlNetwork::ProcessNetworkRotationRho( GAMEINFO_CHARA_NETDATA * pRecvInfo )
{
	getMoveContext()->setRecvThetaRhoTime( 0.f);

	getMoveContext()->setPostRho( getBoneContext()->getRhoValue());
	getMoveContext()->setRecvRho( pRecvInfo->_tPosRotData.getRho());

	if( getMoveContext()->getPostRho() < -I3_PI2 &&
		getMoveContext()->getRecvRho() > I3_PI2 )
	{
		getMoveContext()->setPostRho( getMoveContext()->getPostRho() + I3_2PI);
	}
	else if( getMoveContext()->getPostRho() > I3_PI2 &&
		getMoveContext()->getRecvRho() < -I3_PI2 )
	{
		getMoveContext()->setPostRho( getMoveContext()->getPostRho() - I3_2PI);
	}
}


void CGameCharaInputControlNetwork::_ProcessAutoActionUpper( void)
{
	CGameCharaActionContext * pActionCtx = getActionContext();
	if( pActionCtx->isLower( CHARA_LOWER_STUN ) )
		return;

	CGameCharaMoveContext * pMoveCtx = getMoveContext();

	switch( getActionContext()->getUpper() )
	{
	case CHARA_UPPER_NONE:		break;
	case CHARA_UPPER_MOVE:
		// 앉기, 걷기, 정지이면 지향사격자세로 변경
		// 3인칭은 걷기시에만 동작한다.
		if( m_pOwner->is1PV() )
		{
			if( (getInputContext()->getMoveType() == GAME_CHARA_NETWORK_STAND) )
				m_pOwner->Cmd_Idle_Upper();
			else
			{
				// 1인칭 및 이동중인경우에는 Run상태 애니메이션을 세팅
				m_pOwner->Cmd_Run( getMoveContext()->getMoveDir());
			}
		}
		else
		{
			if( !getActionContext()->isBody2( CHARA_BODYUPPER_ZOOM) )
			{
				if( getActionContext()->isBody( CHARA_BODYLOWER_CROUCH)
					|| (getInputContext()->getMoveType() == GAME_CHARA_NETWORK_STAND)
					//|| (getInputContext()->getMoveType() == GAME_CHARA_NETWORK_WALK) // revision 48723 WALK 역시 이동중인 경우에 포함되어 주석처리
					|| pMoveCtx->getSpeed() < I3_EPS )
				{	
						m_pOwner->Cmd_Idle_Upper();
				}
				else
				{
					// 1인칭 및 이동중인경우에는 상체에 대해 Run상태 애니메이션을 세팅
					m_pOwner->Cmd_Run( nullptr, true, false);

					// Move -> Idle로 전환 시, 0.3초 Move Upper 유지.
					{
						m_pOwner->setAttackIdleTime(0.3f);

						// 듀얼 무기 장착 시, 왼손도 적용.
						WeaponBase * pWeapon = getUsedWeapon();
						if (pWeapon && pWeapon->isDualGun())
							m_pOwner->setAttackIdleTimeLeft(0.3f);
					}
				}
			}
			else
			{
				// hansoft. 8170. 줌 모드에서 네트워크 캐릭터의 standing 상태일 때 상체 idle 전환.
				if (getInputContext()->getMoveType() == GAME_CHARA_NETWORK_STAND)
					m_pOwner->Cmd_Idle_Upper();
			}
		}
		break;
	
	case CHARA_UPPER_FIRE_READY:
	
	case CHARA_UPPER_LOADMAGAZINE:
	case CHARA_UPPER_LOADBULLET:
	case CHARA_UPPER_FIREBULLET:
	case CHARA_UPPER_DAMAGE:
	case CHARA_UPPER_SWAPWEAPON:
	case CHARA_UPPER_EXTENSION_ACTIVATE:
	case CHARA_UPPER_EXTENSION_DEACTIVATE:
	case CHARA_UPPER_REMOVESHELL:
	case CHARA_UPPER_NONEBATTLE_IDLE:
	case CHARA_UPPER_NONEBATTLE_WEAPONSHOW:
	case CHARA_UPPER_NONEBATTLE_IDLE_CHANGE:
	case CHARA_UPPER_NONEBATTLE_IDLE_STAND:
		break;

	case CHARA_UPPER_JUMP:
	case CHARA_UPPER_FIRE:
	case CHARA_UPPER_SECONDARY_FIRE:
		// C4는 자동으로 Idle 상태로 하지 않습니다.
		// UDP를 받으면 처리합니다.	2009.03.10	dfly
		{
			WeaponBase * pWeapon = m_pOwner->GetCurrentCharaWeapon();
			if( i3::same_of<WeaponC4*>( pWeapon))
				return;
		}

		m_pOwner->Cmd_Idle_Upper();
		break;
	case CHARA_UPPER_IDLE:
	default :
		if( m_pOwner->is1PV())
		{
			if( getInputContext()->getMoveType() != GAME_CHARA_NETWORK_STAND)
				m_pOwner->Cmd_Run( nullptr, true, false);
			else if( getInputContext()->getMoveType() == GAME_CHARA_NETWORK_WALK)
				m_pOwner->Cmd_Idle_Upper();
		}
		else
		{
			// 서서 뛰는 동작시에만 상체 애니메이션을 한다.
			if( ((getInputContext()->getMoveType() == GAME_CHARA_NETWORK_RUN)
				&& getActionContext()->isBody( CHARA_BODYLOWER_STAND))
				&& pMoveCtx->getSpeed() < I3_EPS )
			{// revision 47537
				m_pOwner->Cmd_Run( nullptr, true, false);
			}
		}
		break;
	}
}



