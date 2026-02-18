#include "pch.h"

#ifdef	DOMI_OBJECT

#include "WeaponDominationObject.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaActionContext.h"
#include "GameCharaBoneContext.h"
#include "./Character/GameCharaControlBase.h"
#include "WeaponFunction.h"

I3_CLASS_INSTANCE( WeaponDominationObject)//, WeaponBase);

WeaponDominationObject::WeaponDominationObject(void) : m_rInstallElapsedTime(0.0f)
						, m_rUninstallElpasedTime(0.0f)
						, m_rExplosionElapsedTime(0.0f)
						, m_rTracerUpdateTime(0.0f)
						, m_bDisarm(false)
						, m_bEnableActive(false)
{
	
}

WeaponDominationObject::~WeaponDominationObject(void)
{
	
}

void WeaponDominationObject::_OnInitVairable( void)
{	
	WeaponBase::_OnInitVairable();

	m_rInstallElapsedTime	= 0.f;				// 설치 타임
	m_rUninstallElpasedTime	= 0.f;				// 해체 타임	
	m_rExplosionElapsedTime = 0.f;
	m_rTracerUpdateTime		= 0.f;

	m_bDisarm				= false;
	m_bEnableActive			= false;


	i3Matrix::Identity( &m_mtxOld);

	
}

void WeaponDominationObject::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NoUseCollidee(this);
}

void WeaponDominationObject::Reset()
{
	WeaponBase::Reset();		//	먼저 해야함.

	SetMatrix( &m_mtxOld);

	m_StateMaskWeapon		= WSM_LOADBULLET_DISABLE | WSM_LOADMAG_DISABLE;

	m_rInstallElapsedTime	= 0.f;	
	m_rUninstallElpasedTime	= 0.f;	
	m_rExplosionElapsedTime = 0.f;
	m_rTracerUpdateTime		= 0.f;

	m_bDisarm				= false;
	m_bEnableActive			= false;


}


void WeaponDominationObject::PostCreate()
{
	i3Matrix::Identity( &m_mtxOld);
}

void WeaponDominationObject::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	m_rInstallElapsedTime = 0.f;
}
void WeaponDominationObject::OnUpdate( REAL32 rDeltaSeconds)
{	
	WeaponBase::OnUpdate( rDeltaSeconds);

	if(this->getWeaponInfo()->GetNumber() == GetItemIndex("MISSION_OBJECT3"))
	{
		this->OnSetBlendAlpha(0.5);
	}
}

//void WeaponDominationObject::Fire(void)
//{
//	if( getLoadedBulletCount() <= 0)
//		return;
//
//	// 시점이 향하고 있는 목표 지점을 구한다.
//	NET_Fire( nullptr, nullptr );
//}

void WeaponDominationObject::FireEnd(void)
{	
	WeaponBase::FireEnd();

	m_rInstallElapsedTime = 0.f;
}

void WeaponDominationObject::ActiveDisarm( bool bEnable)
{
	if( m_bEnableActive)	//	설치된 상태만 가능
	{
		if( bEnable)
		{
	
			m_bDisarm = true;
		}
		else
		{
			m_bDisarm = false;
			m_rUninstallElpasedTime = 0.f;
		}
	}
}

void WeaponDominationObject::ReturnToReady( void)
{	
	WeaponBase::ReturnToReady();

	m_rInstallElapsedTime	= 0.f;	
	m_rUninstallElpasedTime	= 0.f;	
	m_rExplosionElapsedTime = 0.f;
	m_rTracerUpdateTime		= 0.f;

}


void WeaponDominationObject::Uninstall( void)
{

	m_bEnableActive = false;

	//	반드시 무기 매니저를 통해 제거해야 한다.
	g_pWeaponManager->PushDropWeaponToChara( this );
	//g_pFramework->InvisableDebugObj(false);


}
void WeaponDominationObject::InstallToWorld(i3Node* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld, MATRIX* pMat, REAL32 theta)
{
	VEC3D vWeaponPos = VEC3D( pPos->x, pPos->y, pPos->z);
	
	if( getOwner() == nullptr )
		return;
	


	//제압보스미션용 C4는 인덱스 19번~20번 입니다.
	if (this->getWeaponInfo()->GetNumber() > 18)
	{
		//	오브젝트를 임의 회전. 올바른 방향을 바라 보도록. 캐릭터가 들고 있을시는 다시 복구해야 한다.
		i3Matrix::PostRotateAxisDeg( GetMatrix(), &I3_XAXIS, -90.f);
		i3Matrix::PostRotateAxisDeg( GetMatrix(), &I3_YAXIS, 90.f);

		i3Vector::AddY( &vWeaponPos, 0.65f);	//바닥에서 위쪽으로 폭탄 위치 올림...
	}
	else	
	{
		SetMatrix( getOwner()->GetMatrix());

		//	오브젝트를 임의 회전. 올바른 방향을 바라 보도록. 캐릭터가 들고 있을시는 다시 복구해야 한다.
		i3Matrix::PreRotateAxisDeg( GetMatrix(), &I3_XAXIS, -90.f);
		i3Matrix::PostRotateAxisDeg( GetMatrix(), &I3_YAXIS, 0);
		
		i3Vector::ScaleAdd( &vWeaponPos, i3Matrix::GetUp( GetMatrix()), 0.01f);
		i3Vector::ScaleAdd( &vWeaponPos, i3Matrix::GetRight( GetMatrix()), 0.08f);	// hight 5cm
		i3Vector::ScaleAdd( pPos, i3Matrix::GetAt( GetMatrix()), 1.1f);
	}
		

	
	
	g_pSndMng->StartWeapon_C4StateSnd( GTFW_C4_INSTALLATION, pPos, false, false);

	I3TRACE( "--- 회수아이템 설치 완료 (Pos %.4f %.4f %.4f).\n", pPos->x, pPos->y, pPos->z);
	//	C4는 월드(ParentGameNode)에 C4(GameNode)가 붙어야 합니다. (Owner 캐릭터가 사라진다 해도 OnUpdate를 호출해야 하기 때문에)
	WeaponBase::Transmission( pNewSceneNode, pNewOwner, &vWeaponPos, true);


//	g_pFramework->SetPosDebugObj(0,vWeaponPos);
//	g_pFramework->InvisableDebugObj(true);


	
}

void WeaponDominationObject::ReturnInstance( void)
{	
	m_bEnableActive = false;

	Reset();

	//	노드 제거 (렌더링과 게임노드에서 제거된다.)
	WeaponBase::ReturnInstance();

}


void WeaponDominationObject::_OnCollision(CGameCharaBase * pChara, i3CollideeLine * pLine, I3_PHYSIX_HIT_RESULT * pResult)
{	
	_OnHit( pResult, pLine, false);
}

#endif