#if !defined( _WEAPON_P90EXT_H__)
#define _WEAPON_P90EXT_H__

#include "WeaponDotLaser.h"

#define P90EXT_BULLETBONE_COUNT	25

class WeaponP90Ext : public WeaponDotLaser
{
	I3_CLASS_DEFINE( WeaponP90Ext, WeaponDotLaser);

public:
	i3::vector<i3Node*>	m_BulletList;

protected:
	virtual INT32	_OnProcessFireBulletCount( void) override;

public:
	WeaponP90Ext(void);
	virtual ~WeaponP90Ext(void);

	virtual void	Reset( void) override;
	virtual void	OnBindResource( void) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	AttachNode( i3Node * pSceneNode, i3GameNode * pOwner) override;
	virtual void	DetachNode( void ) override;
	virtual void	OnDrop( void) override;

	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual bool	OnLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndLoadMag( i3AIContext * pCtx, REAL32 tm) override;
	/****************************************************************************/

	virtual void	GetShotTargetPos( VEC3D * pDest, VEC3D * pStart = nullptr) override;
	virtual void	FireCollision( VEC3D * pStartPos, VEC3D * pTargetPos) override;

	void			ResetBullet(void);
};

#endif // _WEAPON_P90EXT_H__