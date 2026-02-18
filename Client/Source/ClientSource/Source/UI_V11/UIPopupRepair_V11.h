#if !defined( __UI_POPUP_REPAIR_H__)
#define __UI_POPUP_REPAIR_H__

#include "UI/UIPopupBase.h"
#include "UI/UIPhaseSubBase.h"

class UIImgSetCtrl;

class UIPopupRepair_V11 : public UIPopupBase
{
	I3_CLASS_DEFINE( UIPopupRepair_V11, UIPopupBase);

public:
	UIPopupRepair_V11();
	virtual ~UIPopupRepair_V11();

	virtual void	OnCreate( i3GameNode * pParent);

	virtual bool OnEntranceStart(void * pArg1 = NULL, void * pArg2 = NULL);
	virtual void OnEntranceEnd(void);
	virtual void OnLoadAllScenes(void);

	virtual bool	OnKey_Enter()	{ RepairConfirm();	return true; }
	virtual bool	OnKey_Escape()	{ _ClosePopup();	return true; }
	virtual void	OnExitKey(void)	{ _ClosePopup(); }

	void	RepairConfirm(void)		{ if(m_CanRepair) _SendBuyEvent(); else _ClosePopup(); }
	void	InputCashCharge(void);
	
	UINT32	GetRepairItemId(void) const { return m_ItemID; }

protected:
	UIImgSetCtrl*	m_pImgCtrl;

	UINT32	m_ItemID;
	BOOL	m_CanRepair;

	BOOL	m_bConfirm;
	REAL32	m_fTimeout;

protected:
	void	_ClosePopup(void);
	void	_SendBuyEvent(void);
	BOOL	_AuthGoods(SHOP_GOODS * pGoods);
	bool	_SetRepairData(void);
};

#endif
