#if !defined( __UI_TOOLTIP_WEAPON_H__)
#define __UI_TOOLTIP_WEAPON_H__

#include "UI/UIToolTipBase.h"

class UIImgSetCtrl;
class UIPhaseInventory_V11;

class UIToolTipWeapon : public UIToolTipBase
{
	I3_CLASS_DEFINE( UIToolTipWeapon, UIToolTipBase);
private:
	UIImgSetCtrl *	m_pImageCtrl;

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene);
	virtual void	_ClearAtUnload(i3UIScene * pScene);
	UIPhaseInventory_V11 * GetInventory( void);
	
public:
	UIToolTipWeapon();
	virtual ~UIToolTipWeapon();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);

	void			SetWeaponInfo( T_ItemID num );
};

#endif
