#include "pch.h"
#include "WeaponWatermelon.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaActionContext.h"
#include "GameCharaBoneContext.h"
#include "./Character/GameCharaControlBase.h"
#include "BattleSlotContext.h"
#include "GameMissionManager.h"
#include "WeaponFunction.h"

#include "BattleObjContext.h"


I3_CLASS_INSTANCE( WeaponWatermelon);//, WeaponBase);

WeaponWatermelon::WeaponWatermelon(void)
{
	m_aii_IDLE_Female	= -1;
	m_aii_IDLE_Male		= -1;
}

WeaponWatermelon::~WeaponWatermelon(void)
{
}

void WeaponWatermelon::_InitCollideeGroup( void)
{
	WF::SetCollideeGroup_NoUseCollidee(this);
}

void WeaponWatermelon::Reset()
{
	WeaponBase::Reset();		//	먼저 해야함.

	SetMatrix( &m_mtxOld);
}

void WeaponWatermelon::PostCreate( void)
{
	i3Matrix::Identity( &m_mtxOld);

	m_aii_IDLE_Female			= (INT8) m_pAICtx->FindAIStateByName( "Fire");
	m_aii_IDLE_Male				= (INT8) m_pAICtx->FindAIStateByName( "Idle");
}


void WeaponWatermelon::OnFire( i3AIContext * pCtx, REAL32 tm)
{
}

void WeaponWatermelon::OnEndFire( i3AIContext * pCtx, REAL32 tm)
{
}

void WeaponWatermelon::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	if( m_pOwner != nullptr)
	{
		if(m_pOwner->isFemale())
			WF::PlayWeaponAnim(this, m_aii_IDLE_Female);
		else
			WF::PlayWeaponAnim(this, m_aii_IDLE_Male);
	}
}

void WeaponWatermelon::OnUpdate( REAL32 rDeltaSeconds)
{
	WeaponBase::OnUpdate( rDeltaSeconds);
}

//void WeaponWatermelon::Fire(void)
//{
//}

void WeaponWatermelon::FireEnd(void)
{	
	WeaponBase::FireEnd();
}

void WeaponWatermelon::ReturnToReady( void)
{	
	WeaponBase::ReturnToReady();
}

void WeaponWatermelon::Explosion()
{	
}

void WeaponWatermelon::_OnCheckCollisionByExplosion(void)
{
}

bool WeaponWatermelon::OnLostDevice( bool bLostDevice)
{
	if( i3GameNode::OnLostDevice( bLostDevice) == false)
		return false;

	return true;
}

bool WeaponWatermelon::OnRevive( i3RenderContext * pCtx)
{
	if( i3GameNode::OnRevive( pCtx) == false)
		return false;

	return true;
}

void WeaponWatermelon::DropToWorld(i3Node* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld, MATRIX* pMat)
{
	VEC3D vWeaponPos = VEC3D( pPos->x, pPos->y, pPos->z);

	if( getOwner() == nullptr )
		return;

	SetMatrix( getOwner()->GetMatrix());

	//	오브젝트를 임의 회전. 올바른 방향을 바라 보도록. 캐릭터가 들고 있을시는 다시 복구해야 한다.
	i3Matrix::PreRotateAxisDeg( GetMatrix(), &I3_XAXIS, -135.f);
	i3Matrix::PostRotateAxisDeg( GetMatrix(), &I3_YAXIS, 180.f);

	i3Vector::ScaleAdd( &vWeaponPos, i3Matrix::GetUp( GetMatrix()), 0.01f);
	i3Vector::ScaleAdd( &vWeaponPos, i3Matrix::GetRight( GetMatrix()), 0.05f);	// hight 5cm

	WeaponBase::Transmission( pNewSceneNode, pNewOwner, &vWeaponPos, true);

}