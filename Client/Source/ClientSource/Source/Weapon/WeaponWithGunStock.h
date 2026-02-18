#if !defined( __WEAPON_WITH_GUN_STOCK_H__)
#define __WEAPON_WITH_GUN_STOCK_H__

#include "WeaponBase.h"

#define MAX_GUN_STOCK_MODEL		2
#define MAX_WEAPON_LOD			3

class WeaponWithGunStock : public WeaponBase
{
	I3_CLASS_DEFINE( WeaponWithGunStock, WeaponBase);
protected:

	enum IDLE_TYPE
	{
		IDLE_A = 0,
		IDLE_B = 1,

		IDLE_MAX
	};
	
	i3Geometry * m_pGunStockModel[ MAX_WEAPON_LOD][ MAX_GUN_STOCK_MODEL];

	INT8 m_aiiUI_Idle_SWAT_Male[ IDLE_MAX];
	INT8 m_aiiUI_Idle_SWAT_Female[ IDLE_MAX];
	INT8 m_aiiUI_Idle_REBEL_Male[ IDLE_MAX];
	INT8 m_aiiUI_Idle_REBEL_Female[ IDLE_MAX];
	
public:
	WeaponWithGunStock();
	virtual ~WeaponWithGunStock();

	virtual void	PostCreate( void) override;
	virtual void	OnUpdate( REAL32 rDeltaSeconds) override;
	virtual void	OnSetVisibleExtension( bool bVal ) override;
	virtual void	OnDrop( void) override;

	void			SetVisibleGunStock( bool bVal);

	// UI weapon animation
	virtual void	PlayAnim_UI_IdleA( void) override;
	virtual void	PlayAnim_UI_IdleB( void) override;
};

#endif

