#include "pch.h"
#include "GameCharaContextBase.h"
#include "GameCharaBase.h"
#include "GameCharaContexts.h"

I3_CLASS_INSTANCE( CGameCharaContextBase);

CGameCharaContextBase::~CGameCharaContextBase()
{
	OnDestroy();
}

CGameCharaWeaponContext * CGameCharaContextBase::getWeaponContext(void)	
{ 
	return m_pOwner->getWeaponContext();		
}

CGameCharaActionContext * CGameCharaContextBase::getActionContext(void)	
{ 
	return m_pOwner->getActionContext();		
}

CGameCharaAnimContext * CGameCharaContextBase::getAnimContext(void)		
{ 
	return m_pOwner->getAnimContext();		
}

CGameCharaBoneContext * CGameCharaContextBase::getBoneContext( void)
{
	return m_pOwner->getBoneContext();
}

CGameCharaUpdateContext * CGameCharaContextBase::getUpdateContext( void)
{
	return m_pOwner->getUpdateContext();
}

CGameCharaSoundContext * CGameCharaContextBase::getSoundContext( void)
{
	return m_pOwner->getSoundContext();
}

CGameCharaInputContext * CGameCharaContextBase::getInputContext( void)
{
	return m_pOwner->getInputContext();
}

CGameCharaMaterialContext * CGameCharaContextBase::getMaterialContext( void)
{
	return m_pOwner->getMaterialContext();
}

CGameCharaCameraContext * CGameCharaContextBase::getCameraContext( void)
{
	return m_pOwner->getCameraContext();
}

CGameCharaMoveContext * CGameCharaContextBase::getMoveContext( void)
{
	return m_pOwner->getMoveContext();
}

CGameCharaCollisionContext * CGameCharaContextBase::getCollisionContext( void)
{
	return m_pOwner->getCollisionContext();
}


CGameCharaEquipContext * CGameCharaContextBase::getEquipContext( void)
{
	return m_pOwner->getEquipContext();
}

CGameCharaHUDContext * CGameCharaContextBase::getHUDContext( void)
{
	return m_pOwner->getHUDContext();
}

WeaponBase * CGameCharaContextBase::getCurrentWeapon( void)
{
	return m_pOwner->GetCurrentCharaWeapon();
}

void CGameCharaContextBase::OnCreate( CGameCharaBase * pOwner, INT32 defaultView, INT32 defaultNetwork)
{
	
	m_pOwner = pOwner;

	I3ASSERT_RETURN(m_pOwner);
	// 
	if( m_pOwner->is1PV())
		OnChange1PV();
	else
		OnChange3PV();

	if( m_pOwner->isLocal())
		OnChangeLocal();
	else if( m_pOwner->isNetwork())
		OnChangeNetwork();
	else if( m_pOwner->isNPC())
		OnChangeNPC();

	OnChangeBoneHierarchy( m_pOwner->getSceneObject()->GetBody());
}

void CGameCharaContextBase::OnReset( void)
{
	I3ASSERT( m_pOwner != nullptr);
}


