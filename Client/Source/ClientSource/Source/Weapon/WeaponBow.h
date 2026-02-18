#if !defined( __WEAPON_BOW_H__)
#define __WEAPON_BOW_H__

	
class WeaponBow : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponBow, WeaponBase);
private:
	bool			m_b1PV;
	INT32			m_nLoadedShell;
	INT32			m_nTotalShell;
	//T_ItemID		m_ArrowItemID;
	T_ItemID		m_ExtensionArrowItemID;
	
protected:
	virtual MainWeapon*	CreateMainWeapon() ;

public:
	WeaponBow();
	virtual ~WeaponBow();

	virtual void	Create( CGameCharaBase * pOwner, T_ItemID Itemid, bool bCreateInWeapon) override;

	virtual void	OnBindResource( void) override;

	virtual void	Reset( void) override;

	virtual void	Transmission(i3Node * pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos /* = nullptr */, bool bToWorld /* = false */) override;

	void			ThrowArrow( VEC3D * pStart, VEC3D * pDir, REAL32 rSpeed, INT32 nNetSlotIdx, bool bExtension);

	void			PlayUIAnim(void);
	void			CalcArrowDirection(void);
	void			CalcBullet(void);

	virtual void	PlayAnimation_AttackReady(void) override;
	virtual void	PlayAnim_UI_IdleB( void) override;
	virtual void	PlayAnim_UI_IdleA( void) override;
	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;

	void			GetTargetDir( VEC3D * pDir);

	INT32			getTotalShellCount( void);
	void			setTotalShellCount( INT32 nCount);

	INT32			getLoadedShellCount( void);
	void			setLoadedShellCount( INT32 nCount);

	T_ItemID		getExtensionArrowItemID() { return m_ExtensionArrowItemID; }

public:
	virtual void	_OnPlayWeaponIdleAnim(void) override;		// 파생가상함수들은 이 기본클래스함수 호출을 요구하지 않고 있다...(일관성 문제가 좀 있다..)

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual	 void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual	 void	OnEndFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual	 void	OnSecondaryFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnSwaping( i3AIContext * pCtx, REAL32 tm) override;
	virtual void OnExtensionActivate( i3AIContext * pCtx, REAL32 tm) override;
	virtual void OnExtensionDeactivate( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

public:

	virtual bool	isBulletFireWeapon( void) override { return false; }

	virtual void	GetNetBullet(GAMEINFO_BULLET* pBullet) override;
	virtual void	SetNetBullet(GAMEINFO_BULLET* pBullet) override;
};

#endif
