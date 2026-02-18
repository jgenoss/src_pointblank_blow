#include "pch.h"
#include "WeaponP90Ext.h"
#include "GlobalVariables.h"
#include "GameCharaCollisionContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaActionContext.h"
#include "GameCharaWeaponContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "./Character/GameCharaControlBase.h"

I3_CLASS_INSTANCE( WeaponP90Ext);//, WeaponDotLaser);


WeaponP90Ext::WeaponP90Ext(void)
{
}

WeaponP90Ext::~WeaponP90Ext(void)
{
}

void WeaponP90Ext::Reset( void)
{
	WeaponDotLaser::Reset();

	ResetBullet();
}

void WeaponP90Ext::OnBindResource( void)
{
	WeaponDotLaser::OnBindResource();
}

void WeaponP90Ext::OnIdle( i3AIContext * pCtx, REAL32 tm)
{
	WeaponDotLaser::OnIdle( pCtx, tm);
}

void WeaponP90Ext::OnFire( i3AIContext * pCtx, REAL32 tm)
{
	WeaponDotLaser::OnFire( pCtx, tm);	
}

bool WeaponP90Ext::OnLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	bool bRet = WeaponDotLaser::OnLoadMag( pCtx, tm);

	if( pCtx->getLocalTime() > m_pWeaponInfo->GetReloadTime() * 0.2f)
	{
		ResetBullet();
	}

	return bRet;
}

void WeaponP90Ext::OnEndLoadMag( i3AIContext * pCtx, REAL32 tm)
{
	WeaponDotLaser::OnEndLoadMag( pCtx, tm);
}

void WeaponP90Ext::OnUpdate( REAL32 rDeltaSeconds)
{	
	WeaponDotLaser::OnUpdate( rDeltaSeconds);	
}

void WeaponP90Ext::AttachNode( i3Node * pSceneNode, i3GameNode * pOwner)
{
	WeaponDotLaser::AttachNode( pSceneNode, pOwner);
}

void WeaponP90Ext::DetachNode( void )
{
	WeaponDotLaser::DetachNode();
}

void WeaponP90Ext::OnDrop( void)
{
	WeaponDotLaser::OnDrop();
}

void WeaponP90Ext::GetShotTargetPos( VEC3D * pDest, VEC3D * pStart)
{
	WeaponDotLaser::GetShotTargetPos( pDest, pStart);			
}

void WeaponP90Ext::FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos)
{
	WeaponDotLaser::FireCollision( pStartPos, pTargetPos);
}

void WeaponP90Ext::ResetBullet(void)
{
	m_BulletList.clear();

	for( INT32 i = 1; i < P90EXT_BULLETBONE_COUNT+1; i++)
	{
		char szBoneName[16];
		sprintf_s( szBoneName, "Model_Bullet%.2d", i);
		
		i3Node * pNode = i3Scene::FindNodeByName( getSceneObject(), szBoneName);
		if( pNode != nullptr)
		{
			pNode->RemoveFlag( I3_NODEFLAG_DISABLE);
			m_BulletList.push_back( pNode);			
		}
	}
}


INT32 WeaponP90Ext::_OnProcessFireBulletCount( void)
{
	INT32 iBullet = WeaponBase::_OnProcessFireBulletCount();

	const size_t nCount = m_BulletList.size();

	if( nCount > 0)
	{
		if( iBullet < (INT32)nCount * 2)
		{
			i3Node * pNode = m_BulletList[0];
			pNode->AddFlag( I3_NODEFLAG_DISABLE);

			i3::vu::erase_index(m_BulletList, 0);
		}
	}

	return iBullet;
}

