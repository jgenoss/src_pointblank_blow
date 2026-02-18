#include "pch.h"
#include "WeaponThrowKnife.h"
#include "GlobalVariables.h"
#include "StageBattle.h"
#include "BattleHud.h"
#include "GameCharaActionContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaBoneContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaWeaponContext.h"
#include "BattleSlotContext.h"
#include "BattleObjContext.h"

#include "MainWeapon_ThrowKnife.h"

I3_CLASS_INSTANCE( WeaponThrowKnife);//, WeaponKnife);

MainWeapon*		WeaponThrowKnife::CreateMainWeapon()
{
	MainWeapon_ThrowKnife * p = new MainWeapon_ThrowKnife(this);
	MEMDUMP_NEW( p, sizeof( MainWeapon_ThrowKnife));
	return p;
}


WeaponThrowKnife::WeaponThrowKnife( void)
{

}

WeaponThrowKnife::~WeaponThrowKnife( void)
{	
}



/*virtual*/ void WeaponThrowKnife::OnBindResource(void)
{
	// WeaponKnife::OnBindResource() 호출시 크래시남
	WeaponBase::OnBindResource();
}

void WeaponThrowKnife::ReturnInstance( void)
{
	WeaponKnife::ReturnInstance();

}

/*virtual*/ void WeaponThrowKnife::Reset( void)
{
	WeaponBase::Reset();

}

/*
void WeaponThrowKnife::UpdateNetworkData2( REAL32 rDeltatime)
{
	VEC3D	vNew;
	i3Vector::Scale( &vNew, &m_vDirection, m_rSpeed * m_rElapsedThrowTime);
	i3Vector::Add( GetPos(), &m_vStartPos, &vNew);
	addY( GetPos(), -9.81f * m_rElapsedThrowTime * m_rElapsedThrowTime * m_pWeaponInfo->GetWeight());

	//i3Matrix::PreRotateX( GetMatrix(), -3.f * rDeltatime * I3_2PI );

	//GlobalFunc::PB_TRACE("UpdateNetworkData2 GetPos() %f %f %f", GetPos()->x, GetPos()->y, GetPos()->z);
}
*/


void WeaponThrowKnife::OnUpdate( REAL32 rDeltaSeconds)
{
	MainWeapon_ThrowKnife* mainWeapon = static_cast<MainWeapon_ThrowKnife*>(GetMainWeapon());

	mainWeapon->OnUpdate_ThrowKnife(rDeltaSeconds);

	WeaponBase::OnUpdate( rDeltaSeconds);
}


/*virtual*/ void WeaponThrowKnife::OnSwaping( i3AIContext * pCtx, REAL32 tm)
{
	WeaponBase::OnSwaping( pCtx, tm);
}
