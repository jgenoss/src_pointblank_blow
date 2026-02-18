/**
 * \file	GameCharaContextBase.h
 *
 * Declares the game chara context base class.
 */


#if !defined( __GAME_CHARA_CONTROL_CONTEXT_H__)
#define __GAME_CHARA_CONTROL_CONTEXT_H__

class CGameCharaBase;
class CGameCharaWeaponContext;
class CGameCharaActionContext;
class CGameCharaAnimContext;
class CGameCharaBoneContext;
class CGameCharaUpdateContext;
class CGameCharaSoundContext;
class CGameCharaInputContext;
class CGameCharaMaterialContext;
class CGameCharaCameraContext;
class CGameCharaMoveContext;
class CGameCharaCollisionContext;
class CGameCharaEquipContext;
class CGameCharaHUDContext;

class CGameCharaContextBase : public i3ElementBase
{
	I3_CLASS_DEFINE( CGameCharaContextBase, i3ElementBase );
protected:
	VAR_LR(CGameCharaBase*,233)	m_pOwner = nullptr;


protected:
	CGameCharaWeaponContext *	getWeaponContext(void);
	CGameCharaActionContext *	getActionContext(void);
	CGameCharaAnimContext *		getAnimContext(void);
	CGameCharaBoneContext *		getBoneContext( void);
	CGameCharaUpdateContext *	getUpdateContext( void);
	CGameCharaSoundContext *	getSoundContext( void);
	CGameCharaInputContext *	getInputContext( void);
	CGameCharaMaterialContext *	getMaterialContext( void);
	CGameCharaCameraContext *	getCameraContext( void);
	CGameCharaMoveContext *		getMoveContext( void);
	CGameCharaCollisionContext * getCollisionContext( void);
	CGameCharaEquipContext *	getEquipContext( void);
	CGameCharaHUDContext *		getHUDContext( void);

	WeaponBase *				getCurrentWeapon( void);
	WeaponBase *				getUsedWeapon( void)	{ if( getCurrentWeapon() == nullptr) return nullptr; return getCurrentWeapon()->getUsedWeapon(); }

public:
	virtual ~CGameCharaContextBase();

	virtual void	OnCreate( CGameCharaBase * pOwner, INT32 defaultView = 0, INT32 defaultNetwork = 0);

	virtual void	OnDestroy( void)		{}

	virtual void	OnReset( void);

	/** \brief 1인칭 캐릭터로 변경 */
	virtual void	OnChange1PV( void) {}

	/** \brief 3인칭 캐릭터로 변경 */
	virtual void	OnChange3PV( void) {}

	/** \brief Local 캐릭터로 변경 */
	virtual void	OnChangeLocal( void) {}

	/** \brief Network 캐릭터로 변경 */
	virtual void	OnChangeNetwork( void) {}

	/** \brief NPC 캐릭터로 변경 */
	virtual void	OnChangeNPC( void) {}

	/** \brief AI 캐릭터로 변경 */
	virtual void	OnChangeAI( void){}

	/** \brief Profile 캐릭터로 변경 */
	virtual void	OnChangeProfile( void) {}

	virtual void	OnChangeFullbody( void) {}

	/** \brief Bone 구조가 변경되면 호출된다. */
	virtual void	OnChangeBoneHierarchy( i3Body * pBody) {}

	/** \brief 공룡 캐릭터로 변경
		\param[in] bTRex TRex 여부 */
	//virtual void	OnChangeDino( bool bTRex) {}
	virtual void	OnChangeTrex() {}
	virtual void	OnChangeSting() {}
	virtual void	OnChangeRaptor() {}

	/** Update 순에 따라 호출됩니다. */
	virtual void	OnFirstUpdate( REAL32 rDeltatime) {}
	virtual void	OnUpdate( REAL32 rDeltatime) {}
	virtual void	OnLastUpdate( REAL32 rDeltatime) {}
};

#endif
