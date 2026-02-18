#include "pch.h"
#include "WeaponM197.h"
#include "GameCharaBase.h"
#include "GameCharaContexts.h"
#include "GlobalVariables.h"
#include "GlobalFunc.h"
#include "StageBattle.h"
#include "Camera/GameCamera_OnObject.h"
#include "StageObject/ObjectWithWeapon.h"
#include "M197Info.h"

#include "UI/UIBattlePopupBase.h"
#include "UI/UIBattleFrame.h" // 강퇴제안팝업시 닉네임마크를 삭제하기 위해 포함시킴.(11.06.09.수빈)
#include "CrosshairMgr.h"
#include "Crosshair.h"
#include "BattleSlotContext.h"

#define HUD_CROSSNICKCHECK_TIME		0.05f

I3_CLASS_INSTANCE(WeaponM197);//, WeaponBase);

WeaponM197::WeaponM197()
{
	InitVariables();
	m_pWeaponInfo = nullptr;
}
/*virtual*/ WeaponM197::~WeaponM197()
{
	m_pWeaponInfo = nullptr; 
}

void WeaponM197::InitVariables()
{
	i3Matrix::Identity(&m_matOrgGun);
	m_bNeedPrehit = true;
	m_bFirePenalty = false;

	m_rNoPressAccuTime = 0.f;
	m_rPressAccuTime = 0.f;
	m_rFireSpeedGauge = 0.f;
	m_rOldFireSpeedGauge = 0.f;
	m_rAccuTimeForPenalty = 0.f;

	i3Vector::Zero( &m_vMuzzleDir);
	i3Vector::Zero( &m_vBulletTraceStartPos);

	m_RecoilData.Reset();
	m_DeviationData.Reset();

	m_rCrossHitTimer	= 0.f;
	m_pCrossHitChara	= nullptr;

	m_bFired = false;
	m_pTurnOnSnd = nullptr;
	m_pSndInfo = nullptr;

	m_rElapsedLastFire = 0.0f;

}
/*virtual*/ REAL32	WeaponM197::GetFireDelay()
{
	REAL32 rRatio = 0.f;

	REAL32 rFactor =  ((CM197Info*)m_pWeaponInfo)->GetSlope_DelayPerFire() / ((CM197Info*)m_pWeaponInfo)->GetContinueFireTime();

	REAL32 rFireTime = ((CM197Info*)m_pWeaponInfo)->GetContinueFireTime() - m_rFireSpeedGauge;
	rRatio = (rFactor * rFireTime) * (rFactor * rFireTime) + 1.f;

	return m_pWeaponInfo->GetFireDelay() * rRatio;
}

void WeaponM197::InitWeaponOnObject()
{
	WeaponOnObject::InitWeaponOnObject();

	SetCameraLookAtIdx( getSceneObject()->GetBody()->FindBoneByName( "MagazineDummy") );
	i3Matrix::Copy( &m_matOrgGun, GetCameraLookAtMatrix() );
	
	m_pWeaponInfo = g_pWeaponInfoDataBase->getWeaponInfo( WEAPON_CLASS_OBJECT_GUN, (WEAPON::RES_ID)1, true);
}

void WeaponM197::Reset(void)
{
	WeaponOnObject::Reset();

	InitVariables();
}

void WeaponM197::ActiveWeaponOnObject(CGameObjectWithWeapon* pWeapon)
{
	WeaponOnObject::ActiveWeaponOnObject(pWeapon);

	
	m_bNeedPrehit = true;
	m_bFirePenalty = false;

	m_rNoPressAccuTime = 0.f;
	m_rPressAccuTime = 0.f;
	m_rFireSpeedGauge = 0.f;	
	m_rAccuTimeForPenalty = 0.f;
}

void WeaponM197::DeactiveWeaponOnObject()
{
	WeaponOnObject::DeactiveWeaponOnObject();

	_StopFireSound();
}

VEC3D* WeaponM197::GetMuzzleDir()
{ 
	i3Vector::Copy( &m_vMuzzleDir, i3Matrix::GetRight( getFXMatrix()) );
	m_vMuzzleDir.x = -m_vMuzzleDir.x;
	m_vMuzzleDir.y = -m_vMuzzleDir.y;
	m_vMuzzleDir.z = -m_vMuzzleDir.z;

	i3Vector::Normalize(&m_vMuzzleDir, &m_vMuzzleDir);

	return &m_vMuzzleDir; 
}

void WeaponM197::_PlayFireSound( WEAPON::FIRE_SOUND_TYPE type)
{
	m_rElapsedLastFire = 0.6f;

	WeaponBase::_PlayFireSound(type);
}

bool WeaponM197::FireWeaponOnObject()
{
	m_FireOrder = WEAPON::PRIMARY_FIRE;

	VEC3D vAt, vUp;


	i3Vector::Copy(m_StartPos, GetMuzzlePos() );
	i3Vector::Copy( &vAt, GetMuzzleDir() );
	i3Vector::Scale( &vAt, &vAt, GetRange());
	i3Vector::Add( m_TargetPos, &vAt, m_StartPos );
	i3Vector::Copy(&m_vBulletTraceStartPos, GetMuzzlePos() );
	i3Vector::Copy( &vAt, GetMuzzleDir() );
	i3Vector::ScaleAdd( &m_vBulletTraceStartPos, &vAt, -2.5f);
	i3Vector::Set(&vUp, 0.f, 1.f, 0.f);
	i3Vector::ScaleAdd( &m_vBulletTraceStartPos, &vUp, -1.f);

	SetDeviation();
	
	NET_Fire( m_StartPos, m_TargetPos, m_FireOrder );

	return true;	
}

void WeaponM197::UpdateAfterAnim( REAL32 rDeltaSeconds)
{
	if(m_bActive)
	{
		if( IsControlUser() )
		{
			I3ASSERT(g_pFramework);

			FireM197();
			
			
		}

		RotateM197();

		UpdateFlag();
	}

	//마지막으로 발사한 이후 1.0초가 지나고 나면 발사소리는 끕니다.
	m_rElapsedLastFire -= rDeltaSeconds;
	if( m_rElapsedLastFire  < 0.0f)
	{
		_StopFireSound();
	}
}

void WeaponM197::_OnEmitFireEffect()
{
	if( !i3::same_of<CGameCamera_OnObject*>(g_pCamera))
	{
		WeaponBase::_OnEmitFireEffect();
	}
}

VEC3D*	WeaponM197::GetBulletTraceStartPos()
{
	if( !i3::same_of<CGameCamera_OnObject*>(g_pCamera))
	{
		return WeaponBase::GetBulletTraceStartPos();
	}
	else 
	{
		return &m_vBulletTraceStartPos;
	}
}

void WeaponM197::UpdateFlag()
{
	

	if(m_bNeedPrehit == true)
	{
		if( m_rPressAccuTime >= ((CM197Info*)m_pWeaponInfo)->GetFirstDelayTime() )
		{
			m_bNeedPrehit = false;
		}
	}

	if(m_rAccuTimeForPenalty > ((CM197Info*)m_pWeaponInfo)->GetFirePenaltyTime())
	{
		m_bFirePenalty = false;
	}
}

void WeaponM197::FireM197()
{
	UINT64 keyPress	= g_pFramework->getKeyPress();	
	UINT64 keyStroke = g_pFramework->getKeyStroke();	

	if( keyPress & GAME_KEY_ACTION_ATTACK || keyStroke & GAME_KEY_ACTION_ATTACK)
	{	
		if(m_bNeedPrehit == true && m_rPressAccuTime < 0.00001f && m_bFirePenalty == false)
		{
			m_pTurnOnSnd = g_pSndMng->StartWeaponStateSnd( GTBWS_M197, 2, GetMuzzlePos(), (bool)((m_pOwner != nullptr) && ( m_pOwner->is1PV())));
			if( m_pTurnOnSnd != nullptr)
			{
				m_pSndInfo = m_pTurnOnSnd->getPlayInfo();
			}

			//GlobalFunc::PB_TRACE("WM_M197_Turn_On \n");
		}

		if(m_bNeedPrehit == false && m_rFireSpeedGauge > 0.f && m_bFirePenalty == false)
		{
			if( (getWeaponState() == WEAPON::STATE_READY) || (getWeaponState() == WEAPON::STATE_NEEDTO_FIRE) )
			{
				FireWeaponOnObject();
				m_bFired = true;
			}
		}
		m_rNoPressAccuTime = 0;
		m_rPressAccuTime += g_pViewer->getDeltaTime();

		if(m_bNeedPrehit == false && m_bFirePenalty == false)
		{
			m_rFireSpeedGauge -= g_pViewer->getDeltaTime();
		}

		if(m_rFireSpeedGauge < 0)
		{
			if( (m_rFireSpeedGauge * m_rOldFireSpeedGauge) < 0.f )
			{
				_StopFireSound();

				g_pSndMng->StartWeaponStateSnd( GTBWS_M197, 0, GetMuzzlePos(), (bool)((m_pOwner != nullptr) && ( m_pOwner->is1PV())));
				//GlobalFunc::PB_TRACE("WM_M197_OverHeat %f %f \n", (m_rPressAccuTime - m_rFireSpeedGauge), 5.5f + g_pViewer->getDeltaTime());
				m_bFired = false;
			}
			m_bFirePenalty = true;
			m_rFireSpeedGauge = -0.000000001f;
			m_rAccuTimeForPenalty = 0;
		}
	}
	else 
	{
		m_bNeedPrehit = true;

		_StopFireSound();
		//sound manager에서 return받은 사운드는 내부에서 플레이가 끝나면 invalid하게된다.
		//현재 시점에서 이미 다른 사운드를 플레이 하고있을수도 있다.
		{
			if(m_pTurnOnSnd != nullptr)
			{
				if( m_pSndInfo == m_pTurnOnSnd->getPlayInfo())
				{
					m_pTurnOnSnd->Stop();
				}
			}

			m_pTurnOnSnd = nullptr;
		}

		if(m_rNoPressAccuTime < 0.00001f)
		{
			if(m_bFired && m_rPressAccuTime > ((CM197Info*)m_pWeaponInfo)->GetFirstDelayTime())
			{
				g_pSndMng->StartWeaponStateSnd( GTBWS_M197, 1, GetMuzzlePos(), (bool)((m_pOwner != nullptr) && ( m_pOwner->is1PV())));
				//GlobalFunc::PB_TRACE("WM_M197_Turn_Off \n");
			}
		}

		m_rPressAccuTime = 0;
		m_rNoPressAccuTime += g_pViewer->getDeltaTime();

		m_rFireSpeedGauge += g_pViewer->getDeltaTime() * ((CM197Info*)m_pWeaponInfo)->GetChargeSpeedRatio();
		if( m_rFireSpeedGauge > ((CM197Info*)m_pWeaponInfo)->GetContinueFireTime() )
		{
			m_rFireSpeedGauge = ((CM197Info*)m_pWeaponInfo)->GetContinueFireTime();
		}

		setAutoFireCount( 0);
		m_bFired = false;
	}

	m_rAccuTimeForPenalty += g_pViewer->getDeltaTime();

	m_rOldFireSpeedGauge = m_rFireSpeedGauge;
}



void WeaponM197::RotateM197()
{
	REAL32 rMoveRho = 0.f;
	REAL32 rMoveTheta = 0.f;

	// [RU.440] 헬기 탑승 후 ESC 키로 설정 창을 띄울 때, 1인칭 카메라가 고정되지 않는 문제 수정
	// 게임 키 모드일 때만 마우스 입력을 받도록 합니다
	if( IsControlUser() && g_pFramework->getGameKeyMode() == GAMEKEY_MODE_GAME)
	{
		rMoveRho = -g_pFramework->getMouseMoveDelta()->y * 0.005f;
		rMoveTheta = -g_pFramework->getMouseMoveDelta()->x * 0.005f;

		m_rObjectRho += rMoveRho;
		m_rObjectTheta += rMoveTheta;

		if(m_rObjectRho > 0)
		{
			m_rObjectRho = 0;
		}

		if(m_rObjectRho < I3_DEG2RAD( -((CM197Info*)m_pWeaponInfo)->GetLimitRho() ))
		{
			m_rObjectRho = I3_DEG2RAD( -((CM197Info*)m_pWeaponInfo)->GetLimitRho() );
		}

		if(m_rObjectTheta > I3_DEG2RAD( ((CM197Info*)m_pWeaponInfo)->GetLimitTheta() ))
		{
			m_rObjectTheta = I3_DEG2RAD( ((CM197Info*)m_pWeaponInfo)->GetLimitTheta() );
		}

		if(m_rObjectTheta < I3_DEG2RAD( -((CM197Info*)m_pWeaponInfo)->GetLimitTheta() ))
		{
			m_rObjectTheta = I3_DEG2RAD( -((CM197Info*)m_pWeaponInfo)->GetLimitTheta() );
		}

		CGameCharaBase * pPlayer = g_pCharaManager->getCharaByNetIdx( BattleSlotContext::i()->getMySlotIdx());

		if( pPlayer != nullptr)
		{
			pPlayer->getBoneContext()->setTheta( m_rObjectTheta);
			pPlayer->getBoneContext()->setRho( m_rObjectRho);

			GAMEEVENT::Write_Chara_SyncUseObject( pPlayer, GetObjectWithWeapon(), NET_SYNCKEY_ON_OBJECT);
		}

		if( g_pFramework->getMouseMoveDelta()->x != 0.0f)
		{
			// 수평 반동의 회복 중, 마우스가 돌아가면 회복 모드를 Cancel한다.
			m_RecoilData.m_HorzState = RECOIL_HORZ_NA;
		}
	}
	else
	{
		ReceiveAngle();
	}	

	CalcDeviation( g_pViewer->getDeltaTime());

	REAL32 recoil = I3_DEG2RAD( m_RecoilData.m_RecoilAngle );
	REAL32 rho = m_rObjectRho;

	MATRIX matDummy;
	i3Matrix::Copy(&matDummy, &m_matOrgGun);
	i3Matrix::SetPos(&matDummy, 0.f, 0.f, 0.f, 1.f);
	
	i3Matrix::PostRotateZ(&matDummy, m_rObjectTheta);
	i3Matrix::PostRotateAxis(&matDummy, i3Matrix::GetUp(&matDummy), rho + recoil);

	i3Matrix::SetPos(&matDummy, i3Matrix::GetPos(GetCameraLookAtMatrix()) );
	
	i3Matrix::Copy(GetCameraLookAtMatrix(), &matDummy);
}



void WeaponM197::ReceiveAngle()
{
	I3ASSERT( GetObjectWithWeapon() );
	if(GetObjectWithWeapon()->GetEnterCharaIdx() == -1)		return;
	CGameCharaBase * pChara = g_pCharaManager->getCharaByNetIdx( GetObjectWithWeapon()->GetEnterCharaIdx());
	if( pChara == nullptr)		return;

	GAMEINFO_CHARACTER *	pGameInfo	= BattleSlotContext::i()->setGameInfo_Chara( GetObjectWithWeapon()->GetEnterCharaIdx() );

	if(pGameInfo->_bRecvFlag == true)
	{
		pChara->getMoveContext()->setPostTheta( m_rObjectTheta);
		pChara->getMoveContext()->setRecvTheta( pGameInfo->_tNetData._tPosRotData.getTheta());
		pChara->getMoveContext()->setPostRho( m_rObjectRho);
		pChara->getMoveContext()->setRecvRho( pGameInfo->_tNetData._tPosRotData.getRho());
		pChara->getMoveContext()->setRecvThetaRhoTime( 0.f);

		pGameInfo->_bRecvFlag	= false;
	}
	
	REAL32	flerp = pChara->getMoveContext()->getRecvThetaRhoTime();
	flerp += g_pViewer->getDeltaTime();
	pChara->getMoveContext()->setRecvThetaRhoTime( flerp);

	if( flerp > 0.1f )
	{
		pChara->getMoveContext()->setPostTheta( pChara->getMoveContext()->getRecvTheta());
		pChara->getMoveContext()->setPostRho( pChara->getMoveContext()->getRecvRho());
		flerp = 0.1f;
	}
	flerp *= 10.f;

	//	Theta
	m_rObjectTheta =  pChara->getMoveContext()->getPostTheta() +
		((pChara->getMoveContext()->getRecvTheta() - pChara->getMoveContext()->getPostTheta()) * flerp);
	//	Rho
	m_rObjectRho = pChara->getMoveContext()->getPostRho() +
		((pChara->getMoveContext()->getRecvRho() - pChara->getMoveContext()->getPostRho()) * flerp);
}

void WeaponM197::SetDeviation( void)
{
	REAL32 dev, factor = 1.0f;
	
	dev = GetDeviation() * factor;		// KEYWORD : 호칭

	m_DeviationData.m_FireDeviation = m_DeviationData.m_DeviationAngle;
	m_DeviationData.m_DeviationAngle = m_DeviationData.m_DeviationTargetAngle = m_DeviationData.m_DeviationAngle + dev;
	m_DeviationData.m_DeviationStartAngle = m_DeviationData.m_DeviationAngle;

	if( m_DeviationData.m_DeviationTargetAngle > m_pWeaponInfo->GetCrossHairMax())
		m_DeviationData.m_DeviationTargetAngle = m_pWeaponInfo->GetCrossHairMax();

	m_DeviationData.m_DeviationTargetTime = getCrosshairSpeedFactor() * m_pWeaponInfo->GetCrossHairContractTime();

	m_DeviationData.m_DeviationTime = 0.0f;
	m_DeviationData.m_DeviationState = DEVIATION_CHANGE;
}

void WeaponM197::CalcDeviation( REAL32 rDeltatime)
{
	I3ASSERT( m_pWeaponInfo != nullptr);

	REAL32 recoilHorz;

	if( m_RecoilData.m_RecoilHorzTime < m_RecoilData.m_RecoilHorzTimeTarget)
	{
		// 수평 반동의 적용
		if( m_RecoilData.m_HorzState != RECOIL_HORZ_FIRING)
		{
			// 새롭게 수평 반동이 적용되기 시작했다. (새로운 연사)
			m_RecoilData.m_HorzStartAngle = m_rObjectTheta;
			m_RecoilData.m_HorzState = RECOIL_HORZ_FIRING;			
		}

		recoilHorz = m_RecoilData.m_RecoilHorzAngle * (rDeltatime * m_RecoilData.m_rRecoilHorzTimeTarget);

		m_rObjectTheta += I3_DEG2RAD( recoilHorz);

		m_RecoilData.m_RecoilHorzTime = m_RecoilData.m_RecoilHorzTime + rDeltatime;
	}
	else
	{
		// 수평 반동이 적용되지 않는다.
		if( (m_RecoilData.m_HorzState == RECOIL_HORZ_FIRING) )
		{
			// 발사 중인데도 getAutoFireCount() == 0이면 연사를 멈춘 상황
			if( getAutoFireCount() <= 0)
			{
				m_RecoilData.m_HorzState = RECOIL_HORZ_RESTORE;		// 원래의 Theta로 되돌아가는 모드로 변환.
				m_RecoilData.m_timeHorzRestore = 0.0f;
			}
		}
		
		if( m_RecoilData.m_HorzState == RECOIL_HORZ_RESTORE)
		{
			// Theta의 Restore 상태라면...
			while( m_rObjectTheta < 0.0f)
				m_rObjectTheta += I3_2PI;

			REAL32 oldTheta = m_rObjectTheta;
			while( m_RecoilData.m_HorzStartAngle < 0.0f)
				m_RecoilData.m_HorzStartAngle = m_RecoilData.m_HorzStartAngle + I3_2PI;

			REAL32 diff = m_RecoilData.m_HorzStartAngle - m_rObjectTheta;

			if( i3Math::abs( diff) >= I3_PI)
			{
				// 각도가 차가 180도를 넘어간다면 동일각에 대한 각도의 표현 차이(0 == 360) 때문에
				// 발생하는 현상이 있다.
				// 예를 들어, 왼쪽으로 10도만 돌면 될 것이 오른쪽으로 350도 돌아가는 현상이 그것이다.
				//
				// 아래는 그것을 방지하기 위해, 회전각이 180도를 넘어가는 경우에 보정을 한다.
				//
				if( diff > 0.0f)
					m_RecoilData.m_HorzStartAngle = m_RecoilData.m_HorzStartAngle - I3_2PI;
				else
					m_RecoilData.m_HorzStartAngle = m_RecoilData.m_HorzStartAngle + I3_2PI;

				diff = m_RecoilData.m_HorzStartAngle - m_rObjectTheta;
			}

			if( m_RecoilData.m_HorzStartAngle > oldTheta)
			{
				if( oldTheta >= m_RecoilData.m_HorzStartAngle)
				{
					m_RecoilData.m_timeHorzRestore = 2.0f;
					m_rObjectTheta = m_RecoilData.m_HorzStartAngle;
				}
			}
			else
			{
				if( oldTheta <= m_RecoilData.m_HorzStartAngle)
				{
					m_RecoilData.m_timeHorzRestore = 2.0f;
					m_rObjectTheta = m_RecoilData.m_HorzStartAngle;
				}
			}

			m_rObjectTheta += (diff) * 0.07f * m_RecoilData.m_RecoilHorzAngle_Recovertime;

			m_RecoilData.m_timeHorzRestore = m_RecoilData.m_timeHorzRestore + rDeltatime;

			if( m_RecoilData.m_timeHorzRestore > 2.0f)
			{
				m_RecoilData.cancelRecoilHorz();
			}
		}
	}

	m_RecoilData._calcRecoil( rDeltatime );

	REAL32 base = m_pWeaponInfo->GetCrossHairMin();
	REAL32 newDeviation = base;
	
	newDeviation += (base * m_pWeaponInfo->GetCrossHairSizeStand());

	if( newDeviation < 0.0f)
		newDeviation = 0.0f;

	if( newDeviation > m_pWeaponInfo->GetCrossHairMax() )
		newDeviation = m_pWeaponInfo->GetCrossHairMax();
	
	if( newDeviation != m_DeviationData.m_DeviationTargetAngle)
	{	
		REAL32 speed = getCrosshairSpeedFactor() * m_pWeaponInfo->GetCrossHairContractTime();

		if( newDeviation > m_DeviationData.m_DeviationAngle)
		{
			// Crosshair가 커지는 것은 아주 빠르게 한다.
			speed = m_pWeaponInfo->GetCrossHairExpandTime();
		}

		m_DeviationData.m_DeviationTargetAngle = newDeviation;
		m_DeviationData.m_DeviationStartAngle = m_DeviationData.m_DeviationAngle;
		m_DeviationData.m_DeviationState = DEVIATION_CHANGE;
		m_DeviationData.m_DeviationTime = 0.0f;
		m_DeviationData.m_DeviationTargetTime = speed;

		//I3TRACE( "DEVIATION TARGET : %f\n", newDeviation);
	}

	m_DeviationData._calcDeviation( rDeltatime );
}

void WeaponM197::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos)
{	
	VEC3D target, start, dir, vXAxis, vYAxis;
	REAL32 length;
	INT32 i, collGroup;

	if(GetObjectWithWeapon() == nullptr) return;


	I3ASSERT( pStartPos != nullptr);
	I3ASSERT( pTargetPos != nullptr);

	i3Vector::Copy( &target, pTargetPos);
	i3Vector::Copy( &start, pStartPos );

	if(GetEnterCharaIdx() == -1)
	{
		return;
	}
	if(g_pCharaManager->getCharaByNetIdx(GetEnterCharaIdx()) == nullptr)
	{
		// GlobalFunc::PB_TRACE("GetEnterCharaIdx %d \n", GetEnterCharaIdx());
		return;
	}

	
	if( g_pCharaManager->getCharaByNetIdx(GetEnterCharaIdx())->getCharaTeam() & CHARACTER_TEAM_RED )		
	{
		collGroup = CGRP_MASKGROUP_CHARA_BLUE;
	}
	else
	{
		collGroup = CGRP_MASKGROUP_CHARA_RED;
	}

	// GlobalFunc::PB_TRACE("FireCollision collGroup %d iTest %d\n", collGroup, iTest);

	// 네트워크로 보내줍니다.
	NET_FireSync( &target );

	i3Vector::Sub( &dir, &target, &start);
	length = i3Vector::Normalize( &dir, &dir);

	i3Vector::Cross( &vXAxis, &dir, &I3_YAXIS);
	i3Vector::Normalize( &vXAxis, &vXAxis);

	i3Vector::Cross( &vYAxis, &dir, &vXAxis);
	i3Vector::Normalize( &vYAxis, &vYAxis);

	REAL32 deviation;

	if( m_pOwner->isLocal() )
		deviation = m_DeviationData.m_FireDeviation;
	else
		deviation = m_DeviationData.m_DeviationAngle;

	//I3TRACE("%f deviation %f in FireCollision \n", g_pFramework->GetAccTime(), deviation);

	for( i = 0; i < m_pWeaponInfo->GetFireBulletCount(); i++)
	{
		REAL32 r, t;
		VEC3D tTarget, tdir;
		MATRIX tmp;

		r = i3Math::Randf2();
		
		r *= deviation * 0.5f * 0.5f;
		t = i3Math::Randf2() * deviation * 0.5f * 0.5f;

		i3Matrix::SetRotateAxisDeg( &tmp, &vXAxis, r);
		i3Matrix::PostRotateAxisDeg( &tmp, &vYAxis, t);

		i3Vector::TransformNormal( &tdir, &dir, &tmp);

		i3Vector::Scale( &tdir, &tdir, length);
		i3Vector::Add( &tTarget, &start, &tdir);

		DoLineCollision( &start, &tTarget, collGroup);
		//i3Vector::Dump( &tTarget);
	}

	INT32 autoFireCount = getAutoFireCount();

	autoFireCount++;
	setAutoFireCount( autoFireCount);
}

void WeaponM197::CheckCrossHit( void)
{
	i3CollideeLine line;

	bool	bNeutralColor = true;

	if(m_pCrossHitChara == nullptr) return;

	// 캐릭터의 넷 인덱스가 -1인 경우는 해당 캐릭터가 나간경우이다.
	// 업데이트 순서상 이전에 체크된 캐릭터이므로 -1이면 출력하지 말아야합니다. -by dfly 2007.12.20
	if( (m_pCrossHitChara != nullptr) && (m_pCrossHitChara->getCharaNetIndex() != -1) && (m_pCrossHitChara->isNetwork()) )
	{
		if( LocaleValue::Enable( "ForciblyRemove" ) )
		{
			if (UIBattleFrame::i()->GetPopup(UBP_ACCUSE_CRIME)->isEnable() == false)	
				m_pCrossHitChara->Cmd_SetCharaNick( false);
		}
		else
		{
			m_pCrossHitChara->Cmd_SetCharaNick( false);
		}
	}

	m_rCrossHitTimer += g_pViewer->getDeltaTime();
	// 초당 20회만 체크
	if( m_rCrossHitTimer < HUD_CROSSNICKCHECK_TIME )
		return;

	REAL32 rMinLength = 100000.f;
		
	Crosshair * pCrosshair = CrosshairMgr::i()->getCrosshair();
	
	// 월드 컬리젼
	line.SetStart( GetMuzzlePos());
	line.SetDir( GetMuzzleDir());
	I3_PHYSIX_HIT_RESULT * pResult = g_pFramework->raycastClosest( &line, CGRP_MASKGROUP_ATTACK);
	if( pResult != nullptr)
	{
		g_pFramework->reserveRaycastResult( 1);
		rMinLength = MIN( pResult->m_T, GetRange());
	}

	m_pCrossHitChara = nullptr;
	if( m_pWeaponInfo->isUnconditionalNeutralColorCrossHair())		bNeutralColor = true;
	else
	{
		UINT32 collGroup;
		
		if(GetEnterCharaIdx() == -1)
		{
			return;
		}
		if(g_pCharaManager->getCharaByNetIdx(GetEnterCharaIdx()) == nullptr)
		{
			return;
		}

		if( g_pCharaManager->getCharaByNetIdx(GetEnterCharaIdx())->getCharaTeam() & CHARACTER_TEAM_BLUE )
		{
			collGroup = CGRP_MASKGROUP_CHARA_RED;
		}
		else
		{
			collGroup = CGRP_MASKGROUP_CHARA_BLUE;
		}

		UINT32 nResultCount = g_pFramework->raycast( &line, collGroup);
		if( nResultCount > 0 )
		{
			pResult = g_pFramework->getRaycastResult();

			g_pFramework->reserveRaycastResult( nResultCount + 1);

			for( UINT32 i = 0; i < nResultCount; i++ )
			{
				if( rMinLength < pResult[ i].m_T )
					continue;

				I3ASSERT( pResult[ i ].m_pDestShapeSet != nullptr);

				if( !i3::same_of<CGameCharaBase*>(pResult[ i ].m_pDestShapeSet->getEventReceiver()))
					continue;

				CGameCharaBase * pChara = (CGameCharaBase *) pResult[ i ].m_pDestShapeSet->getEventReceiver();
				if( !(pChara->isCharaStateMask( CHARA_STATE_DEATH)) )
				{
					bNeutralColor = false;
					rMinLength = pResult[ i].m_T;
					m_pCrossHitChara = pChara;
				}
			}
		}
	}

	if( bNeutralColor)//보통색
	{
		// 조준된 상대라도 죽엇으면 보통 색상으로 변경
		//pCrosshair->SetCrosshairColor( 0, 255, 0);
		pCrosshair->SetCrosshairColor();
	}
	else			  //뻘건색
	{	
		pCrosshair->SetCrosshairColor( 255, 0, 0);
	}

	m_rCrossHitTimer = 0.f;

	g_pFramework->resetRaycastResult();
}

/*virtual*/ void WeaponM197::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	WeaponOnObject::AttachNode( pSceneNode, pOwner);

	addWeaponFlag( WEAPON_FLAG_TAKE_CHARA);
}

