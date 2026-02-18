#if !defined( __WEAPON_MINIAXE_H__)
#define __WEAPON_MINIAXE_H__

class WeaponMiniAxe : public WeaponKnife
{
	I3_CLASS_DEFINE( WeaponMiniAxe, WeaponKnife);
private:
	MainWeapon*			CreateMainWeapon();

protected:

	virtual void	_PlaySwapSound(void) override;


public:
	virtual bool		isBulletFireWeapon( void) override { return false; }

	virtual WeaponBase *	getUsedWeapon( void) override;
	virtual CWeaponInfo *	getRealWeaponInfo( void) override;

	virtual void		setExtensionActivate( bool bVal) override { WeaponBase::setExtensionActivate( !bVal); }

public:
	WeaponMiniAxe();
	virtual ~WeaponMiniAxe();

	virtual void	OnBindResource(void) override;
	virtual void	Reset( void) override;

	virtual void	ReturnInstance( void) override;

	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void	OnSetBlendAlpha( REAL32 val) override;
	virtual void	OnSetMaterialColor( COLORREAL * pfDiffuse, COLORREAL * pfSpecular) override;

	virtual void	OnChangeFPSView( void) override;
	virtual void	OnChangeObserverView( void) override;

	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner) override;
	virtual void	DetachNode( void ) override;

	virtual bool	Fire( WEAPON::FIRE_ORDER type/* = WEAPON::PRIMARY_FIRE*/) override;
	virtual void	FireEnd(void) override;

	virtual void	NET_Fire( VEC3D * pStartPos, VEC3D * pTargetPos, WEAPON::FIRE_ORDER type = WEAPON::PRIMARY_FIRE ) override;
	virtual bool	OnCheckExtensionPossible( bool bVal) override;
	virtual void	OnPlayExtensionAnimation( bool bVal, bool bAnimPlay = true)  override {}
	virtual void	NET_FirstThrow( VEC3D * pStartPos, VEC3D * pDir, INT32 nNetSlotIdx) override;
	virtual void	NET_Throw( VEC3D * pStartPos, VEC3D * pTargetPos, INT32 nThrowNetIdx ) override;

	virtual void	SetOwner( CGameCharaBase * pOwner) override;
	virtual void	FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos) override;
	virtual void	ReturnToReady( void) override;


	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override;		// 발사중일 경우 호출됨
	virtual void	OnSecondaryFire( i3AIContext * pCtx, REAL32 tm) override;


	// UI weapon animation
	virtual void	PlayAnim_UI_IdleB( void) override;
};

#endif
