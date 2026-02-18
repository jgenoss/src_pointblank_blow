#if !defined( _WEAPON_WETARMELON_H__)
#define _WEAPON_WETARMELON_H__

#include "weaponbase.h"

class WeaponWatermelon :	public WeaponBase
{
	I3_CLASS_DEFINE( WeaponWatermelon, WeaponBase);

protected:
	INT8	m_aii_IDLE_Female;
	INT8	m_aii_IDLE_Male;

	MATRIX			m_mtxOld;
	
protected:
	virtual void	_InitCollideeGroup( void) override;
	virtual void	_OnCheckCollisionByExplosion( void) override;

public:
	WeaponWatermelon(void);
	virtual ~WeaponWatermelon(void);

	virtual void	PostCreate( void) override;
	virtual void	OnIdle( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	Explosion() override;
	virtual void	Reset() override;
	//virtual void	Fire(void) override;
	virtual void	FireEnd(void) override;
	virtual void	ReturnToReady( void) override;
	
	/****************************************************************************/
	/*						Animation Callback function							*/
	/****************************************************************************/
	virtual void	OnFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnEndFire( i3AIContext * pCtx, REAL32 tm) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	/****************************************************************************/

	virtual bool	OnLostDevice( bool bLostDevice) override;
	virtual bool	OnRevive( i3RenderContext * pCtx) override;

	void			DropToWorld(i3Node* pNewSceneNode, i3GameNode * pNewOwner, VEC3D * pPos, bool bToWorld, MATRIX* pMat);
};

#endif