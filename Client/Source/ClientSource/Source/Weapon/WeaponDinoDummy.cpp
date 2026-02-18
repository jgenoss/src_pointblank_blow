#include "pch.h"

#include "WeaponDinoDummy.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "GameCharaActionContext.h"
#include "GameCharaBoneContext.h"
#include "./Character/GameCharaControlBase.h"
#include "../UI/HUD/UIHudDomi_Tag.h"
#include "WeaponFunction.h"

I3_CLASS_INSTANCE( WeaponDinoDummy)

WeaponDinoDummy::WeaponDinoDummy() 
	: m_Dummy(0), m_pAttrSet(0), m_pWeaponTag(0), m_hp(0), m_max_hp(0), m_installer(0)
{
}

WeaponDinoDummy::~WeaponDinoDummy()
{
	I3_SAFE_RELEASE( m_pWeaponTag);
}

void WeaponDinoDummy::OnBindResource()
{
	i3Chara::OnBindResource();

	m_pAICtx = this->FindAI("Default");
}

void WeaponDinoDummy::_InitCollideeGroup( void)
{
//	InitColliderSet( CGRP_ITEM_WHOLE, CGRP_MASK_ITEM);
//	i3Scene::ModifyDynamicState( getSceneObject(), 0, I3_PHYSIX_STATE_NO_COLLISION);
	WF::SetCollideeGroup_DinoDummy(this);
}

void WeaponDinoDummy::Reset()
{
	WeaponBase::Reset();

	m_StateMaskWeapon = WSM_LOADBULLET_DISABLE | WSM_LOADMAG_DISABLE;
	m_hp = 0;
	m_max_hp = 0;
	m_installer = 0;
}

void WeaponDinoDummy::Create(CGameCharaBase* owner, T_ItemID itemID, bool bCreateInWeapon )
{
	WeaponBase::Create(owner, itemID, bCreateInWeapon);
}

void WeaponDinoDummy::OnUpdate(REAL32 tm)
{
	WeaponBase::OnUpdate(tm);
}

void WeaponDinoDummy::OnNone(i3AIContext* ctx, REAL32 tm)
{
}

void WeaponDinoDummy::OnIdle(i3AIContext* ctx, REAL32 tm)
{
}

void WeaponDinoDummy::OnDestroy(i3AIContext* ctx, REAL32 tm)
{
}

void WeaponDinoDummy::Uninstall()
{
	INT32 effectIdx = g_pEffectManager->FindEffect("Exp_C5_01");
	g_pEffectManager->AddEffect(effectIdx, i3Matrix::GetPos(getSceneObject()->GetCacheMatrix()), &I3_YAXIS);
	g_pSndMng->StartWeapon_C4StateSnd(GTFW_C4_EXPLOSION, GetPos(), false);

	_OnCheckCollisionByExplosion();
	WF::PlayWeaponAnim(this, DUMMY_NONE);
	Reset();
}

void WeaponDinoDummy::InstallToWorld(i3Node* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld, MATRIX* pMat, REAL32 theta)
{
	I3ASSERT( getOwner() != nullptr);

	m_installer	= getOwner();

	VEC3D vWeaponPos = VEC3D( pPos->x, pPos->y, pPos->z);
	
	WeaponBase::Transmission( pNewSceneNode, pNewOwner, &vWeaponPos, true);

	if( SetTagToWeapon( m_pWeaponTag, WEP_TAG_DUMMY ) )
		getAttachNode()->AddChild( m_pWeaponTag);

	WF::PlayWeaponAnim(this, DUMMY_IDLE);
}

void WeaponDinoDummy::ReturnInstance()
{	
	Reset();
	WeaponBase::ReturnInstance();
}

void WeaponDinoDummy::_OnCollision(CGameCharaBase * pChara, i3CollideeLine * pLine, I3_PHYSIX_HIT_RESULT * pResult)
{	
	_OnHit( pResult, pLine, true);
}

/*virtual*/ void WeaponDinoDummy::_OnCheckCollisionByExplosion(void)
{
	INT32 i;
	VEC3D vLength;

	REAL32 rExplosionRange = 30.0f; // 30미터 수동 설정, GetExplosionRange()의 NEW_SKILL_SYSTEM의 getSkillValue()에서 크래쉬 날 수 있음.

	for( i = 0; i < g_pCharaManager->getCharaCount(); ++i)
	{
		CGameCharaBase * pChara = g_pCharaManager->getChara( i);
		if( pChara == nullptr)
			continue;

		i3Vector::Sub( &vLength, GetPos(), i3Matrix::GetPos( pChara->getBoneContext()->getIKMatrix()));
		REAL32 rLength = i3Vector::Length( &vLength );

		if( pChara->IsRex()) rExplosionRange *= 2.0f;

		//수류탄이 터지는 근처에 Owner가 위치해있는경우 카메라 흔들림 효과
		if( (g_pCamera != nullptr) && pChara->is1PV() && !pChara->isCharaStateMask( CHARA_STATE_DEATH))
		{	
			REAL32 MaxLength	= rExplosionRange * 2.0f;
			if( rLength < MaxLength)
			{
				REAL32 TrembAmpRate = 1.0f - rLength/MaxLength;
				g_pCamera->SetTremble( 15.0f * TrembAmpRate * TrembAmpRate, 10.0f * TrembAmpRate, 0.2f, 1.0f *	TrembAmpRate);
			}
		}
	}
}

void WeaponDinoDummy::SetHP(INT16 hp)
{
	//max hp 설정.
	if( m_max_hp < hp)
		m_max_hp = hp;

	//current hp 설정.
	m_hp = hp;
}

REAL32 WeaponDinoDummy::GetHPRatio() const
{
	return (m_max_hp > 0) ? (m_hp / (REAL32)m_max_hp) : (0.f);
}

