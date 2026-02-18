#if !defined( __UI_PHASE_INVENTORY_H__)
#define __UI_PHASE_INVENTORY_H__

#include "UIMultiTab.h"
#include "UILBScrollSlot.h"

#define USE_INVEN_SLOT_COUNT	7

struct USE_INVEN_INFO
{
	i3UIImageBox*			m_m_UseInvenFrm;
	i3UIEditBox*			m_UseInvenEdit;
	UIImgSetCtrl*			m_m_UseInvenImg;
};

struct ITEM_INFO;

class UIPhaseInventory : public iMultiTap
{
	I3_CLASS_DEFINE( UIPhaseInventory, iMultiTap);

	friend class InvenSlot;

	enum{ SLIDE_OFFSET = 0, X = 634, WIDTH = 190,	};

private:
	INT32 m_oldClickSlot;

	USE_INVEN_INFO		m_UseInvenInfo[USE_INVEN_SLOT_COUNT];
	bool				m_bEnableUseInven;
	INT32				m_iUseInvenSlotCount;

	i3::scoped_connection_set	m_SigConnection;

	void _RestoreOldSlotClick();
private:
	void cb_finish_buy_extend_item(bool);
	void cb_finish_remove_item(bool, const ITEM_INFO*);
	void cb_finish_repair_item(bool bAck);
	void cb_finish_auth_item(bool, const SHOP_GOODS*);


private:
	virtual void open_menu(INT32 idx) const override;
	virtual void clicked_menu_impl(INT32 idx) override;
	virtual void clicked_tap_impl(INT32 idx1, INT32 idx2) override;
	void clicked_tap_UseInvenInfo(INT32 idx1, INT32 idx2);


	//------------------------------------------------------------------------------//
public:
	UIPhaseInventory();
	virtual ~UIPhaseInventory();

	virtual void OnCreate( i3GameNode * pParent) override;
	virtual void OnLoadAllScenes() override;
	virtual void OnUnloadAllScenes() override;

	virtual void OnUpdate( REAL32 rDeltaSeconds) override;

	virtual void Enter(void) override;
	virtual void Leave(void) override;

	virtual void ProcessGameEvent( GAME_EVENT event, INT32 Arg, const i3::user_data& UserData) override;

public:
	virtual void init_slide_impl() override;
	virtual void clicked_UseInven() override;
};

#endif