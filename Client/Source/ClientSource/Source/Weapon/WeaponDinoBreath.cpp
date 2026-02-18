#include "pch.h"
#include "WeaponDinoBreath.h"
#include "GameCharaDino.h"
#include "BattleServerContext.h"
#include "BattleSlotContext.h"
#include "GameCharaAnimContext.h"
#include "GameCharaCameraContext.h"
#include "GameCharaBoneContext.h"
#include "MainWeapon_GrenadeShell.h"

I3_CLASS_INSTANCE( WeaponDinoBreath);

WeaponDinoBreath::WeaponDinoBreath()
{
}

WeaponDinoBreath::~WeaponDinoBreath()
{

}

/*virtual*/ bool WeaponDinoBreath::DinoAttackProcess(REAL32 rAnimTime, REAL32 rTimeScale)
{
	INT32 autoFireCount = getAutoFireCount();

	if( autoFireCount >= m_pWeaponInfo->GetAutoFireCount() )
		return false;

	bool isAttack = false;

	if( rAnimTime > GetDinoAttackTime(autoFireCount) )
	{
		if( getOwner()->isLocal() || getOwner()->isAI() )
			ThrowShell();

		_PlayFireEffectDino();
		autoFireCount++;
		isAttack = true;

		m_pOwner->getAnimContext()->SetUpperTimeScale(rTimeScale);
		
	}

	setAutoFireCount(autoFireCount);

	return isAttack;
}

void WeaponDinoBreath::ThrowShell(void)
{
	if( m_pOwner == nullptr)
		return;

	I3ASSERT(m_pExtWeapon != nullptr);

	WeaponGrenadeShell * pNewWeapon = (WeaponGrenadeShell*) g_pWeaponManager->GetFreeThrowWeapon( WEAPON_CLASS_GRENADESHELL, m_pExtWeapon->getWeaponInfo()->GetNumber());

	if( pNewWeapon == nullptr )
		return;

	VEC3D vTarget, vDir;
	VEC3D* pStart;

	pNewWeapon->SetOwner( m_pOwner);	// 먼저 세팅해야됩니다.	
	pNewWeapon->setPairWeapon(this);

	pStart = i3Matrix::GetPos( ((CGameCharaDino*)m_pOwner)->getFXMatrix() );
	pNewWeapon->GetShotTargetPos( &vTarget);

	i3Vector::Sub( &vDir, &vTarget, pStart);

	i3Vector::Normalize( &vDir, &vDir);

	pNewWeapon->Reset();

	MainWeapon_GrenadeShell* mainWeapon_gs = static_cast<MainWeapon_GrenadeShell*>(pNewWeapon->GetMainWeapon());
	mainWeapon_gs->Throw( pStart, &vDir, pNewWeapon->GetThrowSpeed(), false);

	pNewWeapon->NET_FireSync( &vTarget, WEAPON::SECONDARY_FIRE );

	const GAMEINFO_CHARACTER * pSendBuffer = BattleSlotContext::i()->getGameInfo_Chara( m_pOwner->getCharaNetIndex() );
	pNewWeapon->setNetworkIdx( pSendBuffer->_tNetData._tDomiDinoGrenadeData.getSlot());

	GAMEEVENT::BindNetGrenadeInfo( pNewWeapon->getNetworkIdx(), pNewWeapon, pStart);

	pNewWeapon->SetVisible( true );

	if (pNewWeapon->IsDedicatedHost() )
	{
		GAMETOCONTEXT_INFO_WEAPON_THROW throwInfo(pNewWeapon->getWeaponInfo()->GetItemID(), pNewWeapon->GetPos(), NET_GRENADE_STATE_FLY);

		GAMEEVENT::Write_ThrowingWeapon( pNewWeapon->getNetworkIdx(), &throwInfo);
	}
	else
	{	
		pNewWeapon->SetVisible( false );
		i3Matrix::MakeWithAt( pNewWeapon->GetMatrix(), &vDir, &I3_YAXIS);
		i3Matrix::PreRotateX( pNewWeapon->GetMatrix(), I3_PI2);
	}
}