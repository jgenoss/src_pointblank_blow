/**
 * \file	UI\UIToolTipNonWeapon.h
 *
 * Declares the user interface tool tip non weapon class.
 */


#if !defined( __UI_TOOLTIP_NONWEAPON_H__)
#define __UI_TOOLTIP_NONWEAPON_H__

#include "UI/UIToolTipBase.h"

class UIImgSetCtrl;
class UIPhaseInventory_V11;

class UIToolTipNonWeapon : public UIToolTipBase
{
	I3_CLASS_DEFINE( UIToolTipNonWeapon, UIToolTipBase);
private:
	UIImgSetCtrl *		m_pImageCtrl;
	i3UIFrameWnd *		m_pInfoCtrl0;
	i3UIStaticText *	m_pInfoCtrl1;
	VEC2D				m_pOriginalsize[2];

protected:
	virtual void	_InitializeAtLoad( i3UIScene * pScene);
	virtual void	_ClearAtUnload(i3UIScene * pScene);
	UIPhaseInventory_V11 *	GetInventory( void);

public:
	UIToolTipNonWeapon();
	virtual ~UIToolTipNonWeapon();

	virtual void	OnCreate( i3GameNode * pParent);

	/** \brief √¢¿ª ∂ÁøÛ¥œ¥Ÿ.
		\param[in] pArg1 Inventory¿« SlotIndex¿‘¥œ¥Ÿ. */
	virtual bool	OnEntranceStart( void * pArg1 = NULL, void * pArg2 = NULL);
};

#endif
