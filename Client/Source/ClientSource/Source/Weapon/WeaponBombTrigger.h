#if !defined( __WEAPON_BOMB_TRIGGER_H__)
#define __WEAPON_BOMB_TRIGGER_H__

#include "WeaponBase.h"

class WeaponBombTrigger : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponBombTrigger, WeaponBase);
protected:
	WeaponBase *		m_pBomb;			// 트리거에 연결된 무기( 실제 사용되는 것은 아닙니다.)
	WEAPON_CLASS_TYPE	m_nSideWeaponType;
	UINT32				m_uCount;		// 해킹 방지 count

	i3::vector<WeaponBase*>	m_BombList;			// 던진 폭탄에 대한 리스트( 복수개를 던질 수 있는 구조)

protected:
	virtual void	_InitCollideeGroup( void) override;

public:
	WeaponBase *		getBomb( void)									{ return m_pBomb; }
	void				setBomb( WeaponBase * pWeapon)					{ m_pBomb = pWeapon; }

	INT32				getDropedBombCount( void)						{ return (INT32)m_BombList.size(); }
	WeaponBase *		getDropedBomb( INT32 idx)						{ return m_BombList[ idx]; }
	void				removeDropedBomb( WeaponBase * pWeapon)			{ i3::vu::remove(m_BombList, pWeapon); }

	void				setSideWeaponType( WEAPON_CLASS_TYPE type)		{ m_nSideWeaponType = type; }

	// 탄환 발사 형태의 무기
	virtual bool	isBulletFireWeapon( void) override { return false; }
 
public:
	WeaponBombTrigger();
	virtual ~WeaponBombTrigger();

	virtual void	Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon) override;

	virtual void	Reset( void) override;

	virtual void	SetOwner( CGameCharaBase * pOwner) override;

	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner) override;
	virtual void	DetachNode( void ) override;
	virtual void	ReturnInstance() override;

	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	void			ThrowBomb( INT32 nNetSlotIdx);
	virtual void	FireEnd(void) override;

	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE ) override;
	virtual void	NET_FirstThrow( VEC3D * pStartPos, VEC3D * pDir, INT32 nNetSlotIdx) override;
	virtual void	NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nThrowNetIdx) override;

	virtual void	OnSetBlendAlpha( REAL32 val) override;
	virtual void	OnChangeFPSView( void) override;
	virtual void	OnChangeObserverView( void) override;

	virtual REAL32	GetThrowSpeed() override;

	// UI weapon animation
	virtual void	PlayAnim_UI_IdleB() override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndFire(i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

	/*virtual*/ void    ApplyBulletWithDropItem(REAL32 rRate);

private:
	void OnFireDomi(i3AIContext* pCtx, REAL32 tm);

};

#endif
