#if !defined( __GAME_CHARA_CONTROL_BASE_H__ )
#define __GAME_CHARA_CONTROL_BASE_H__

#include "GameCharaContexts.h"

#define EXT_CHECK

#ifdef EXT_CHECK
#define EXT_ASSERT(a)		I3ASSERT(a)
#else
#define EXT_ASSERT(a)		{ if(!(a)) return false; }
#endif

class CGameCharaBase;


class CGameCharaControlBase : public i3ElementBase
{
	I3_CLASS_DEFINE( CGameCharaControlBase, i3ElementBase );
protected:
	VAR_LR(CGameCharaBase*,75)	m_pOwner = nullptr;

public:
	void	BindOwner( CGameCharaBase * pOwner) { m_pOwner = pOwner; }

	
protected:
	CGameCharaActionContext *	getActionContext(void)		{ return m_pOwner->getActionContext();	 }
	CGameCharaAnimContext *		getAnimContext(void)		{ return m_pOwner->getAnimContext(); }
	CGameCharaBoneContext *		getBoneContext( void)		{ return m_pOwner->getBoneContext(); }
	CGameCharaCameraContext *	getCameraContext( void)		{ return m_pOwner->getCameraContext(); }
	CGameCharaCollisionContext * getCollisionContext( void)	{ return m_pOwner->getCollisionContext(); }
	CGameCharaEquipContext *	getEquipContext( void)		{ return m_pOwner->getEquipContext(); }
	CGameCharaInputContext *	getInputContext( void)		{ return m_pOwner->getInputContext(); }
	CGameCharaMaterialContext *	getMaterialContext( void)	{ return m_pOwner->getMaterialContext(); }
	CGameCharaMoveContext *		getMoveContext( void)		{ return m_pOwner->getMoveContext(); }
	CGameCharaSoundContext *	getSoundContext( void)		{ return m_pOwner->getSoundContext(); }
	CGameCharaUpdateContext *	getUpdateContext( void)		{ return m_pOwner->getUpdateContext(); }
	CGameCharaWeaponContext *	getWeaponContext(void)		{ return m_pOwner->getWeaponContext(); }
	CGameCharaHUDContext *		getHUDContext( void)		{ return m_pOwner->getHUDContext(); }
	
	WeaponBase *				getUsedWeapon( void)
	{
		CGameCharaWeaponContext * pWeaponCxt = getWeaponContext();

		if (pWeaponCxt == nullptr || pWeaponCxt->getCurrent() == nullptr)
			return nullptr;

		return pWeaponCxt->getCurrent()->getUsedWeapon();
	}
};

#endif
